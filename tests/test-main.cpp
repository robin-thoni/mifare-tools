#include <iostream>
#include <string.h>
#include <gtest/gtest.h>
#include <DataAccess/LibNfc.h>
#include <DBO/StringUtils.h>
#include <DBO/ArrayUtils.h>
#include <DBO/AccessBitsDbo.h>

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

TEST(StringUtils, toLower)
{
  ASSERT_EQ(StringUtils::toLower('a'), 'a');
  ASSERT_EQ(StringUtils::toLower('z'), 'z');
  ASSERT_EQ(StringUtils::toLower('A'), 'a');
  ASSERT_EQ(StringUtils::toLower('Z'), 'z');
  ASSERT_EQ(StringUtils::toLower('1'), '1');
  ASSERT_EQ(StringUtils::toLower('@'), '@');
}

TEST(StringUtils, toUpper)
{
  ASSERT_EQ(StringUtils::toUpper('a'), 'A');
  ASSERT_EQ(StringUtils::toUpper('z'), 'Z');
  ASSERT_EQ(StringUtils::toUpper('A'), 'A');
  ASSERT_EQ(StringUtils::toUpper('Z'), 'Z');
  ASSERT_EQ(StringUtils::toUpper('1'), '1');
  ASSERT_EQ(StringUtils::toUpper('@'), '@');
}

TEST(StringUtils, humanToRaw)
{
  ASSERT_EQ(StringUtils::humanToRaw("0f00").getData(), std::string({0x0f, 0x00}));
  ASSERT_EQ(StringUtils::humanToRaw("000f").getData(), std::string({0x00, 0x0f}));
  ASSERT_EQ(StringUtils::humanToRaw("0a0a").getData(), std::string({0x0a, 0x0a}));
  ASSERT_EQ(StringUtils::humanToRaw("1a0f").getData(), std::string({0x1a, 0x0f}));
  ASSERT_EQ(StringUtils::humanToRaw("1a00f").getError(), "Malformed hex data: invalid length");
  ASSERT_EQ(StringUtils::humanToRaw("1a\n00f").getError(), "Malformed hex data at char 2");
}

TEST(StringUtils, ensureSize)
{
  ASSERT_EQ(StringUtils::ensureSize("test", 3), "tes");
  ASSERT_EQ(StringUtils::ensureSize("test", 4), "test");
  ASSERT_EQ(StringUtils::ensureSize("test", 5), std::string({'t', 'e', 's', 't', '\0'}));
}

TEST(StringUtils, ltrim)
{
  ASSERT_EQ(StringUtils::ltrim("  \n 42"), "42");
  ASSERT_EQ(StringUtils::ltrim("  \n 42 \n"), "42 \n");
  ASSERT_EQ(StringUtils::ltrim("42  \n "), "42  \n ");
  ASSERT_EQ(StringUtils::ltrim(" 42 \n 42  \n "), "42 \n 42  \n ");
}

TEST(StringUtils, rtrim)
{
  ASSERT_EQ(StringUtils::rtrim("  \n 42"), "  \n 42");
  ASSERT_EQ(StringUtils::rtrim("  \n 42 \n"), "  \n 42");
  ASSERT_EQ(StringUtils::rtrim("42  \n "), "42");
  ASSERT_EQ(StringUtils::rtrim(" 42 \n 42  \n "), " 42 \n 42");
}

TEST(StringUtils, trim)
{
  ASSERT_EQ(StringUtils::trim("  \n 42"), "42");
  ASSERT_EQ(StringUtils::trim("  \n 42 \n"), "42");
  ASSERT_EQ(StringUtils::trim("42  \n "), "42");
  ASSERT_EQ(StringUtils::trim(" 42 \n 42  \n "), "42 \n 42");
}

TEST(StringUtils, removeSpaces)
{
  ASSERT_EQ(StringUtils::removeSpaces("  \n 42"), "42");
  ASSERT_EQ(StringUtils::removeSpaces("  \n 42 \n"), "42");
  ASSERT_EQ(StringUtils::removeSpaces("42  \n "), "42");
  ASSERT_EQ(StringUtils::removeSpaces(" 42 \n 42  \n "), "4242");
}

TEST(ArrayUtils, getArrayBitimple)
{
  const unsigned char buf[] = {0x04};
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 0));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 1));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 2));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 3));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 4));
  ASSERT_TRUE(ArrayUtils::getArrayBit(buf, 5));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 6));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 7));
}

