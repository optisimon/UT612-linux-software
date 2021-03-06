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
	UT612ByteStreamParser(bool doPrintHeader, bool doTimestamp);

	/** Create a printable column header for all the measurements */
	std::string getColumnHeader() const;

	void processByte(uint8_t byte);

protected:
	// Protected to allow unit testing
	std::string processFrame(const std::vector<uint8_t>&data, size_t next_start);
	std::string getCurrentTime() const;

	std::string mMode2String(uint8_t mMode, uint8_t flags) const;
	std::string mUnit2String(uint8_t mUnit) const;
	std::string sMode2String(uint8_t sMode) const;
	std::string sUnit2String(uint8_t sUnit) const;
	std::string freq2String(uint8_t freq) const;
	std::string bits2Number(uint8_t msb, uint8_t lsb, uint8_t decimals, uint8_t maxNumSegments) const;
private:
	bool _doTimestamp;
	bool _frameInProgress;
	bool _lastByteWasCarriageReturn;
	size_t _byteCount;
	size_t _lastFrameByteCount;
	size_t _frameCount;
	std::vector<uint8_t> _buffer;
};
