/**
 * @file stub_intercepts.cpp
 * @brief Register all built-in stub intercept handlers.
 *
 * Each handler pops typed arguments from the operand stack, performs
 * the operation, and pushes the result (if any) back.
 *
 * Key conventions:
 * - System.out sentinel = &kOut, System.err sentinel = &kErr
 * - String reference is always a std::string* (interned via StringPool)
 * - Methods returning String must intern the result before pushing
 */

#include "stub_intercepts.hpp"

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "oops/string_pool.hpp"
#include "runtime/operand_stack.hpp"
#include "utilities/logger.hpp"
#include "utilities/types.hpp"

namespace jvm::engine {
namespace {

// ====================================================================
//  Helpers
// ====================================================================

/// Sentinel values to distinguish System.out from System.err.
alignas(64) char kOut;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
alignas(64) char kErr;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

/// Dispatch to std::cout or std::cerr based on the sentinel receiver.
std::ostream& streamFor(Jref recv) { return recv == &kErr ? std::cerr : std::cout; }

// ====================================================================
//  Object.<init> — INVOKESPECIAL
// ====================================================================

void objectInit(runtime::OperandStack& s) { s.popRef(); }

// ====================================================================
//  System.out / System.err — GETSTATIC
// ====================================================================

void systemOut(runtime::OperandStack& s) { s.pushRef(&kOut); }

void systemErr(runtime::OperandStack& s) { s.pushRef(&kErr); }

// ====================================================================
//  PrintStream.print/println — INVOKEVIRTUAL
// ====================================================================

void printlnString(runtime::OperandStack& s) {
  auto* str  = static_cast<std::string*>(s.popRef());  // value (String)
  auto* recv = s.popRef();                             // PrintStream receiver (sentinel)
  streamFor(recv) << *str << '\n';
}

void printString(runtime::OperandStack& s) {
  auto* str  = static_cast<std::string*>(s.popRef());
  auto* recv = s.popRef();
  streamFor(recv) << *str;
}

void printlnInt(runtime::OperandStack& s) {
  auto  value = s.popInt();
  auto* recv  = s.popRef();
  streamFor(recv) << value << '\n';
}

void printInt(runtime::OperandStack& s) {
  auto  value = s.popInt();
  auto* recv  = s.popRef();
  streamFor(recv) << value;
}

void printlnLong(runtime::OperandStack& s) {
  auto  value = s.popLong();
  auto* recv  = s.popRef();
  streamFor(recv) << value << '\n';
}

void printLong(runtime::OperandStack& s) {
  auto  value = s.popLong();
  auto* recv  = s.popRef();
  streamFor(recv) << value;
}

void printlnFloat(runtime::OperandStack& s) {
  auto  value = s.popFloat();
  auto* recv  = s.popRef();
  streamFor(recv) << value << '\n';
}

void printFloat(runtime::OperandStack& s) {
  auto  value = s.popFloat();
  auto* recv  = s.popRef();
  streamFor(recv) << value;
}

void printlnDouble(runtime::OperandStack& s) {
  auto  value = s.popDouble();
  auto* recv  = s.popRef();
  streamFor(recv) << value << '\n';
}

void printDouble(runtime::OperandStack& s) {
  auto  value = s.popDouble();
  auto* recv  = s.popRef();
  streamFor(recv) << value;
}

void printlnBoolean(runtime::OperandStack& s) {
  auto  value = s.popInt();  // boolean stored as int
  auto* recv  = s.popRef();
  streamFor(recv) << (value != 0 ? "true" : "false") << '\n';
}

void printBoolean(runtime::OperandStack& s) {
  auto  value = s.popInt();
  auto* recv  = s.popRef();
  streamFor(recv) << (value != 0 ? "true" : "false");
}

void printlnChar(runtime::OperandStack& s) {
  auto  value = static_cast<char>(s.popInt());  // char stored as int
  auto* recv  = s.popRef();
  streamFor(recv) << value << '\n';
}

void printChar(runtime::OperandStack& s) {
  auto  value = static_cast<char>(s.popInt());
  auto* recv  = s.popRef();
  streamFor(recv) << value;
}

// ====================================================================
//  String read-only methods — INVOKEVIRTUAL on std::string*
// ====================================================================

void stringLength(runtime::OperandStack& s) {
  auto* str = static_cast<std::string*>(s.popRef());
  s.pushInt(static_cast<Jint>(str->size()));
}

void stringCharAt(runtime::OperandStack& s) {
  auto  index = s.popInt();
  auto* str   = static_cast<std::string*>(s.popRef());
  s.pushInt(static_cast<Jint>(static_cast<Jchar>((*str)[static_cast<size_t>(index)])));
}

void stringIsEmpty(runtime::OperandStack& s) {
  auto* str = static_cast<std::string*>(s.popRef());
  s.pushInt(str->empty() ? 1 : 0);
}

void stringEquals(runtime::OperandStack& s) {
  auto* other = static_cast<std::string*>(s.popRef());  // argument Object (std::string*)
  auto* self  = static_cast<std::string*>(s.popRef());  // receiver
  s.pushInt((self != nullptr && other != nullptr && *self == *other) ? 1 : 0);
}

void stringHashCode(runtime::OperandStack& s) {
  auto* str = static_cast<std::string*>(s.popRef());
  // Java String hashCode: s[0]*31^(n-1) + s[1]*31^(n-2) + ... + s[n-1]
  Jint hash = 0;
  for (size_t i = 0; i < str->size(); ++i) {
    hash = 31 * hash + static_cast<Jchar>((*str)[i]);  // char is unsigned
  }
  s.pushInt(hash);
}

void stringCompareTo(runtime::OperandStack& s) {
  auto* other = static_cast<std::string*>(s.popRef());
  auto* self  = static_cast<std::string*>(s.popRef());
  if (self == nullptr || other == nullptr) {
    s.pushInt(0);
    return;
  }
  size_t min_len = std::min(self->size(), other->size());
  for (size_t i = 0; i < min_len; ++i) {
    if ((*self)[i] != (*other)[i]) {
      s.pushInt(
        static_cast<Jint>(static_cast<Jchar>((*self)[i]) - static_cast<Jchar>((*other)[i])));
      return;
    }
  }
  s.pushInt(static_cast<Jint>(self->size() - other->size()));
}

// ====================================================================
//  Static utility methods — INVOKESTATIC
// ====================================================================

void systemCurrentTimeMillis(runtime::OperandStack& s) {
  auto now = std::chrono::system_clock::now();
  auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
  s.pushLong(static_cast<Jlong>(ms));
}

void systemNanoTime(runtime::OperandStack& s) {
  auto ns = std::chrono::steady_clock::now().time_since_epoch().count();
  s.pushLong(static_cast<Jlong>(ns));
}

void systemExit(runtime::OperandStack& s) {
  auto status = s.popInt();
  LOG_WARN("System.exit(", status, ") called \u2014 terminating VM");
  std::exit(static_cast<int>(status));
}

void integerParseInt(runtime::OperandStack& s) {
  auto* str = static_cast<std::string*>(s.popRef());
  if (str == nullptr) {
    throw std::runtime_error("NullPointerException in Integer.parseInt");
  }
  s.pushInt(static_cast<Jint>(std::stoi(*str)));
}

void integerToString(runtime::OperandStack& s) {
  auto        value    = s.popInt();
  auto        result   = std::to_string(value);
  const auto* interned = oops::StringPool::getSingleton().intern(result);
  s.pushRef(const_cast<std::string*>(interned));
}

void mathAbsInt(runtime::OperandStack& s) {
  auto value = s.popInt();
  s.pushInt(value < 0 ? -value : value);  // Note: overflow on MIN_VALUE returns MIN_VALUE
}

void mathAbsLong(runtime::OperandStack& s) {
  auto value = s.popLong();
  s.pushLong(value < 0 ? -value : value);
}

void mathAbsFloat(runtime::OperandStack& s) {
  auto value = s.popFloat();
  s.pushFloat(std::abs(value));
}

void mathAbsDouble(runtime::OperandStack& s) {
  auto value = s.popDouble();
  s.pushDouble(std::abs(value));
}

void mathMaxInt(runtime::OperandStack& s) {
  auto b = s.popInt();
  auto a = s.popInt();
  s.pushInt(a > b ? a : b);
}

void mathMinInt(runtime::OperandStack& s) {
  auto b = s.popInt();
  auto a = s.popInt();
  s.pushInt(a < b ? a : b);
}

void mathMaxLong(runtime::OperandStack& s) {
  auto b = s.popLong();
  auto a = s.popLong();
  s.pushLong(a > b ? a : b);
}

void mathMinLong(runtime::OperandStack& s) {
  auto b = s.popLong();
  auto a = s.popLong();
  s.pushLong(a < b ? a : b);
}

void mathSqrt(runtime::OperandStack& s) {
  auto value = s.popDouble();
  s.pushDouble(std::sqrt(value));
}

}  // namespace

void registerStubIntercepts() {
  auto& t = StubIntercepts::getSingleton();

  // ==================================================================
  //  Object.<init>
  // ==================================================================
  t.bind("java/lang/Object.<init> ()V", &objectInit);

  // ==================================================================
  //  System.out / System.err (GETSTATIC)
  // ==================================================================
  t.bind("java/lang/System.out Ljava/io/PrintStream;", &systemOut);
  t.bind("java/lang/System.err Ljava/io/PrintStream;", &systemErr);

  // ==================================================================
  //  PrintStream.print/println  (INVOKEVIRTUAL)
  // ==================================================================
  t.bind("java/io/PrintStream.println (Ljava/lang/String;)V", &printlnString);
  t.bind("java/io/PrintStream.print (Ljava/lang/String;)V", &printString);
  t.bind("java/io/PrintStream.println (I)V", &printlnInt);
  t.bind("java/io/PrintStream.print (I)V", &printInt);
  t.bind("java/io/PrintStream.println (J)V", &printlnLong);
  t.bind("java/io/PrintStream.print (J)V", &printLong);
  t.bind("java/io/PrintStream.println (F)V", &printlnFloat);
  t.bind("java/io/PrintStream.print (F)V", &printFloat);
  t.bind("java/io/PrintStream.println (D)V", &printlnDouble);
  t.bind("java/io/PrintStream.print (D)V", &printDouble);
  t.bind("java/io/PrintStream.println (Z)V", &printlnBoolean);
  t.bind("java/io/PrintStream.print (Z)V", &printBoolean);
  t.bind("java/io/PrintStream.println (C)V", &printlnChar);
  t.bind("java/io/PrintStream.print (C)V", &printChar);

  // ==================================================================
  //  String instance methods  (INVOKEVIRTUAL on std::string*)
  // ==================================================================
  t.bind("java/lang/String.length ()I", &stringLength);
  t.bind("java/lang/String.charAt (I)C", &stringCharAt);
  t.bind("java/lang/String.isEmpty ()Z", &stringIsEmpty);
  t.bind("java/lang/String.equals (Ljava/lang/Object;)Z", &stringEquals);
  t.bind("java/lang/String.hashCode ()I", &stringHashCode);
  t.bind("java/lang/String.compareTo (Ljava/lang/String;)I", &stringCompareTo);

  // ==================================================================
  //  Static utility methods  (INVOKESTATIC)
  // ==================================================================
  t.bind("java/lang/System.currentTimeMillis ()J", &systemCurrentTimeMillis);
  t.bind("java/lang/System.nanoTime ()J", &systemNanoTime);
  t.bind("java/lang/System.exit (I)V", &systemExit);
  t.bind("java/lang/Integer.parseInt (Ljava/lang/String;)I", &integerParseInt);
  t.bind("java/lang/Integer.toString (I)Ljava/lang/String;", &integerToString);
  t.bind("java/lang/Math.abs (I)I", &mathAbsInt);
  t.bind("java/lang/Math.abs (J)J", &mathAbsLong);
  t.bind("java/lang/Math.abs (F)F", &mathAbsFloat);
  t.bind("java/lang/Math.abs (D)D", &mathAbsDouble);
  t.bind("java/lang/Math.max (II)I", &mathMaxInt);
  t.bind("java/lang/Math.min (II)I", &mathMinInt);
  t.bind("java/lang/Math.max (JJ)J", &mathMaxLong);
  t.bind("java/lang/Math.min (JJ)J", &mathMinLong);
  t.bind("java/lang/Math.sqrt (D)D", &mathSqrt);
}
}  // namespace jvm::engine