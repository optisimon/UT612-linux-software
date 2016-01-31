/*
 * main.cpp
 *
 *  Created on: Jan 9, 2016
 *
 *  Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
 */

#include "UT612ByteStreamParser.hpp"
#include "ByteStreamToFile.hpp"
#include "UT612ByteSource.hpp"

#include <iostream>
#include <fstream>
#include <memory>
#include <stdio.h>
#include <string>
#include <vector>

#include <getopt.h>
#include <stdint.h>


void process(const std::vector<uint8_t>& data, bool doPrintHeader, bool verbose)
{
	std::cout << "SIZE: " << data.size() << " bytes\n";
	
	//
	// Simulate stream processing
	//
	UT612ByteStreamParser p(doPrintHeader, /* doTimestamp */ false);

	for (size_t i = 0; i < data.size()-2; i++)
	{
		p.processByte(data[i]);
	}
}


void processFile(std::string file, bool doPrintHeader, bool verbose)
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
		process(data, doPrintHeader, verbose);
	}
	
	in.close();
}


int processUSB(bool doPrintHeader, bool verbose, const char* binaryDumpFile)
{
	UT612ByteStreamParser parser(doPrintHeader, /* doTimestamp */ true);

	UT612ByteSource byteSource(verbose);

	byteSource.registerByteSink(parser);

	std::shared_ptr<ByteStreamToFile> byteSaver;

	if (binaryDumpFile)
	{
		byteSaver.reset(new ByteStreamToFile(binaryDumpFile));
		byteSource.registerByteSink(*byteSaver);
	}

	// Will block until Ctrl-C pressed if we could connect to meter...
	return byteSource.run();
}


void printHelp()
{
	std::cout
	<< "Usage: ut612 [options]\n"
	"\n"
	"Running with no arguments will connect to the UT612 LCR Meter.\n"
	"This tool should start printing measurement values provided that the meter is\n"
	"on, and has USB mode selected (an icon of a small computer screen with an S\n"
	"inside it should be visible in the top left corner of the LCR meters display)\n"
	"\n"
	"OPTIONS:\n"
	"--dumpbinary FILENAME   Save raw bytes received over USB (i.e. no timestamps)\n"
	"--parsebinary FILENAME  Parse previously saved raw data\n"
	"--verbose               Print a lot of debugging information\n"
	"--noheader              Do not print/save the measurement data header\n"
	"--help                  Show this help message\n"
	"\n"
	"COPYRIGHT:\n"
	"Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)\n"
	"This software is provided AS-IS, there is NO warranties, not even for\n"
	"merchantability or fitness for a particular purpose\n";
}


int main (int argc, char* argv[])
{
	bool argVerbose = false;
	bool argNoHeaders = false;
	bool argPrintHelp = false;
	const char* argBinaryDumpFile = 0;
	const char* argParseBinary  = 0;

	while (true)
	{
		int option_index = 0;
		static struct option long_options[] = {
				{"dumpbinary",  required_argument, 0, 'd' },
				{"parsebinary", required_argument, 0, 'p' },
				{"verbose",     no_argument, 0, 'v' },
				{"noheader",    no_argument, 0, 'n' },
				{"help",        no_argument, 0, 'h' },
				{0, 0, 0, 0 }
		};

		int c = getopt_long(argc, argv, "l:d:p:vnh", long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
		{
		case 'd':
			argBinaryDumpFile = optarg;
			break;

		case 'p':
			argParseBinary = optarg;
			break;

		case 'v':
			argVerbose = true;
			break;

		case 'n':
			argNoHeaders = true;
			break;

		case 'h':
			argPrintHelp = true;
			break;

		case '?':
			std::cerr << "ERROR: error occurred while parsing command line options.\n\n";
			printHelp();
			return 1;
			break;

		default:
			std::cerr << "ERROR: getopt returned unhandled character code " << int(c) << "\n\n";
			printHelp();
			return 1;
		}
	}
	if (optind < argc)
	{
		std::cerr << "ERROR: non-option arguments provided: ";
		while (optind < argc)
		{
			std::cerr << "\"" << argv[optind++] << "\" ";
		}
		std::cerr << "\n\n";
		printHelp();
		return 1;
	}

	if (argPrintHelp)
	{
		printHelp();
		return 0;
	}

	if (argParseBinary)
	{
		processFile(argParseBinary, !argNoHeaders, argVerbose);
		return 0;
	}
	else
	{
		return processUSB(!argNoHeaders, argVerbose, argBinaryDumpFile);
	}

	
	return 1;
}
