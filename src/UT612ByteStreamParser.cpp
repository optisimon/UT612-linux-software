/*
 * UT612ByteStreamParser.cpp
 *
 *  Created on: Jan 10, 2016
 *
 *  Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
 */

#include "UT612ByteStreamParser.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include <sys/time.h>
#include <stdio.h>


UT612ByteStreamParser::UT612ByteStreamParser(bool doTimestamp)
: _doTimestamp(doTimestamp),
  _frameInProgress(false),
  _lastByteWasCarriageReturn(false),
  _byteCount(0),
  _lastFrameByteCount(0),
  _frameCount(0)
{
	// Print a column header for all the measurements
	std::cout << "NO\t";

	if (_doTimestamp)
	{
		std::cout << "Time\t";
	}

	std::cout << "MMode\tMValue\tMUnit\tSMode\tSValue\tSUnit\tFreq\n";
}


const int frame_size = 17;


// Stored in byte 5
enum MModeEnum {
	MMODE_L = 1,
	MMODE_C = 2,
	MMODE_R = 3,
	MMODE_DCR = 4, // WARNING: This mode don't have the p/s modifier
};

// Stored in byte 2, bit 7 and 6
enum MModeModifier {
	MMODE_MODIFIER_SERIAL = 0x40, // NOTE: It's this bit, not this value
	MMODE_MODIFIER_PARALLEL = 0x80 // NOTE: It's this bit, not this value

	// TODO: There is also a bit, 0x20, which I don't know what it does yet (can be either way in both the Rs and the Ls mode)
};


// Seems to be stored in byte 3
enum FreqEnum {
	FREQ_100HZ = 0x18, //24,
	FREQ_120HZ = 0x38, //56,
	FREQ_1KHZ = 0x58, //88,
	FREQ_10KHZ = 0x78, //120,
	FREQ_100KHZ = 0x98, //152,

	// TODO: Verify that this really is DC resistance measurement. Possibly rename to FREQ_0HZ?
	FREQ_DCR_MEASUREMENT = 0xB8
};

// Seems to be stored in byte 10
enum SModeEnum {
	SMODE_EMPTY = 0,
	SMODE_D = 1,
	SMODE_Q = 2,
	SMODE_ESR = 3,
	SMODE_QUESTIONMARK = 4,
};

