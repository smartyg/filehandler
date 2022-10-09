#include <gtest/gtest.h>

#include "config.h"

#include <string>

#include "libgpsfile2/utils/Iobuf.hpp"

using libgpsfile2::utils::Iobuf;

TEST(IobufTest, EmptyInit)
{
	Iobuf buf;
	std::string str = buf.str ();
	EXPECT_TRUE (str.empty ());
}

TEST(IobufTest, NonEmptyInit)
{
	Iobuf buf;
	buf.str ("Hello World!");
	std::string str = buf.str ();
	EXPECT_EQ (buf.showmanyc (), 12);
	EXPECT_STREQ (str.c_str (), "Hello World!");
}

TEST(IobufTest, Get1)
{
	Iobuf buf;
	buf.str ("Hello World!");
	ASSERT_EQ (buf.sgetc (), 'H');
	ASSERT_EQ (buf.snextc (), 'e');
	ASSERT_EQ (buf.sgetc (), 'e');
	ASSERT_EQ (buf.sbumpc (), 'e');

	//std::string str = buf.str ();
	EXPECT_EQ (buf.showmanyc (), 10);
	std::array<char, 12> buf2;
	const auto res = buf.sgetn (buf2.data (), 12);
	EXPECT_EQ (res, 10);

	EXPECT_EQ (buf2[0], 'l');
	EXPECT_EQ (buf2[1], 'l');
	EXPECT_EQ (buf2[9], '!');
}

TEST(IobufTest, Put1)
{
	Iobuf buf;
	ASSERT_EQ (buf.sputc ('H'), 'H');
	ASSERT_EQ (buf.sputc ('e'), 'e');
	ASSERT_EQ (buf.sputc ('l'), 'l');
	ASSERT_EQ (buf.sputc ('l'), 'l');
	ASSERT_EQ (buf.sputc ('o'), 'o');
	ASSERT_EQ (buf.sputc (' '), ' ');
	ASSERT_EQ (buf.sputc ('W'), 'W');
	ASSERT_EQ (buf.sputc ('o'), 'o');
	ASSERT_EQ (buf.sputc ('r'), 'r');
	ASSERT_EQ (buf.sputc ('l'), 'l');
	ASSERT_EQ (buf.sputc ('d'), 'd');
	ASSERT_EQ (buf.sputc ('!'), '!');
	std::string str = buf.str ();
	EXPECT_EQ (buf.showmanyc (), 12);
	EXPECT_STREQ (str.c_str (), "Hello World!");
}

TEST(IobufTest, Put2)
{
	Iobuf buf;
	const char* str = "Hello World";
	const auto res = buf.sputn (str, 11);
	ASSERT_EQ (res, 11);
	ASSERT_EQ (buf.sputc ('!'), '!');
	std::string str_res = buf.str ();
	EXPECT_EQ (buf.showmanyc (), 12);
	EXPECT_STREQ (str_res.c_str (), "Hello World!");
}

TEST(IobufTest, Putback1)
{
	Iobuf buf;
	buf.str ("Hello World!");
	const auto res = buf.sputbackc ('B');
	ASSERT_EQ (res, -1);
}

TEST(IobufTest, Io1)
{
	Iobuf buf;
	const char* str1 = "Hello";
	auto res = buf.sputn (str1, 5);
	ASSERT_EQ (res, 5);

	ASSERT_EQ (buf.sputc (' '), ' ');

	const char* str2 = "World";
	res = buf.sputn (str2, 5);
	ASSERT_EQ (res, 5);

	ASSERT_EQ (buf.sputc ('!'), '!');

	std::array<char, 12> buf2;
	res = buf.sgetn (buf2.data (), 6);
	ASSERT_EQ (res, 6);

	EXPECT_EQ (buf.sungetc (), ' ');
	res = buf.sputbackc ('g');
	EXPECT_EQ (res, 'g');
	res = buf.sputbackc ('i');
	EXPECT_EQ (res, 'i');
	res = buf.sputbackc ('B');
	EXPECT_EQ (res, 'B');
	EXPECT_EQ (buf.showmanyc (), 10);
}

TEST(IobufTest, Put3)
{
	Iobuf buf(6);
	ASSERT_EQ (buf.sputc ('H'), 'H');
	ASSERT_EQ (buf.sputc ('e'), 'e');
	ASSERT_EQ (buf.sputc ('l'), 'l');
	ASSERT_EQ (buf.sputc ('l'), 'l');
	ASSERT_EQ (buf.sputc ('o'), 'o');
	ASSERT_EQ (buf.sputc (' '), ' ');
	ASSERT_EQ (buf.sputc ('W'), 'W');
	ASSERT_EQ (buf.sputc ('o'), 'o');
	ASSERT_EQ (buf.sputc ('r'), 'r');
	ASSERT_EQ (buf.sputc ('l'), 'l');
	ASSERT_EQ (buf.sputc ('d'), 'd');
	ASSERT_EQ (buf.sputc ('!'), '!');
	std::string str = buf.str ();
	EXPECT_EQ (buf.showmanyc (), 12);
	EXPECT_STREQ (str.c_str (), "Hello World!");
	EXPECT_EQ (buf.getTotalSize (), 12);
}

TEST(IobufTest, Put4)
{
	Iobuf buf(4);
	ASSERT_EQ (buf.getTotalSize (), 4);

	const char* str = "Hello";
	const auto res = buf.sputn (str, 5);
	ASSERT_EQ (res, 5);
	EXPECT_EQ (buf.getTotalSize (), 8);
	EXPECT_EQ (buf.getConsumed (), 0);
	EXPECT_EQ (buf.getReserved (), 3);

	std::string str_res = buf.str ();
	EXPECT_EQ (buf.showmanyc (), 5);
	EXPECT_STREQ (str_res.c_str (), "Hello");
}

TEST(IobufTest, Put5)
{
	Iobuf buf(4);
	ASSERT_EQ (buf.getTotalSize (), 4);

	const char* str = "Hello";
	const auto res = buf.sputn (str, 5);
	ASSERT_EQ (res, 5);

	ASSERT_EQ (buf.sbumpc (), 'H');
	std::string str_res = buf.str ();
	EXPECT_EQ (buf.showmanyc (), 4);
	EXPECT_STREQ (str_res.c_str (), "ello");
	EXPECT_EQ (str_res.size (), 4);
}