/*
 * UT612ByteStreamParser.cpp
 *
 *  Created on: Jan 10, 2016
 *
 *  Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
 */

#include "UT612ByteStreamParser.hpp"

#include <iostream>

UT612ByteStreamParser::UT612ByteStreamParser()
 : _frameInProgress(false),
   _lastByteWasCarriageReturn(false),
   _byteCount(0),
   _lastFrameByteCount(0),
   _frameCount(0)
{ }


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
	if (d[9] == 0)
	{
		// NO ERROR - use the three preceeding bytes for measurement value

		int val = d[7] + d[6] * 256;

		std::cout
		<< "val=" << val;


		switch (d[8])
		{
		case 0x0a:
			std::cout << "/100 Ohm\t";
			break;
		case 0x0b:
			std::cout << "*1 Ohm\t";
			break;
		case 0x14:
			std::cout << "/10000 KOhm\t";
			break;
		case 0x1a:
			std::cout << "??? MOhm\t"; // TODO: only seen val=20000 and OL
			break;
		case 0x1b:
			std::cout << "???? MOhm\t"; // TODO: Windows SW says MOhm. Ohm meter says /10000 MOhm
			break;

		case 0x29:
			std::cout << "/10 uH\t";
			break;
		case 0x39:
			std::cout << "/10 H\t";
			break;

		case 0x49:
			std::cout << "/10 pF\t";
			break;
		case 0x59:
			std::cout << "/10 uF ????\t"; // TODO: only seen val=20000 and OL
			break;
		case 0x00:
			std::cout << "NOTHING\t"; // TODO: Empty field, and
			break;
		default:
			std::cout << "\nERROR: unknown unit: " << int(d[8]) << "\n";
		}
	}
	else if (d[9] == 34)
	{
		std::cout << "-\t";
	}
	else if (d[9] == 195)
	{
		std::cout << "OL\t";
	}
	else
	{
		std::cout << "\nERROR: don't know what to do with error modifier: " << int(d[9]) << "\n";
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
		std::cout << "?\t";
		break;
	default:
		std::cout << "\nERROR: Unexpected byte in SMode: " << int(d[10]) << "\n";
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
		std::cout << "n = " << _frameCount++ << " ==> ";

		processFrame(_buffer, _buffer.size() - frame_size);

		_buffer.clear();
	}

	_lastByteWasCarriageReturn = (byte == 0x0d);
}