TEST(ArrayUtils, getArrayBitMultiple)
{
  const unsigned char buf[] = {0x80, 0x14, 0x01};
  ASSERT_TRUE(ArrayUtils::getArrayBit(buf, 0));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 1));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 2));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 3));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 4));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 5));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 6));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 7));

  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 8));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 9));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 10));
  ASSERT_TRUE(ArrayUtils::getArrayBit(buf, 11));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 12));
  ASSERT_TRUE(ArrayUtils::getArrayBit(buf, 13));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 14));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 15));

  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 16));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 17));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 18));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 19));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 20));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 21));
  ASSERT_FALSE(ArrayUtils::getArrayBit(buf, 22));
  ASSERT_TRUE(ArrayUtils::getArrayBit(buf, 23));
}

TEST(AccessBitsDbo, getBit)
{
  const unsigned char buf[4] = {0x05, 0xa3, 0xcf, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.getBit(1, 0));
  ASSERT_TRUE(dbo.getBit(2, 0));
  ASSERT_FALSE(dbo.getBit(3, 0));

  ASSERT_TRUE(dbo.getBit(1, 1));
  ASSERT_TRUE(dbo.getBit(2, 1));
  ASSERT_FALSE(dbo.getBit(3, 1));

  ASSERT_FALSE(dbo.getBit(1, 2));
  ASSERT_TRUE(dbo.getBit(2, 2));
  ASSERT_TRUE(dbo.getBit(3, 2));

  ASSERT_TRUE(dbo.getBit(1, 3));
  ASSERT_TRUE(dbo.getBit(2, 3));
  ASSERT_TRUE(dbo.getBit(3, 3));
}

