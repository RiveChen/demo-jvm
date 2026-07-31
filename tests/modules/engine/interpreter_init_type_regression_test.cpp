/**
 * @file interpreter_init_type_regression_test.cpp
 * @brief Regression tests for four latent bugs in class init / type checks.
 *
 * Each case is designed to FAIL on the current implementation and PASS once the
 * corresponding fix lands:
 *
 *   B1  pc reset to 0 after initialize() with no <clinit> frame pushed
 *       -> fix: `pc = thread->getCurrentFrame().getPC();`
 *   B2  isInstanceOf misses a superclass-implemented interface
 *       -> fix: recurse on super in InstanceKlass::isInstanceOf
 *   B3  <clinit> looked up via findMethod inherits the super's <clinit>
 *       -> fix: look up "<clinit>" in this class's methods only
 *   B4  sub <clinit> runs before super <clinit>
 *       -> fix: push own <clinit> frame before recursing into
 * super.initialize()
 */

#include "interpreter_test_base.hpp"
#include "oops/klass.hpp"
#include "utilities/types.hpp"

#include <gtest/gtest.h>

using namespace jvm;

namespace {

class InterpreterInitTypeRegressionTest : public InterpreterTestBase {
 public:
  static constexpr const char* kDriver          = "tests.data.java.RegressionInitTypeTest";
  static constexpr const char* kDerivedNoClinit = "tests.data.java.RIT_DerivedNoClinit";
};

// --- B1: work before a no-<clinit> static trigger must not be re-executed ----
TEST_F(InterpreterInitTypeRegressionTest, B1_NoRestartOnNoClinitTrigger) {
  // Correct: the increment happens once (1). Bug: pc resets to 0, the method
  // re-runs, and the increment happens twice (2).
  EXPECT_EQ(executeStaticMethod<Jint>(kDriver, "b1NoRestart"), 1)
    << "instruction stream before a no-<clinit> static trigger was re-executed "
       "(pc reset to 0 instead of retrying the trigger)";
}

// --- B2: instanceof must see interfaces implemented by a superclass ----------
TEST_F(InterpreterInitTypeRegressionTest, B2_InstanceOfSuperInterface) {
  // RIT_Sub extends RIT_Impl implements RIT_Iface.
  EXPECT_TRUE(executeStaticMethod<Jboolean>(kDriver, "b2SubIsIface"))
    << "isInstanceOf missed an interface implemented by a superclass";
}

// --- B4: superclass <clinit> must run before subclass <clinit> ---------------
TEST_F(InterpreterInitTypeRegressionTest, B4_SuperClinitBeforeSubClinit) {
  // Base contributes digit 1, Derived digit 2. Correct order => 12; sub-first
  // => 21.
  EXPECT_EQ(executeStaticMethod<Jint>(kDriver, "b4SuperBeforeSub"), 12)
    << "subclass <clinit> ran before superclass <clinit> (expected 12, buggy "
       "21)";
}

// --- B3: a subclass without its own <clinit> must reach FullyInitialized
// ------
TEST_F(InterpreterInitTypeRegressionTest, B3_SubclassWithoutClinitFullyInitialized) {
  // Trigger init of RIT_DerivedNoClinit (extends RIT_Base, which HAS a
  // <clinit>).
  //
  // NOTE: with the bug present in a build that keeps asserts (debug), the
  // second markFullyInitialized(RIT_Base) will abort the process -- that is
  // itself a detection of B3. In a release build the assert is a no-op and the
  // subclass is left in BeingInitialized, which the EXPECT below catches.
  executeStaticMethod<Jint>(kDriver, "b3TriggerNoClinit");

  auto* k = loader_->loadClass(kDerivedNoClinit);
  ASSERT_NE(k, nullptr);
  EXPECT_EQ(k->getState(), oops::InstanceKlass::FullyInitialized)
    << "subclass without its own <clinit> did not reach FullyInitialized "
       "(super <clinit> was inherited via findMethod and re-run)";
}

}  // namespace
