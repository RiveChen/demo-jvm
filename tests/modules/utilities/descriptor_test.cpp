#include "utilities/descriptor.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

using namespace jvm::descriptor;

// ============================================================================
// Field types — base types
// ============================================================================

TEST(DescriptorFieldTypeTest, BaseIntegralKinds) {
  // byte / char / int / short / boolean all collapse to Int
  EXPECT_EQ(parseFieldType("B"), TypeKind::Int);
  EXPECT_EQ(parseFieldType("C"), TypeKind::Int);
  EXPECT_EQ(parseFieldType("I"), TypeKind::Int);
  EXPECT_EQ(parseFieldType("S"), TypeKind::Int);
  EXPECT_EQ(parseFieldType("Z"), TypeKind::Int);
}

TEST(DescriptorFieldTypeTest, FloatLongDouble) {
  EXPECT_EQ(parseFieldType("F"), TypeKind::Float);
  EXPECT_EQ(parseFieldType("J"), TypeKind::Long);
  EXPECT_EQ(parseFieldType("D"), TypeKind::Double);
}

// ============================================================================
// Field types — object & array (always a reference)
// ============================================================================

TEST(DescriptorFieldTypeTest, ObjectReference) {
  EXPECT_EQ(parseFieldType("Ljava/lang/String;"), TypeKind::Ref);
  EXPECT_EQ(parseFieldType("Ljava/lang/Object;"), TypeKind::Ref);
}

TEST(DescriptorFieldTypeTest, ArrayIsReference) {
  EXPECT_EQ(parseFieldType("[I"), TypeKind::Ref);
  EXPECT_EQ(parseFieldType("[[I"), TypeKind::Ref);
  EXPECT_EQ(parseFieldType("[[[D"), TypeKind::Ref);
  EXPECT_EQ(parseFieldType("[Ljava/lang/String;"), TypeKind::Ref);
  EXPECT_EQ(parseFieldType("[[Ljava/lang/Object;"), TypeKind::Ref);
}

// ============================================================================
// slotCount / isCategory2
// ============================================================================

TEST(DescriptorSlotTest, SlotCount) {
  EXPECT_EQ(slotCount(TypeKind::Int), 1);
  EXPECT_EQ(slotCount(TypeKind::Float), 1);
  EXPECT_EQ(slotCount(TypeKind::Ref), 1);
  EXPECT_EQ(slotCount(TypeKind::Long), 2);
  EXPECT_EQ(slotCount(TypeKind::Double), 2);
  EXPECT_EQ(slotCount(TypeKind::Void), 0);
}

TEST(DescriptorSlotTest, IsCategory2) {
  EXPECT_TRUE(isCategory2(TypeKind::Long));
  EXPECT_TRUE(isCategory2(TypeKind::Double));
  EXPECT_FALSE(isCategory2(TypeKind::Int));
  EXPECT_FALSE(isCategory2(TypeKind::Float));
  EXPECT_FALSE(isCategory2(TypeKind::Ref));
  EXPECT_FALSE(isCategory2(TypeKind::Void));
}

// ============================================================================
// Method descriptors
// ============================================================================

TEST(DescriptorMethodTest, NoArgsVoid) {
  auto m = parseMethodDescriptor("()V");
  EXPECT_TRUE(m.params.empty());
  EXPECT_EQ(m.ret, TypeKind::Void);
}

TEST(DescriptorMethodTest, SingleIntReturnsInt) {
  auto m = parseMethodDescriptor("(I)I");
  ASSERT_EQ(m.params.size(), 1U);
  EXPECT_EQ(m.params[0], TypeKind::Int);
  EXPECT_EQ(m.ret, TypeKind::Int);
}

TEST(DescriptorMethodTest, IntLongReturnsVoid) {
  auto m = parseMethodDescriptor("(IJ)V");
  ASSERT_EQ(m.params.size(), 2U);
  EXPECT_EQ(m.params[0], TypeKind::Int);
  EXPECT_EQ(m.params[1], TypeKind::Long);
  EXPECT_EQ(m.ret, TypeKind::Void);
}

TEST(DescriptorMethodTest, LongDoubleReturnsLong) {
  auto m = parseMethodDescriptor("(JD)J");
  ASSERT_EQ(m.params.size(), 2U);
  EXPECT_EQ(m.params[0], TypeKind::Long);
  EXPECT_EQ(m.params[1], TypeKind::Double);
  EXPECT_EQ(m.ret, TypeKind::Long);
}