void UT612ByteStreamParser::processFrame(const std::vector<uint8_t>&data, size_t next_start)
{
	const uint8_t* d = &data[next_start];

	if (d[0] != 0)
	{
		std::cout << "\nWARNING: expected 0x0d 0x0a to be followed by 0x00\n";
		std::cout << "\nERROR: a zero in first byte\n";
	}

	switch (d[5])
	{
	case MMODE_L:
		std::cout << "L";
		break;
	case MMODE_C:
		std::cout << "C";
		break;
	case MMODE_R:
		std::cout << "R";
		break;
	case MMODE_DCR:
		std::cout << "DCR";
		break;
	default:
		std::cout << "\nERROR: Unexpected byte in MMode: " << int(d[5]) << "\n";
	}

	if (d[5] == MMODE_DCR)
	{
		std::cout << "\t";
	}
	else
	{
		// Add parallel modifier
		if (d[2] & MMODE_MODIFIER_PARALLEL)
		{
			std::cout << "p\t";
		}
		else if (d[2] & MMODE_MODIFIER_SERIAL || (d[2] == 0)) // TODO: Figure out real rule. Maybe only top bit matters
		{
			std::cout << "s\t";
		}
		else
		{
			std::cout << "\nERROR: unknown serial/parallel modifier: " << int(d[2]) << "\n";
		}
	}

	// Handle error modifiers
	if (d[9] == 0 || d[9] == 128) // TODO: 128 first seen when measuring Cs, approx 790uF. No clue what it means
	{
		// NO ERROR - use the three preceeding bytes for measurement value

		int val = d[7] + d[6] * 256;


		size_t numDecimals = d[8] & 0x07;

		std::ostringstream oss;
		oss << val;
		std::string s = "00000" + oss.str();

		if (numDecimals-1 < s.length() && numDecimals > 0)
		{
			s.insert(s.length() - numDecimals, ".");

			while (s.length()> 1 && s[0] == '0' && s[1] != '.')
			{
				s = s.substr(1);
			}

			std::cout << s << " ";
		}
		else
		{
			std::cout << "\nERROR: don't know what to do with numDecimals=" << numDecimals << ", val=" << val << "\n";
		}

		int unit = d[8] >> 3;

		switch(unit)
		{
		case 1:
			std::cout << "Ohm";
			break;
		case 2:
			std::cout << "kOhm";
			break;
		case 3:
			std::cout << "MOhm";
			break;


		case 5:
			std::cout << "uH";
			break;
		case 6:
			std::cout << "mH";
			break;
		case 7:
			std::cout << "H";
			break;


		case 9:
			std::cout << "pF";
			break;
		case 10:
			std::cout << "nF";
			break;
		case 11:
			std::cout << "uF";
			break;
		case 12:
			std::cout << "mF";
			break;
		default:
			std::cout << "\nERROR: unknown unit " << unit << " (" << int(d[8]) << ")\n";
			break;
		}
		std::cout << "\t";


	}
	else if (d[9] == 34)
	{
		std::cout << "-\t";
	}
	else if (d[9] == 195)
	{
		std::cout << "OL\t";
	}
	else if (d[9] == 67)
	{
		std::cout << "OL\t"; // TODO: this also sets secondary reading to "----". Seen for Lp, OL. H, secondary reading Q
	}
	else
	{
		std::cout << "\nERROR: don't know what to do with error modifier d[9]: " << int(d[9]) << "\n";
	}


	switch (d[10])
	{
	case SMODE_EMPTY:
		std::cout << " \t";
		break;
	case SMODE_D:
		std::cout << "D\t";
		break;
	case SMODE_Q:
		std::cout << "Q\t";
		break;
	case SMODE_ESR:
		std::cout << "ESR\t";
		break;
	case SMODE_QUESTIONMARK:
		if (d[5] == MMODE_C || d[5] == MMODE_L)
		{
			std::cout << "theta\t";
		}
		else
		{
			std::cout << "?\t";
		}
		break;
	default:
		std::cout << "\nERROR: Unexpected byte in SMode: " << int(d[10]) << "\n";
	}


	if (d[14] == 0 || d[14] == 128) // TODO: JUST GUESSING
	{
		int sval = int16_t(d[12] + d[11]*256);
		//std::cout << "sval=" << sval << "\t";

		size_t numDecimals = d[13] & 0x07;


		// TODO: if numdecimals is set to 4, then the windows software only shows the first 3 digits after the decimal
		// TODO: and if val=65535, and numdecimals set to 1, then the windows software only shows 6553, not 6553.5
		std::ostringstream oss;
		oss << sval;
		std::string s = "00000" + oss.str();

		if (numDecimals-1 < s.length() && numDecimals > 0)
		{
			s.insert(s.length() - numDecimals, ".");

			while (s.length()> 1 && s[0] == '0' && s[1] != '.')
			{
				s = s.substr(1);
			}

			std::cout << s << " ";
		}
		else
		{
			std::cout << "\nERROR: don't know what to do with numDecimals=" << numDecimals << ", sval=" << sval << "\n";
		}
	}
	else if (d[14] == 162)
	{
		std::cout << "-\t";
	}
	else if (d[14] == 129)
	{
		std::cout << "\t";
	}
	else if (d[14] == 195)
	{
		std::cout << "OL\t";
	}
	else
	{
		std::cout << "\nERROR: don't know what to do with d[14]=" << int(d[14]) << "\n";
	}

	int sunit = d[13] >> 3;

	switch(sunit)
	{
	case 0x00:
		std::cout << "\t";
		break;

	case 0x01:
		std::cout << "Ohm\t";
		break;

	case 0x02:
		std::cout << "kOhm\t";
		break;

	case 0x03:
		std::cout << "MOhm\t";
		break;

	case 0x0e:
		std::cout << "Deg\t";
		break;

	default:
		std::cout << "\nERROR: don't know what to do with sunit=" << sunit << "\n";
		break;
	}


	switch (d[3])
	{
	case FREQ_100HZ:
		std::cout << "100Hz";
		break;
	case FREQ_120HZ:
		std::cout << "120Hz";
		break;
	case FREQ_1KHZ:
		std::cout << "1KHz";
		break;
	case FREQ_10KHZ:
		std::cout << "10KHz";
		break;
	case FREQ_100KHZ:
		std::cout << "100KHz";
		break;
	case FREQ_DCR_MEASUREMENT:
		std::cout << "0Hz";
		break;
	default:
		std::cout << "\nERROR: Unexpected byte in test frequency: " << int(d[3]) << "\n";
		break;
	}

	std::cout << "\n";
}


void UT612ByteStreamParser::processByte(uint8_t byte)
{
	_byteCount++;

	bool currentByteIsLinefeed = (byte == 0x0a);
	bool frameIsEnding = _lastByteWasCarriageReturn && currentByteIsLinefeed;

	_buffer.push_back(byte);

	if (frameIsEnding && _buffer.size() >= frame_size)
	{
		//
		// Warn on frame alignment errors
		//
		int bytesSinceLastFrame = _byteCount - _lastFrameByteCount;
		if (bytesSinceLastFrame != frame_size)
		{
			std::cout << "\nERROR - FRAME ALIGNMENT ERROR (diff=" << bytesSinceLastFrame << ")\n";
		}
		_lastFrameByteCount = _byteCount;


		//
		// Process the frame
		//
		std::cout << _frameCount++ << "\t";

		if (_doTimestamp)
		{
			std::cout << getCurrentTime() << "\t";
		}

		processFrame(_buffer, _buffer.size() - frame_size);

		_buffer.clear();
	}

	_lastByteWasCarriageReturn = (byte == 0x0d);
}


std::string UT612ByteStreamParser::getCurrentTime()
{
	const bool appendMilliseconds = true;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	time_t rawtime = tv.tv_sec;

	tm timeinfo;
	tm * result = localtime_r(&rawtime, &timeinfo);

	if (result == 0)
	{
		return ""; // TODO: should we assert? or print to stderr? or throw exception?
	}

	char buffer [64];
	strftime (buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", result);

	std::string retval = buffer;

	if (appendMilliseconds)
	{
		snprintf(buffer, sizeof(buffer), ".%03d", int(tv.tv_usec / 1000));
		retval += buffer;
	}

	return retval;
}
