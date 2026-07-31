/**
 * @file array_klass_test.cpp
 * @brief Phase A array-klass plumbing.
 *
 * Covers the MethodArea array-klass factory (dedup + names + element size),
 * Klass::getDescriptorName, multi-dimensional element linkage (the building
 * blocks that arrayKlassForName composes), and the isInstanceOf matrix.
 *
 * Note: java/lang/String and java/lang/Object are not loadable here (the lang/
 * dir is empty), so real loadable test classes are used as element types, and
 * the "instanceof Object" branch is exercised at the interpreter level once
 * array creation exists.
 */

#include "oops/klass.hpp"
#include "oops/method_area.hpp"
#include "oops_test_base.hpp"
#include "utilities/basic_type.hpp"

#include <gtest/gtest.h>

using namespace jvm;

namespace {

class ArrayKlassTest : public OopsTestBase {};

// --- TypeArrayKlass: name, element size, kind, dedup -------------------------
TEST_F(ArrayKlassTest, TypeArrayNameSizeKindDedup) {
  auto& ma   = oops::MethodArea::getSingleton();
  auto* ints = ma.getOrCreateTypeArrayKlass(T_INT);
  ASSERT_NE(ints, nullptr);
  EXPECT_EQ(ints->getName(), "[I");
  EXPECT_EQ(ints->elementSize(), 4);
  EXPECT_EQ(ints->kind(), oops::Klass::Kind::TypeArray);
  // dedup: same name must return the same singleton (required for isInstanceOf)
  EXPECT_EQ(ma.getOrCreateTypeArrayKlass(T_INT), ints);
}

TEST_F(ArrayKlassTest, TypeArrayAllPrimitives) {
  auto& ma = oops::MethodArea::getSingleton();
  struct Case {
    BasicType   t;
    const char* name;
    Jint        size;
  };
  const Case cases[] = {
    {T_BOOLEAN, "[Z", 1}, {T_BYTE, "[B", 1},  {T_CHAR, "[C", 2}, {T_SHORT, "[S", 2},
    {T_INT, "[I", 4},     {T_FLOAT, "[F", 4}, {T_LONG, "[J", 8}, {T_DOUBLE, "[D", 8},
  };
  for (const auto& c : cases) {
    auto* k = ma.getOrCreateTypeArrayKlass(c.t);
    EXPECT_EQ(k->getName(), c.name);
    EXPECT_EQ(k->elementSize(), c.size) << "element size for " << c.name;
  }
}

// --- getDescriptorName -------------------------------------------------------
TEST_F(ArrayKlassTest, DescriptorNames) {
  auto& ma   = oops::MethodArea::getSingleton();
  auto* inst = loader_->loadClass("tests.data.java.KlassTestData");
  ASSERT_NE(inst, nullptr);
  // instance -> "Lname;"
  EXPECT_EQ(inst->getDescriptorName(), "Ltests/data/java/KlassTestData;");
  // primitive array -> the name is already the descriptor
  EXPECT_EQ(ma.getOrCreateTypeArrayKlass(T_INT)->getDescriptorName(), "[I");
  // obj array -> "[" + component descriptor
  EXPECT_EQ(ma.getOrCreateObjArrayKlass(inst)->getDescriptorName(),
            "[Ltests/data/java/KlassTestData;");
}

// --- ObjArrayKlass: name, element linkage, dedup ----------------------------
TEST_F(ArrayKlassTest, ObjArrayNameLinkageDedup) {
  auto& ma   = oops::MethodArea::getSingleton();
  auto* inst = loader_->loadClass("tests.data.java.KlassTestData");
  ASSERT_NE(inst, nullptr);
  auto* arr = ma.getOrCreateObjArrayKlass(inst);
  ASSERT_NE(arr, nullptr);
  EXPECT_EQ(arr->getName(), "[Ltests/data/java/KlassTestData;");
  EXPECT_EQ(arr->kind(), oops::Klass::Kind::ObjArray);
  EXPECT_EQ(arr->elementKlass(), inst);
  EXPECT_EQ(arr->elementSize(), static_cast<Jint>(sizeof(void*)));
  EXPECT_EQ(ma.getOrCreateObjArrayKlass(inst), arr);  // dedup
}

// --- Multi-dimensional building (the pieces arrayKlassForName composes)
// -------
TEST_F(ArrayKlassTest, MultiDimObjArrayWrapsTypeArray) {
  auto& ma   = oops::MethodArea::getSingleton();
  auto* ints = ma.getOrCreateTypeArrayKlass(T_INT);  // [I
  auto* int2 = ma.getOrCreateObjArrayKlass(ints);    // [[I
  EXPECT_EQ(int2->getName(), "[[I");
  EXPECT_EQ(int2->getDescriptorName(), "[[I");
  EXPECT_EQ(int2->elementKlass(), ints);  // element is the [I singleton
}

// --- isInstanceOf matrix (structural; recursive covariance is deferred) ------
TEST_F(ArrayKlassTest, IsInstanceOfSameAndDifferentElement) {
  auto& ma    = oops::MethodArea::getSingleton();
  auto* ints  = ma.getOrCreateTypeArrayKlass(T_INT);
  auto* longs = ma.getOrCreateTypeArrayKlass(T_LONG);
  EXPECT_TRUE(ints->isInstanceOf(ints));    // same singleton
  EXPECT_FALSE(ints->isInstanceOf(longs));  // different primitive array

  auto* a = loader_->loadClass("tests.data.java.KlassTestData");
  auto* b = loader_->loadClass("tests.data.java.Animal");
  ASSERT_NE(a, nullptr);
  ASSERT_NE(b, nullptr);
  auto* arrA = ma.getOrCreateObjArrayKlass(a);
  auto* arrB = ma.getOrCreateObjArrayKlass(b);
  EXPECT_TRUE(
    arrA->isInstanceOf(ma.getOrCreateObjArrayKlass(a)));  // same element -> same singleton
  EXPECT_FALSE(arrA->isInstanceOf(arrB));                 // different element (exact-match)
  // NOTE: recursive covariance ([Sub] <: [Super]) is a known gap (exact-match
  // only); add a case here once ObjArrayKlass::isInstanceOf walks the element
  // hierarchy.
}

}  // namespace
