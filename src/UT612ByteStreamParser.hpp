/*
 * UT612ByteStreamParser.hpp
 *
 *  Created on: Jan 10, 2016
 *
 *  Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
 */

#pragma once


#include "IByteSink.hpp"

#include <stdint.h>
#include <vector>
#include <string>
#include <cstddef>


class UT612ByteStreamParser : public IByteSink {
public:
	UT612ByteStreamParser(bool doTimestamp = true);

	void processByte(uint8_t byte);

private:
	void processFrame(const std::vector<uint8_t>&data, size_t next_start);
	std::string getCurrentTime();

	bool _doTimestamp;
	bool _frameInProgress;
	bool _lastByteWasCarriageReturn;
	size_t _byteCount;
	size_t _lastFrameByteCount;
	size_t _frameCount;
	std::vector<uint8_t> _buffer;
};