TEST(AccessBitsDbo, canKeyDoThisOnThisBlock1)
{
  const unsigned char buf[4] = {0xff, 0x0f, 0x00, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_TRUE(dbo.canKeyAReadBlock(0));
  ASSERT_TRUE(dbo.canKeyBReadBlock(0));

  ASSERT_TRUE(dbo.canKeyAWriteBlock(0));
  ASSERT_TRUE(dbo.canKeyBWriteBlock(0));

  ASSERT_TRUE(dbo.canKeyAIncrementBlock(0));
  ASSERT_TRUE(dbo.canKeyBIncrementBlock(0));

  ASSERT_TRUE(dbo.canKeyADecrementBlock(0));
  ASSERT_TRUE(dbo.canKeyBDecrementBlock(0));
}

TEST(AccessBitsDbo, canKeyDoThisOnThisBlock2)
{
  const unsigned char buf[4] = {0xef, 0x0f, 0x01, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_TRUE(dbo.canKeyAReadBlock(0));
  ASSERT_TRUE(dbo.canKeyBReadBlock(0));

  ASSERT_FALSE(dbo.canKeyAWriteBlock(0));
  ASSERT_FALSE(dbo.canKeyBWriteBlock(0));

  ASSERT_FALSE(dbo.canKeyAIncrementBlock(0));
  ASSERT_FALSE(dbo.canKeyBIncrementBlock(0));

  ASSERT_FALSE(dbo.canKeyADecrementBlock(0));
  ASSERT_FALSE(dbo.canKeyBDecrementBlock(0));
}

TEST(AccessBitsDbo, canKeyDoThisOnThisBlock3)
{
  const unsigned char buf[4] = {0xfe, 0x1f, 0x00, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_TRUE(dbo.canKeyAReadBlock(0));
  ASSERT_TRUE(dbo.canKeyBReadBlock(0));

  ASSERT_FALSE(dbo.canKeyAWriteBlock(0));
  ASSERT_TRUE(dbo.canKeyBWriteBlock(0));

  ASSERT_FALSE(dbo.canKeyAIncrementBlock(0));
  ASSERT_FALSE(dbo.canKeyBIncrementBlock(0));

  ASSERT_FALSE(dbo.canKeyADecrementBlock(0));
  ASSERT_FALSE(dbo.canKeyBDecrementBlock(0));
}

TEST(AccessBitsDbo, canKeyDoThisOnThisBlock4)
{
  const unsigned char buf[4] = {0xee, 0x1f, 0x01, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_TRUE(dbo.canKeyAReadBlock(0));
  ASSERT_TRUE(dbo.canKeyBReadBlock(0));

  ASSERT_FALSE(dbo.canKeyAWriteBlock(0));
  ASSERT_TRUE(dbo.canKeyBWriteBlock(0));

  ASSERT_FALSE(dbo.canKeyAIncrementBlock(0));
  ASSERT_TRUE(dbo.canKeyBIncrementBlock(0));

  ASSERT_TRUE(dbo.canKeyADecrementBlock(0));
  ASSERT_TRUE(dbo.canKeyBDecrementBlock(0));
}

TEST(AccessBitsDbo, canKeyDoThisOnThisBlock5)
{
  const unsigned char buf[4] = {0xff, 0x0e, 0x10, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_TRUE(dbo.canKeyAReadBlock(0));
  ASSERT_TRUE(dbo.canKeyBReadBlock(0));

  ASSERT_FALSE(dbo.canKeyAWriteBlock(0));
  ASSERT_FALSE(dbo.canKeyBWriteBlock(0));

  ASSERT_FALSE(dbo.canKeyAIncrementBlock(0));
  ASSERT_FALSE(dbo.canKeyBIncrementBlock(0));

  ASSERT_TRUE(dbo.canKeyADecrementBlock(0));
  ASSERT_TRUE(dbo.canKeyBDecrementBlock(0));
}

TEST(AccessBitsDbo, canKeyDoThisOnThisBlock6)
{
  const unsigned char buf[4] = {0xef, 0x0e, 0x11, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.canKeyAReadBlock(0));
  ASSERT_TRUE(dbo.canKeyBReadBlock(0));

  ASSERT_FALSE(dbo.canKeyAWriteBlock(0));
  ASSERT_TRUE(dbo.canKeyBWriteBlock(0));

  ASSERT_FALSE(dbo.canKeyAIncrementBlock(0));
  ASSERT_FALSE(dbo.canKeyBIncrementBlock(0));

  ASSERT_FALSE(dbo.canKeyADecrementBlock(0));
  ASSERT_FALSE(dbo.canKeyBDecrementBlock(0));
}

TEST(AccessBitsDbo, canKeyDoThisOnThisBlock7)
{
  const unsigned char buf[4] = {0xfe, 0x1e, 0x10, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.canKeyAReadBlock(0));
  ASSERT_TRUE(dbo.canKeyBReadBlock(0));

  ASSERT_FALSE(dbo.canKeyAWriteBlock(0));
  ASSERT_FALSE(dbo.canKeyBWriteBlock(0));

  ASSERT_FALSE(dbo.canKeyAIncrementBlock(0));
  ASSERT_FALSE(dbo.canKeyBIncrementBlock(0));

  ASSERT_FALSE(dbo.canKeyADecrementBlock(0));
  ASSERT_FALSE(dbo.canKeyBDecrementBlock(0));
}

TEST(AccessBitsDbo, canKeyDoThisOnThisBlock8)
{
  const unsigned char buf[4] = {0xee, 0x1e, 0x11, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.canKeyAReadBlock(0));
  ASSERT_FALSE(dbo.canKeyBReadBlock(0));

  ASSERT_FALSE(dbo.canKeyAWriteBlock(0));
  ASSERT_FALSE(dbo.canKeyBWriteBlock(0));

  ASSERT_FALSE(dbo.canKeyAIncrementBlock(0));
  ASSERT_FALSE(dbo.canKeyBIncrementBlock(0));

  ASSERT_FALSE(dbo.canKeyADecrementBlock(0));
  ASSERT_FALSE(dbo.canKeyBDecrementBlock(0));
}

TEST(AccessBitsDbo, canKeyDoThisOnThisTrailer1)
{
  const unsigned char buf[4] = {0xff, 0x0f, 0x00, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.canKeyAReadKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyATrailer());

  ASSERT_TRUE(dbo.canKeyAWriteKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyATrailer());

  ASSERT_TRUE(dbo.canKeyAReadAccessBitsTrailer());
  ASSERT_FALSE(dbo.canKeyBReadAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteAccessBitsTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteAccessBitsTrailer());

  ASSERT_TRUE(dbo.canKeyAReadKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyBTrailer());

  ASSERT_TRUE(dbo.canKeyAWriteKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyBTrailer());
}

TEST(AccessBitsDbo, canKeyDoThisOnThisTrailer2)
{
  const unsigned char buf[4] = {0x7f, 0x0f, 0x08, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.canKeyAReadKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyATrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyATrailer());

  ASSERT_TRUE(dbo.canKeyAReadAccessBitsTrailer());
  ASSERT_FALSE(dbo.canKeyBReadAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteAccessBitsTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteAccessBitsTrailer());

  ASSERT_TRUE(dbo.canKeyAReadKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyBTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyBTrailer());
}

TEST(AccessBitsDbo, canKeyDoThisOnThisTrailer3)
{
  const unsigned char buf[4] = {0xf7, 0x8f, 0x00, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.canKeyAReadKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyATrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyATrailer());
  ASSERT_TRUE(dbo.canKeyBWriteKeyATrailer());

  ASSERT_TRUE(dbo.canKeyAReadAccessBitsTrailer());
  ASSERT_TRUE(dbo.canKeyBReadAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteAccessBitsTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAReadKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyBTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyBTrailer());
  ASSERT_TRUE(dbo.canKeyBWriteKeyBTrailer());
}

TEST(AccessBitsDbo, canKeyDoThisOnThisTrailer4)
{
  const unsigned char buf[4] = {0x77, 0x8f, 0x08, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.canKeyAReadKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyATrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyATrailer());

  ASSERT_TRUE(dbo.canKeyAReadAccessBitsTrailer());
  ASSERT_TRUE(dbo.canKeyBReadAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteAccessBitsTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAReadKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyBTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyBTrailer());
}

TEST(AccessBitsDbo, canKeyDoThisOnThisTrailer5)
{
  const unsigned char buf[4] = {0xff, 0x07, 0x80, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.canKeyAReadKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyATrailer());

  ASSERT_TRUE(dbo.canKeyAWriteKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyATrailer());

  ASSERT_TRUE(dbo.canKeyAReadAccessBitsTrailer());
  ASSERT_FALSE(dbo.canKeyBReadAccessBitsTrailer());

  ASSERT_TRUE(dbo.canKeyAWriteAccessBitsTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteAccessBitsTrailer());

  ASSERT_TRUE(dbo.canKeyAReadKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyBTrailer());

  ASSERT_TRUE(dbo.canKeyAWriteKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyBTrailer());
}

TEST(AccessBitsDbo, canKeyDoThisOnThisTrailer6)
{
  const unsigned char buf[4] = {0x7f, 0x07, 0x88, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.canKeyAReadKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyATrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyATrailer());
  ASSERT_TRUE(dbo.canKeyBWriteKeyATrailer());

  ASSERT_TRUE(dbo.canKeyAReadAccessBitsTrailer());
  ASSERT_TRUE(dbo.canKeyBReadAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteAccessBitsTrailer());
  ASSERT_TRUE(dbo.canKeyBWriteAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAReadKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyBTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyBTrailer());
  ASSERT_TRUE(dbo.canKeyBWriteKeyBTrailer());
}

TEST(AccessBitsDbo, canKeyDoThisOnThisTrailer7)
{
  const unsigned char buf[4] = {0xf7, 0x87, 0x80, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.canKeyAReadKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyATrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyATrailer());

  ASSERT_TRUE(dbo.canKeyAReadAccessBitsTrailer());
  ASSERT_TRUE(dbo.canKeyBReadAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteAccessBitsTrailer());
  ASSERT_TRUE(dbo.canKeyBWriteAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAReadKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyBTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyBTrailer());
}

TEST(AccessBitsDbo, canKeyDoThisOnThisTrailer8)
{
  const unsigned char buf[4] = {0x77, 0x87, 0x88, 0x00};
  AccessBitsDbo dbo((const char*)buf);

  ASSERT_FALSE(dbo.canKeyAReadKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyATrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyATrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyATrailer());

  ASSERT_TRUE(dbo.canKeyAReadAccessBitsTrailer());
  ASSERT_TRUE(dbo.canKeyBReadAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteAccessBitsTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteAccessBitsTrailer());

  ASSERT_FALSE(dbo.canKeyAReadKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBReadKeyBTrailer());

  ASSERT_FALSE(dbo.canKeyAWriteKeyBTrailer());
  ASSERT_FALSE(dbo.canKeyBWriteKeyBTrailer());
}

int main(int argc, char* argv[])
{
    std::cout << "LibNfc version: " << LibNfc::getVersion() << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
