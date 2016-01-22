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
#include <stdexcept>

#include <sys/time.h>
#include <stdio.h>


UT612ByteStreamParser::UT612ByteStreamParser(bool doPrintHeader, bool doTimestamp)
: _doTimestamp(doTimestamp),
  _frameInProgress(false),
  _lastByteWasCarriageReturn(false),
  _byteCount(0),
  _lastFrameByteCount(0),
  _frameCount(0)
{
	if (doPrintHeader)
	{
		std::cout << getColumnHeader() << "\n";
	}
}

std::string UT612ByteStreamParser::getColumnHeader() const
{
	std::ostringstream oss;
	oss << "NO\t";

	if (_doTimestamp)
	{
		oss << "Time\t";
	}

	oss << "MMode\tMValue\tMUnit\tSMode\tSValue\tSUnit\tFreq";

	return oss.str();
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
	SMODE_THETA = 4,
};


std::string UT612ByteStreamParser::mMode2String(uint8_t mMode, uint8_t flags) const
{
	std::ostringstream retval;

	switch (mMode)
	{
	case MMODE_L:
		retval << "L";
		break;
	case MMODE_C:
		retval << "C";
		break;
	case MMODE_R:
		retval << "R";
		break;
	case MMODE_DCR:
		retval << "DCR";
		break;
	default:
	{
		std::ostringstream oss;
		oss << "ERROR: Unexpected byte in MMode: " << int(mMode);
		throw std::runtime_error(oss.str());
	}
	}

	if (mMode != MMODE_DCR)
	{
		// Add parallel modifier
		if (flags & MMODE_MODIFIER_PARALLEL)
		{
			retval << "p";
		}
		else if ((flags & MMODE_MODIFIER_SERIAL) || (flags == 0)) // TODO: Figure out real rule. Maybe only top bit matters
		{
			retval << "s";
		}
		else
		{
			std::ostringstream oss;
			oss << "ERROR: unknown serial/parallel modifier: " << int(flags);
			throw std::runtime_error(oss.str());
		}
	}

	return retval.str();
}


std::string UT612ByteStreamParser::mUnit2String(uint8_t mUnit) const
{
	int unit = mUnit >> 3;

	switch(unit)
	{
	case 0x00:
		return "";
		break;

	case 1:
		return "Ohm";
		break;
	case 2:
		return "kOhm";
		break;
	case 3:
		return "MOhm";
		break;


	case 5:
		return "uH";
		break;
	case 6:
		return "mH";
		break;
	case 7:
		return "H";
		break;


	case 9:
		return "pF";
		break;
	case 10:
		return "nF";
		break;
	case 11:
		return "uF";
		break;
	case 12:
		return "mF";
		break;
	default:
	{
		std::ostringstream oss;
		oss << "ERROR: unknown unit " << int(unit) << " (" << int(mUnit) << ")";
		throw std::runtime_error(oss.str());
		break;
	}
	}
}


std::string UT612ByteStreamParser::sMode2String(uint8_t sMode) const
{
	switch (sMode)
	{
	case SMODE_EMPTY:
		return "";
		break;
	case SMODE_D:
		return "D";
		break;
	case SMODE_Q:
		return "Q";
		break;
	case SMODE_ESR:
		return "ESR";
		break;
	case SMODE_THETA:
		return "theta";
//		if (d[5] == MMODE_C || d[5] == MMODE_L)
//		{
//			std::cout << "theta\t";
//		}
//		else
//		{
//			std::cout << "?\t"; // The windows software displays a question mark here if running in full Auto mode.
//			// Example: Rs, meter shows theta and -0.7 deg, but windows software shows theta and 6552, and windows software logs ? 6552 Deg
//		}
		break;
	default:
	{
		std::ostringstream oss;
		oss << "ERROR: Unexpected byte in SMode: " << int(sMode);
		throw std::runtime_error(oss.str());
	}
	}
}


std::string UT612ByteStreamParser::sUnit2String(uint8_t sUnit) const
{
	int sunit = sUnit >> 3;

	switch(sunit)
	{
	case 0x00:
		return "";
		break;

	case 0x01:
		return "Ohm";
		break;

	case 0x02:
		return "kOhm";
		break;

	case 0x03:
		return "MOhm";
		break;

	case 0x0e:
		return "Deg";
		break;

	default:
	{
		std::ostringstream oss;
		oss << "ERROR: don't know what to do with sunit=" << sunit << "\n";
		throw std::runtime_error(oss.str());
		break;
	}
	}
}


std::string UT612ByteStreamParser::freq2String(uint8_t freq) const
{
	switch (freq)
	{
	case FREQ_100HZ:
		return "100Hz";
		break;
	case FREQ_120HZ:
		return "120Hz";
		break;
	case FREQ_1KHZ:
		return "1KHz";
		break;
	case FREQ_10KHZ:
		return "10KHz";
		break;
	case FREQ_100KHZ:
		return "100KHz";
		break;
	case FREQ_DCR_MEASUREMENT:
		return "0Hz";
		break;
	default:
		std::ostringstream oss;
		oss << "ERROR: Unexpected byte in test frequency: " << int(freq);
		throw std::runtime_error(oss.str());
		break;
	}
}


