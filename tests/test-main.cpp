#include <iostream>
#include <string.h>
#include <gtest/gtest.h>
#include <Business/LibNfc.h>
#include <Business/StringUtils.h>

TEST(StringUtils, rawToHumanChar)
{
    ASSERT_EQ(StringUtils::rawToHuman((char)255), "ff");
    ASSERT_EQ(StringUtils::rawToHuman((char)127), "7f");
    ASSERT_EQ(StringUtils::rawToHuman((char)10), "0a");
    ASSERT_EQ(StringUtils::rawToHuman((char)0), "00");
}

TEST(StringUtils, rawToHumanShort)
{
  ASSERT_EQ(StringUtils::rawToHuman((short)65535), "ffff");
  ASSERT_EQ(StringUtils::rawToHuman((short)65534), "fffe");
  ASSERT_EQ(StringUtils::rawToHuman((short)256), "0100");
  ASSERT_EQ(StringUtils::rawToHuman((short)255), "00ff");
  ASSERT_EQ(StringUtils::rawToHuman((short)127), "007f");
  ASSERT_EQ(StringUtils::rawToHuman((short)10), "000a");
  ASSERT_EQ(StringUtils::rawToHuman((short)0), "0000");
}

TEST(StringUtils, rawToHumanInt)
{
  ASSERT_EQ(StringUtils::rawToHuman((int)4294967295), "ffffffff");
  ASSERT_EQ(StringUtils::rawToHuman((int)4294967294), "fffffffe");
  ASSERT_EQ(StringUtils::rawToHuman((int)65536), "00010000");
  ASSERT_EQ(StringUtils::rawToHuman((int)65535), "0000ffff");
  ASSERT_EQ(StringUtils::rawToHuman((int)65534), "0000fffe");
  ASSERT_EQ(StringUtils::rawToHuman((int)256), "00000100");
  ASSERT_EQ(StringUtils::rawToHuman((int)255), "000000ff");
  ASSERT_EQ(StringUtils::rawToHuman((int)127), "0000007f");
  ASSERT_EQ(StringUtils::rawToHuman((int)10), "0000000a");
  ASSERT_EQ(StringUtils::rawToHuman((int)0), "00000000");
}

TEST(StringUtils, rawToHumanLong)
{
  ASSERT_EQ(StringUtils::rawToHuman((long)std::numeric_limits<unsigned long>::max()), "ffffffffffffffff");
  ASSERT_EQ(StringUtils::rawToHuman((long)std::numeric_limits<unsigned long>::max() - 1), "fffffffffffffffe");
  ASSERT_EQ(StringUtils::rawToHuman((long)4294967296), "0000000100000000");
  ASSERT_EQ(StringUtils::rawToHuman((long)4294967295), "00000000ffffffff");
  ASSERT_EQ(StringUtils::rawToHuman((long)4294967294), "00000000fffffffe");
  ASSERT_EQ(StringUtils::rawToHuman((long)65536), "0000000000010000");
  ASSERT_EQ(StringUtils::rawToHuman((long)65535), "000000000000ffff");
  ASSERT_EQ(StringUtils::rawToHuman((long)65534), "000000000000fffe");
  ASSERT_EQ(StringUtils::rawToHuman((long)256), "0000000000000100");
  ASSERT_EQ(StringUtils::rawToHuman((long)255), "00000000000000ff");
  ASSERT_EQ(StringUtils::rawToHuman((long)127), "000000000000007f");
  ASSERT_EQ(StringUtils::rawToHuman((long)10), "000000000000000a");
  ASSERT_EQ(StringUtils::rawToHuman((long)0), "0000000000000000");
}

TEST(StringUtils, rawToHumanString)
{
  ASSERT_EQ(StringUtils::rawToHuman(std::string({0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
                                                0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f})),
            "000102030405060708090a0b0c0d0e0f");
}

int main(int argc, char* argv[])
{
    std::cout << "LibNfc version: " << LibNfc::getVersion() << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
