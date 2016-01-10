/*
 * UT612ByteStreamParser.hpp
 *
 *  Created on: Jan 10, 2016
 *      Author: simon
 */

#pragma once

#include <string>
#include <vector>
#include <stdint.h>

class UT612ByteStreamParser {
public:
	UT612ByteStreamParser();

	std::string processByte(uint8_t byte);

	void processFrame(const std::vector<uint8_t>&data, size_t next_start);

private:
	bool _frameInProgress;
	bool _lastByteWasCarriageReturn;
	size_t _byteCount;
	size_t _lastFrameByteCount;
	size_t _frameCount;
	std::vector<uint8_t> _buffer;
};
