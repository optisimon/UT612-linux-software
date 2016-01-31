/*
 * ByteStreamToFile.hpp
 *
 *  Created on: Jan 24, 2016
 *
 *  Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
 */

#pragma once

#include <fstream>
#include <string>

#include "IByteSink.hpp"


class ByteStreamToFile : public IByteSink {
public:
	ByteStreamToFile(const std::string& filename);

	~ByteStreamToFile();

	void processByte(uint8_t byte);

private:
	std::ofstream _ofstream;
};

