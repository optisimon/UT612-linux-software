/*
 * test.cpp
 *
 *  Created on: Jan 10, 2016
 *
 *  Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
 */

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

#include "../UT612ByteStreamParser.hpp"


class TestableUT612ByteStreamParser : public UT612ByteStreamParser {
public:
	TestableUT612ByteStreamParser(bool p1, bool p2) : UT612ByteStreamParser(p1, p2)
{ }

	std::string testableProcessFrame(const std::vector<uint8_t>&data, size_t next_start)
	{
		return processFrame(data, next_start);
	}
};


BOOST_AUTO_TEST_SUITE(processFrame_patterns)


class Fixture {
public:
	Fixture(): p(false, false) { }
	TestableUT612ByteStreamParser p;
};


#define CHECK_STRINGS(string1, string2) BOOST_CHECK_EQUAL_COLLECTIONS(string1.begin(), string1.end(), string2.begin(), string2.end()); \
		if (string1.compare(string2) != 0) { \
			std::cout << string1 << "***CORRECT\n" << string2 << "***INCORRECT\n"; \
		}


BOOST_FIXTURE_TEST_CASE(pattern1, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x58, 0x00, 0x03, 0x13, 0x90, 0x14, 0x00, 0x04, 0x00, 0x00, 0x71, 0x80, 0x0d, 0x0a};
	std::string expected("Rs\t0.5008\tkOhm\ttheta\t0.0\tDeg\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern2, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x01, 0x00, 0x00, 0x00, 0x22, 0x02, 0x00, 0x00, 0x00, 0xa2, 0x0d, 0x0a};
	std::string expected("Ls\t-\t\tQ\t-\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern3, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x01, 0x00, 0x64, 0x29, 0x00, 0x02, 0x00, 0x01, 0x04, 0x80, 0x0d, 0x0a};
	std::string expected("Ls\t10.0\tuH\tQ\t0.000\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern4, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x02, 0x00, 0x00, 0x00, 0x22, 0x01, 0x00, 0x00, 0x00, 0xa2, 0x0d, 0x0a};
	std::string expected("Cs\t-\t\tD\t-\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern5, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x02, 0x4e, 0x20, 0x59, 0xc3, 0x01, 0x00, 0x00, 0x00, 0xa2, 0x0d, 0x0a};
	std::string expected("Cs\tOL\tuF\tD\t-\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern6, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x03, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x01, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t-\t\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern7, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x03, 0x13, 0x88, 0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t0.5000\tkOhm\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern8, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x58, 0x00, 0x03, 0x13, 0x8e, 0x14, 0x00, 0x04, 0x00, 0x00, 0x71, 0x80, 0x0d, 0x0a};
	std::string expected("Rs\t0.5006\tkOhm\ttheta\t0.0\tDeg\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern9, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x78, 0x00, 0x03, 0x00, 0x00, 0x00, 0x22, 0x04, 0x00, 0x00, 0x00, 0xa2, 0x0d, 0x0a};
	std::string expected("Rs\t-\t\ttheta\t-\t\t10KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern10, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x78, 0x00, 0x03, 0x13, 0x8b, 0x14, 0x00, 0x04, 0x00, 0x00, 0x71, 0x80, 0x0d, 0x0a};
	std::string expected("Rs\t0.5003\tkOhm\ttheta\t0.0\tDeg\t10KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern11, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0xe0, 0x98, 0x00, 0x03, 0x00, 0x00, 0x00, 0x22, 0x04, 0x00, 0x00, 0x00, 0xa2, 0x0d, 0x0a};
	std::string expected("Rp\t-\t\ttheta\t-\t\t100KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
// TODO: -0.1 instead of -.1
BOOST_FIXTURE_TEST_CASE(pattern12, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x98, 0x00, 0x03, 0x13, 0x8a, 0x14, 0x00, 0x04, 0xff, 0xff, 0x71, 0x80, 0x0d, 0x0a};
	std::string expected("Rs\t0.5002\tkOhm\ttheta\t-0.1\tDeg\t100KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern13, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x98, 0x00, 0x03, 0x13, 0x8b, 0x14, 0x00, 0x04, 0x00, 0x00, 0x71, 0x80, 0x0d, 0x0a};
	std::string expected("Rs\t0.5003\tkOhm\ttheta\t0.0\tDeg\t100KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern14, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x18, 0x00, 0x03, 0x00, 0x00, 0x00, 0x22, 0x04, 0x00, 0x00, 0x00, 0xa2, 0x0d, 0x0a};
	std::string expected("Rs\t-\t\ttheta\t-\t\t100Hz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern15, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x38, 0x00, 0x03, 0x00, 0x00, 0x00, 0x22, 0x04, 0x00, 0x00, 0x00, 0xa2, 0x0d, 0x0a};
	std::string expected("Rs\t-\t\ttheta\t-\t\t120Hz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern16, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x38, 0x00, 0x03, 0x13, 0x84, 0x14, 0x00, 0x04, 0x00, 0x00, 0x71, 0x80, 0x0d, 0x0a};
	std::string expected("Rs\t0.4996\tkOhm\ttheta\t0.0\tDeg\t120Hz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern17, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x58, 0x00, 0x03, 0x00, 0x00, 0x00, 0x22, 0x04, 0x00, 0x00, 0x00, 0xa2, 0x0d, 0x0a};
	std::string expected("Rs\t-\t\ttheta\t-\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern18, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x58, 0x00, 0x03, 0x13, 0x88, 0x14, 0x00, 0x04, 0x00, 0x00, 0x71, 0x80, 0x0d, 0x0a};
	std::string expected("Rs\t0.5000\tkOhm\ttheta\t0.0\tDeg\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}



BOOST_FIXTURE_TEST_CASE(pattern19, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x58, 0x00, 0x03, 0x13, 0x7e, 0x14, 0x00, 0x04, 0x00, 0x00, 0x71, 0x80, 0x0d, 0x0a};
	std::string expected("Rs\t0.4990\tkOhm\ttheta\t0.0\tDeg\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern20, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x01, 0x00, 0x3b, 0x29, 0x00, 0x02, 0x00, 0x00, 0x04, 0x80, 0x0d, 0x0a};
	std::string expected("Ls\t5.9\tuH\tQ\t0.000\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern21, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x80, 0x58, 0x00, 0x01, 0x21, 0xdf, 0x39, 0x00, 0x02, 0x00, 0x00, 0x04, 0x80, 0x0d, 0x0a};
	std::string expected("Lp\t867.1\tH\tQ\t0.000\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern22, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x02, 0x00, 0x00, 0x00, 0x22, 0x01, 0x00, 0x00, 0x00, 0xa2, 0x0d, 0x0a};
	std::string expected("Cs\t-\t\tD\t-\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern23, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x02, 0x4e, 0x20, 0x59, 0xc3, 0x01, 0x00, 0x00, 0x00, 0xa2, 0x0d, 0x0a};
	std::string expected("Cs\tOL\tuF\tD\t-\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern24, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x80, 0x58, 0x00, 0x02, 0x01, 0x33, 0x49, 0x00, 0x01, 0x4e, 0x20, 0x01, 0xc3, 0x0d, 0x0a};
	std::string expected("Cp\t30.7\tpF\tD\tOL\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern25, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x03, 0x13, 0x7e, 0x14, 0x00, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t0.4990\tkOhm\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern26, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x00, 0xb8, 0x00, 0x04, 0x00, 0x00, 0x00, 0x22, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("DCR\t-\t\t\t\t\t0Hz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern27, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x00, 0xb8, 0x00, 0x04, 0x13, 0x7c, 0x14, 0x00, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("DCR\t0.4988\tkOhm\t\t\t\t0Hz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern28, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x58, 0x00, 0x01, 0x00, 0x00, 0x00, 0x22, 0x02, 0x00, 0x00, 0x00, 0xa2, 0x0d, 0x0a};
	std::string expected("Ls\t-\t\tQ\t-\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern29, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x60, 0x58, 0x00, 0x03, 0x13, 0x7b, 0x14, 0x00, 0x04, 0x00, 0x00, 0x71, 0x80, 0x0d, 0x0a};
	std::string expected("Rs\t0.4987\tkOhm\ttheta\t0.0\tDeg\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern30, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x01, 0x01, 0x33, 0x29, 0x00, 0x02, 0x00, 0x03, 0x04, 0x80, 0x0d, 0x0a};
	std::string expected("Ls\t30.7\tuH\tQ\t0.000\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern31, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x80, 0x58, 0x00, 0x01, 0x09, 0x55, 0x39, 0x00, 0x02, 0x00, 0x03, 0x04, 0x80, 0x0d, 0x0a};
	std::string expected("Lp\t238.9\tH\tQ\t0.000\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern32, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x40, 0x58, 0x00, 0x02, 0x35, 0xd8, 0x59, 0x80, 0x01, 0x4e, 0x20, 0x01, 0xc3, 0x0d, 0x0a};
	std::string expected("Cs\t1378.4\tuF\tD\tOL\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(pattern33, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x80, 0x58, 0x00, 0x02, 0x02, 0x7b, 0x49, 0x00, 0x01, 0x4e, 0x20, 0x01, 0xc3, 0x0d, 0x0a};
	std::string expected("Cp\t63.5\tpF\tD\tOL\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}



BOOST_FIXTURE_TEST_CASE(ohms1, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x00, 0x58, 0x00, 0x03, 0x0f, 0xfe, 0x0b, 0x00, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t4.094\tOhm\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(ohms2, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x00, 0x58, 0x00, 0x03, 0x00, 0x00, 0x00, 0x22, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t-\t\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(ohms3, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x00, 0x58, 0x00, 0x03, 0x3e, 0x9c, 0x0a, 0x00, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t160.28\tOhm\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(ohms4, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x00, 0x58, 0x00, 0x03, 0x08, 0x85, 0x14, 0x00, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t0.2181\tkOhm\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(ohms5, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x00, 0x58, 0x00, 0x03, 0x00, 0x00, 0x00, 0x22, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t-\t\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(ohms6, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x00, 0x58, 0x00, 0x03, 0x27, 0x79, 0x14, 0x00, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t1.0105\tkOhm\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(ohms7, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x00, 0x58, 0x00, 0x03, 0x00, 0x00, 0x00, 0x22, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t-\t\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(ohms8, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x00, 0x58, 0x00, 0x03, 0x1a, 0x64, 0x1b, 0x00, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t6.756\tMOhm\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(ohms9, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x80, 0x58, 0x00, 0x03, 0x4e, 0x20, 0x1a, 0xc3, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rp\tOL\tMOhm\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}
BOOST_FIXTURE_TEST_CASE(ohms10, Fixture) {
	std::vector<uint8_t> data = {0x00, 0x0d, 0x00, 0x58, 0x00, 0x03, 0x19, 0x5c, 0x1b, 0x00, 0x00, 0x4e, 0x20, 0x00, 0x81, 0x0d, 0x0a};
	std::string expected("Rs\t6.492\tMOhm\t\t\t\t1KHz");
	std::string output = p.testableProcessFrame(data, 0);
	CHECK_STRINGS(expected, output);
}


BOOST_AUTO_TEST_SUITE_END()
