#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <stdint.h>

const int frame_size = 17;

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
	SMODE_QUESTIONMARK = 4,
};

void processFrame(const std::vector<uint8_t>&data, size_t next_start)
{
	const uint8_t* d = &data[next_start];
	
	if (d[0] != 0)
	{
		std::cout << "ERROR: a zero in first byte\n";
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
	case SMODE_QUESTIONMARK:
		std::cout << "?\t";
		break;
	}
	
	
	switch (d[3])
	{
	case FREQ_100HZ:
		std::cout << "100Hz\n";
		break;
	case FREQ_120HZ:
		std::cout << "120Hz\n";
		break;
	case FREQ_1KHZ:
		std::cout << "1KHz\n";
		break;
	case FREQ_10KHZ:
		std::cout << "10KHz\n";
		break;
	case FREQ_100KHZ:
		std::cout << "100KHz\n";
		break;
	case FREQ_DCR_MEASUREMENT:
		std::cout << "0Hz\n";
		break;
	default:
		std::cout << "ERROR: Unexpected byte in test frequency: " << int(d[3]) << "\n";
		break;
	}
}

void process(const std::vector<uint8_t>& data)
{
	std::cout << "SIZE: " << data.size() << " bytes\n";
	
	int n = 0;
	
	// Find start token (13 10)
	for (size_t i = 0; i < data.size()-2; i++)
	{
		if (data[i] == 0x0d && data[i+1] == 0x0a)
		{
			size_t next_start = i+2;
			
			if (data[next_start] != 0)
			{
				std::cout << "WARNING: expected 0x0d 0x0a to be followed by 0x00\n";
			}
			
			bool enoughBytes = next_start + frame_size < data.size();
			if (enoughBytes)
			{
				std::cout << "n = " << n++ << " ==> ";
				
				processFrame(data, next_start);
			}
		}
	}	
}

int main (int argc, char* argv[])
{
	std::ifstream in("/dev/stdin", std::ios::binary);
	
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
	
	return 0;
}
