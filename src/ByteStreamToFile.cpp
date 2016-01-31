/*
 * ByteStreamToFile.cpp
 *
 *  Created on: Jan 24, 2016
 *
 *  Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
 */

#include "ByteStreamToFile.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>


ByteStreamToFile::ByteStreamToFile(const std::string& filename) : _ofstream(filename.c_str(), std::ios::binary | std::ios::out)
{
	if (!_ofstream.good())
	{
		std::ostringstream oss;
		oss << "ERROR: unable to open file \"" << filename << "\" for writing!";
		std::cerr << oss.str();

		throw std::runtime_error(oss.str());
	}
}


ByteStreamToFile::~ByteStreamToFile()
{
	_ofstream.close();
}


void ByteStreamToFile::processByte(uint8_t byte)
{
	_ofstream.put(byte);
}
