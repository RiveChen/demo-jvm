#include "runtime/local_variables.h"

#include <gtest/gtest.h>

namespace jvm::runtime {

TEST(LocalVariablesTest, SetAndGetInt) {
  LocalVariables variables(1);
  variables.setInt(0, 1);
  EXPECT_EQ(variables.getInt(0), 1);
}

TEST(LocalVariablesTest, SetAndGetFloat) {
  LocalVariables variables(1);
  variables.setFloat(0, 1.0f);
  EXPECT_EQ(variables.getFloat(0), 1.0f);
}

TEST(LocalVariablesTest, InvalidIndex) {
  LocalVariables variables(1);
  EXPECT_THROW(variables.getInt(2), std::out_of_range);
}

}  // namespace jvm::runtime