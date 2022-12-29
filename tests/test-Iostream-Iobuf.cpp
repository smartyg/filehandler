#include <gtest/gtest.h>

#include "config.h"

#include <string>

#include "filehandler/utils/Iobuf.hpp"

using filehandler::utils::Iobuf;

TEST(IostreamIobufTest, Init1)
{
	Iobuf *buf = new Iobuf ();
	std::iostream s (buf);
	EXPECT_TRUE (s.rdbuf () == buf);
	delete buf;
}

TEST(IostreamIobufTest, Write1)
{
	Iobuf *buf = new Iobuf ();
	std::iostream s (buf);
	ASSERT_TRUE (s.rdbuf () == buf);

	const std::string str = "Hello World!";
	s.write (str.c_str (), str.size ());

	EXPECT_STREQ (buf->str ().c_str (), str.c_str ());

	delete buf;
}

TEST(IostreamIobufTest, Write2)
{
	Iobuf *buf = new Iobuf ();
	std::iostream s (buf);
	ASSERT_TRUE (s.rdbuf () == buf);
	s
		<< "Hello World!\n"
		<< "This is a text about a piece of code.\n"
		<< "Code that do not have a purpose other than testing.\n"
		<< "Although it keep hope that some day it will become usefull."
		<< std::endl;

	// First extract one single charachter
	EXPECT_EQ (s.get (), 'H');

	// Second, extract 6 (-1 = 5) charachters
	std::string str1 (6, 0);
	s.get (str1.data (), 6);
	EXPECT_STREQ (str1.c_str (), "ello ");

	// Third, try to extract 9 (-1 = 8) charachters, or till an '\n' is encountered.
	std::string str2 (9, 0);
	s.get (str1.data (), 9);
	EXPECT_STREQ (str1.c_str (), "World!");

	// This should contain the newline
	EXPECT_EQ (s.get (), '\n');

	// Finally extract the next charachter
	EXPECT_EQ (s.get (), 'T');

	delete buf;
}