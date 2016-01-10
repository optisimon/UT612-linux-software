#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <stdint.h>

#include "UT612ByteStreamParser.hpp"


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


int main (int argc, char* argv[])
{
	if (argc == 2)
	{
		processFile(argv[1]);
	}
	else
	{
		processFile("/dev/stdin");
	}
	
	return 0;
}
