#include <gtest/gtest.h>

#include "config.h"

#include <string>

#include "libgpsfile2/utils/Strings.hpp"

//using libgpsfile2::utils::Strings;

TEST(StringsTest, padLeft1)
{
	std::string in = "0";
	libgpsfile2::utils::Strings::padLeft (in, '0', 9);
	EXPECT_STREQ (in.c_str (), "000000000");
}

TEST(StringsTest, padLeft2)
{
	std::string in = "";
	libgpsfile2::utils::Strings::padLeft (in, '0', 9);
	EXPECT_STREQ (in.c_str (), "000000000");
}

TEST(StringsTest, padLeft3)
{
	std::string in = "50";
	libgpsfile2::utils::Strings::padLeft (in, '0', 9);
	EXPECT_STREQ (in.c_str (), "000000050");
}

TEST(StringsTest, padLeft4)
{
	std::string in = "1234";
	libgpsfile2::utils::Strings::padLeft (in, '0', 4);
	EXPECT_STREQ (in.c_str (), "1234");
}

TEST(StringsTest, padLeft5)
{
	std::string in = "12345";
	libgpsfile2::utils::Strings::padLeft (in, '0', 4);
	EXPECT_STREQ (in.c_str (), "12345");
}

TEST(StringsTest, padLeft6)
{
	std::string in = "-40";
	libgpsfile2::utils::Strings::padLeft (in, '0', 4);
	EXPECT_STREQ (in.c_str (), "0-40");
}

TEST(StringsTest, padLeft7)
{
	std::string in = "-00";
	libgpsfile2::utils::Strings::padLeft (in, '0', 4, true);
	EXPECT_STREQ (in.c_str (), "-0000");
}

TEST(StringsTest, padLeft8)
{
	std::string in = "1234";
	libgpsfile2::utils::Strings::padLeft (in, '5', 4, true);
	EXPECT_STREQ (in.c_str (), "15234");
}

TEST(StringsTest, padLeft9)
{
	std::string in = "-4000";
	libgpsfile2::utils::Strings::padLeft (in, '0', 4, true);
	EXPECT_STREQ (in.c_str (), "-4000");
}

TEST(StringsTest, padLeft10)
{
	std::string in = "-4";
	libgpsfile2::utils::Strings::padLeft (in, '0', 5, true);
	EXPECT_STREQ (in.c_str (), "-00004");
}

TEST(StringsTest, padLeft11)
{
	std::string in = "14";
	libgpsfile2::utils::Strings::padLeft (in, '0', 5, true);
	EXPECT_STREQ (in.c_str (), "100004");
}

TEST(StringsTest, isSigned1)
{
	std::string in = "14";
	EXPECT_FALSE (libgpsfile2::utils::Strings::isSigned (in));
}

TEST(StringsTest, isSigned2)
{
	std::string in = "-14";
	EXPECT_TRUE (libgpsfile2::utils::Strings::isSigned (in));
}

TEST(StringsTest, isSigned3)
{
	std::string in = "+14";
	EXPECT_TRUE (libgpsfile2::utils::Strings::isSigned (in));
}

TEST(StringsTest, isSigned4)
{
	std::string in = "1";
	EXPECT_FALSE (libgpsfile2::utils::Strings::isSigned (in));
}

TEST(StringsTest, isSigned5)
{
	std::string in = "-1";
	EXPECT_TRUE (libgpsfile2::utils::Strings::isSigned (in));
}

TEST(StringsTest, isSigned6)
{
	std::string in = "+1";
	EXPECT_TRUE (libgpsfile2::utils::Strings::isSigned (in));
}

TEST(StringsTest, isSigned7)
{
	std::string in = "";
	EXPECT_FALSE (libgpsfile2::utils::Strings::isSigned (in));
}

TEST(StringsTest, isSigned8)
{
	std::string in = "-";
	EXPECT_TRUE (libgpsfile2::utils::Strings::isSigned (in));
}

TEST(StringsTest, isSigned9)
{
	std::string in = "+";
	EXPECT_TRUE (libgpsfile2::utils::Strings::isSigned (in));
}