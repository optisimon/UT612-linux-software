/*
 * main.cpp
 *
 *  Created on: Jan 9, 2016
 *
 *  Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
 */

#include "UT612ByteStreamParser.hpp"
#include "UT612ByteSource.hpp"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <stdint.h>


void process(const std::vector<uint8_t>& data)
{
	std::cout << "SIZE: " << data.size() << " bytes\n";
	
	//
	// Simulate stream processing
	//
	UT612ByteStreamParser p;
	for (size_t i = 0; i < data.size()-2; i++)
	{
		p.processByte(data[i]);
	}
}


void processFile(std::string file)
{
	std::ifstream in(file.c_str(), std::ios::binary);
	
	std::vector<uint8_t> data;
	char c;
	while (in.get(c))
	{
		data.push_back(c);
	}
	
	if (data.size())
	{
		process(data);
	}
	
	in.close();
}


int processUSB()
{
	UT612ByteStreamParser parser;

	UT612ByteSource byteSource;

	byteSource.registerByteSink(parser);

	// Will block until Ctrl-C pressed if we could connect to meter...
	return byteSource.run();
}


int main (int argc, char* argv[])
{
	if (argc == 1)
	{
		return processUSB();
	}
	else if (argc == 2)
	{
		processFile(argv[1]);

		return 0;
	}
	
	return 1;
}