TEST(DescriptorMethodTest, ArrayParamVoid) {
  auto m = parseMethodDescriptor("([Ljava/lang/String;)V");
  ASSERT_EQ(m.params.size(), 1U);
  EXPECT_EQ(m.params[0], TypeKind::Ref);
  EXPECT_EQ(m.ret, TypeKind::Void);
}

TEST(DescriptorMethodTest, ObjectParamsObjectReturn) {
  auto m = parseMethodDescriptor("(Ljava/lang/String;I)Ljava/lang/Object;");
  ASSERT_EQ(m.params.size(), 2U);
  EXPECT_EQ(m.params[0], TypeKind::Ref);
  EXPECT_EQ(m.params[1], TypeKind::Int);
  EXPECT_EQ(m.ret, TypeKind::Ref);
}

TEST(DescriptorMethodTest, AllBaseKindsMixed) {
  // (byte, char, float, double, long, boolean) -> void
  auto m = parseMethodDescriptor("(BCFDJZ)V");
  ASSERT_EQ(m.params.size(), 6U);
  EXPECT_EQ(m.params[0], TypeKind::Int);     // B
  EXPECT_EQ(m.params[1], TypeKind::Int);     // C
  EXPECT_EQ(m.params[2], TypeKind::Float);   // F
  EXPECT_EQ(m.params[3], TypeKind::Double);  // D
  EXPECT_EQ(m.params[4], TypeKind::Long);    // J
  EXPECT_EQ(m.params[5], TypeKind::Int);     // Z
  EXPECT_EQ(m.ret, TypeKind::Void);
}

TEST(DescriptorMethodTest, MixedArrayParamsBooleanReturn) {
  auto m = parseMethodDescriptor("([I[[Ljava/lang/Object;)Z");
  ASSERT_EQ(m.params.size(), 2U);
  EXPECT_EQ(m.params[0], TypeKind::Ref);
  EXPECT_EQ(m.params[1], TypeKind::Ref);
  EXPECT_EQ(m.ret, TypeKind::Int);  // Z -> Int
}

TEST(DescriptorMethodTest, StructEquality) {
  auto       m = parseMethodDescriptor("(IJ)V");
  MethodType expected{
    .params = {TypeKind::Int, TypeKind::Long}, .ret = TypeKind::Void, .arg_slot_count = 3};
  EXPECT_TRUE(m == expected);
}

// ============================================================================
// argSlotCount (excludes receiver; long/double count as 2)
// ============================================================================

TEST(DescriptorArgSlotTest, Counts) {
  EXPECT_EQ((parseMethodDescriptor("()V")).arg_slot_count, 0);
  EXPECT_EQ((parseMethodDescriptor("(I)I")).arg_slot_count, 1);
  EXPECT_EQ((parseMethodDescriptor("(IJ)V")).arg_slot_count, 3);   // 1 + 2
  EXPECT_EQ((parseMethodDescriptor("(JD)J")).arg_slot_count, 4);   // 2 + 2
  EXPECT_EQ((parseMethodDescriptor("(DDD)V")).arg_slot_count, 6);  // 2 * 3
  EXPECT_EQ((parseMethodDescriptor("([Ljava/lang/String;)V")).arg_slot_count, 1);
  EXPECT_EQ((parseMethodDescriptor("(BCFDJZ)V")).arg_slot_count,
            8);  // 1+1+1+2+2+1
}

// ============================================================================
// Malformed descriptors must throw (deterministic error paths, not asserts)
// ============================================================================

TEST(DescriptorErrorTest, InvalidBaseChar) {
  EXPECT_THROW(parseFieldType("Q"), std::runtime_error);
}

TEST(DescriptorErrorTest, UnterminatedObjectRef) {
  EXPECT_THROW(parseFieldType("Ljava/lang/String"), std::runtime_error);
}

TEST(DescriptorErrorTest, ArrayWithNoComponent) {
  EXPECT_THROW(parseFieldType("["), std::runtime_error);
}

TEST(DescriptorErrorTest, MethodMissingCloseParen) {
  EXPECT_THROW(parseMethodDescriptor("(I"), std::runtime_error);
}

TEST(DescriptorErrorTest, MethodInvalidParamChar) {
  EXPECT_THROW(parseMethodDescriptor("(Q)V"), std::runtime_error);
}
