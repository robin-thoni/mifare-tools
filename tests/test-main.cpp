#include <iostream>
#include <string.h>
#include <gtest/gtest.h>
#include <Business/LibNfc.h>

TEST(None, None)
{
  ASSERT_TRUE(true);
}

int main(int argc, char* argv[])
{
  std::cout << "LibNfc version: " << LibNfc::getVersion() << std::endl;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