std::string UT612ByteStreamParser::bits2Number(uint8_t msb, uint8_t lsb, uint8_t decimals, uint8_t maxNumSegments) const
{
	bool isNegative = false;
	int val = int16_t(lsb + msb * 256);

	if (val < 0)
	{
		isNegative = true;
		val = -val;
	}
	size_t numDecimals = decimals & 0x07;

	std::ostringstream oss;
	oss << val;
	std::string s = "00000" + oss.str();

	if (numDecimals-1 < s.length() && numDecimals > 0)
	{
		// Put the comma in the correct place
		s.insert(s.length() - numDecimals, ".");

		// Strip our convenient leading zeros
		while (s.length() > 1 && s[0] == '0' && s[1] != '.')
		{
			s = s.substr(1);
		}


		//
		// If too many digits, and a decimal point present, strip rightmost digits
		// NOTE: If we got here, we definitely did add a decimal point to the number.
		// NOTE: Looked like the windows software truncates the values, instead
		//       of correctly rounding them, so I truncates the values as well...
		//

		// -1 since we added a dot in the number, which won't occupy the segments needed for a digit.
		int numActualDigits = s.length() - 1;

		while (maxNumSegments < numActualDigits)
		{
			if (s[s.length() - 1] != '.')
			{
				s = s.substr(0, s.length() - 1);
				numActualDigits--;
			}
			else
			{
				throw std::runtime_error("ERROR: don't know what to do inside bits2Number");
			}
		}


		// restore negativity
		if (isNegative)
		{
			s = "-" + s;
		}

		return s;
	}
	else
	{
		std::ostringstream oss;
		oss << "ERROR: don't know what to do with numDecimals=" << numDecimals << ", val=" << val << ", isNegative" << isNegative;
		throw std::runtime_error(oss.str());
	}
}


std::string UT612ByteStreamParser::processFrame(const std::vector<uint8_t>&data, size_t next_start)
{
	std::ostringstream retval;

	const uint8_t* d = &data[next_start];

	if (d[0] != 0)
	{
		throw std::runtime_error("ERROR: not a zero in first byte (expected 0x0d 0x0a to be followed by 0x00)");
	}

	std::string mmode = mMode2String(d[5], d[2]);

	retval << mmode << "\t";

	// Handle error modifiers
	if (d[9] == 0 || d[9] == 128) // TODO: 128 first seen when measuring Cs, approx 790uF. No clue what it means
	{
		// NO ERROR - use the three preceeding bytes for measurement value

		std::string number = bits2Number(d[6], d[7], d[8], 5);

		retval << number << "\t";
	}
	else if (d[9] == 34)
	{
		retval << "-\t";
	}
	else if (d[9] == 195)
	{
		retval << "OL\t";
	}
	else if (d[9] == 67)
	{
		retval << "OL\t"; // TODO: this also sets secondary reading to "----". Seen for Lp, OL. H, secondary reading Q
	}
	else
	{
		std::ostringstream oss;
		oss << "ERROR: don't know what to do with error modifier d[9]: " << int(d[9]);
		throw std::runtime_error(oss.str());
	}


	// Measurement unit
	std::string mUnit = mUnit2String(d[8]);

	retval << mUnit << "\t";



	// Secondary display mode
	std::string sMode = sMode2String(d[10]);

	retval << sMode << "\t";


	if (d[14] == 0 || d[14] == 128) // TODO: JUST GUESSING
	{

		std::string number = bits2Number(d[11], d[12], d[13], 4);

		retval << number << "\t";
	}
	else if (d[14] == 162)
	{
		retval << "-\t";
	}
	else if (d[14] == 129)
	{
		retval << "\t";
	}
	else if (d[14] == 195)
	{
		retval << "OL\t";
	}
	else
	{
		std::ostringstream oss;
		oss << "ERROR: don't know what to do with d[14]=" << int(d[14]);
		throw std::runtime_error(oss.str());
	}


	// Secondary display unit
	std::string sUnit = sUnit2String(d[13]);
	retval << sUnit << "\t";


	// Measurement frequency
	std::string freq = freq2String(d[3]);
	retval << freq;

	return retval.str();
}


static std::string convert2HexString(const uint8_t* data, size_t length)
{
	std::ostringstream oss;

	oss << "{";
	for (size_t i = 0; i < length; i++)
	{
		if (i != 0) {
			oss << ", ";
		}
		char buff[32];
		snprintf(buff, sizeof(buff), "0x%02x", (unsigned int)(data[i]));
		oss << buff;
	}
	oss << "}";
	return oss.str();
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
			std::cerr << "\nERROR - FRAME ALIGNMENT ERROR (diff=" << bytesSinceLastFrame << ")" << std::endl;
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

		try {
			std::string values = processFrame(_buffer, _buffer.size() - frame_size);
			std::cout << values;
		}
		catch (std::runtime_error & e)
		{
			std::cerr << "Exception: \""<< e.what() << "\"\n";

			const uint8_t* d = &_buffer[_buffer.size() - frame_size];
			std::cerr << "Offending bytes: " << convert2HexString(d, frame_size) << std::endl;
		}

		bool showHex = false;
		if (showHex)
		{
			const uint8_t* d = &_buffer[_buffer.size() - frame_size];
			std::cout << "\t" << convert2HexString(d, frame_size);
		}

		std::cout << "\n";

		_buffer.clear();
	}

	_lastByteWasCarriageReturn = (byte == 0x0d);
}


std::string UT612ByteStreamParser::getCurrentTime() const
{
	const bool appendMilliseconds = true;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	time_t rawtime = tv.tv_sec;

	tm timeinfo;
	tm * result = localtime_r(&rawtime, &timeinfo);

	if (result == 0)
	{
		throw std::runtime_error("call to localtime_r failed");
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
