/*
 * UT612ByteSource.cpp
 *
 *  Created on: Jan 11, 2016
 *
 *  Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
 */


#include "hidapi/hidapi.h"
#include "UT612ByteSource.hpp"

#include <iostream>

#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>


static volatile bool killed = false;

static void sigint_handler(int)
{
	killed = true;
}

UT612ByteSource::UT612ByteSource(bool verbose) : _verbose(verbose)
{ }


void UT612ByteSource::registerByteSink(IByteSink& sink)
{
	_byteSinks.insert(&sink);
}


int UT612ByteSource::run()
{
	setupSIGINTHandler(); // Catch Ctrl-C


	//
	// Open the USB HID device and set up the UART
	//
	if (hid_init())
	{
		return -1;
	}

	// Open the device using the VID, PID,
	// and optionally the Serial number.
	////handle = hid_open(0x4d8, 0x3f, L"12345");
	hid_device* handle = hid_open(0x10c4, 0xea80, NULL);
	if (!handle)
	{
		std::cerr
		<< "ERROR: unable to connect to the UT612 LCR Meter\n"
		<< "The most common problem is that the cp210x kernel module is interfering\n\n"
		<< "If you installed this program using\n"
		<< "sudo make install\n"
		<< "then you should have a file /etc/modprobe.d/blacklist-cp210x.conf blacklisting the VID:PID of your LCR meter.\n";

		if (_verbose)
		{
			printAllHidDevices();

			std::cout
			<< "\nTo be able to troubleshoot any problems, you need to provide the output of a script in the src folder:\n"
			<< "cd src && ./collect_debug_info.sh\n"
			<< "\n";
		}

		return 1;
	}

	if (_verbose)
	{
		printHidDeviceInfo(handle);
	}

	setupUartForUT612(handle);

	enableUartForUT612(handle);


	//
	// Main loop
	//
	while (!killed)
	{
		uint8_t data[64];

		// TODO: When timeout set to 0, non blocking read is used. When set to -1 blocking read is used.
		int read = hid_read_timeout(handle, data, sizeof(data), /*milliseconds*/ 1000 );

		if (read == -1)
		{
			std::cerr << "\nERROR: hid_read_timeout returned -1" << std::endl;
		}
		else if (read == 0 && _verbose)
		{
			std::cerr << "\nERROR: timed out calling hid_read_timeout" << std::endl;
		}

		if (read > 1)
		{
			int numUartRxBytes = data[0];

			if (numUartRxBytes != read-1)
			{
				std::cerr << "\nERROR: numUartRxBytes = " << numUartRxBytes << ", read=" << read << std::endl;
			}

			for (int i = 0; i < numUartRxBytes; i++)
			{
				notifyAllByteSinks(data[i+1]);
			}
		}
	}

	enableUartForUT612(handle, false);

	std::cout << "\nKilled by user" << std::endl;


	//
	// Clean up
	//
	hid_close(handle);
	hid_exit();
	return 0;
}


void UT612ByteSource::printAllHidDevices()
{
	printf("\n--- List of all HID devices: ---\n");

	hid_device_info* devs = hid_enumerate(0x0, 0x0);
	hid_device_info* cur_dev = devs;
	while (cur_dev) {
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n",  cur_dev->interface_number);
		printf("\n");
		cur_dev = cur_dev->next;
	}
	hid_free_enumeration(devs);

	std::cout << "--------------------------------" << std::endl;
}


void UT612ByteSource::printHidDeviceInfo(hid_device* handle)
{
	// Read the Manufacturer String
#define MAX_STR 255
	wchar_t wstr[MAX_STR];

	wstr[0] = 0x0000;
	int res = hid_get_manufacturer_string(handle, wstr, MAX_STR);
	if (res < 0)
		std::cerr << "Unable to read manufacturer string" << std::endl;
	printf("Manufacturer String: %ls\n", wstr);

	// Read the Product String
	wstr[0] = 0x0000;
	res = hid_get_product_string(handle, wstr, MAX_STR);
	if (res < 0)
		std::cerr << "Unable to read product string" << std::endl;
	printf("Product String: %ls\n", wstr);

	// Read the Serial Number String
	wstr[0] = 0x0000;
	res = hid_get_serial_number_string(handle, wstr, MAX_STR);
	if (res < 0)
		std::cerr << "Unable to read serial number string" << std::endl;
	printf("Serial Number String: (%d) %ls", wstr[0], wstr);
	std::cout << std::endl;
}


#define GETSET_UART_CONFIG 0x50
#define HID_UART_EIGHT_DATA_BITS 0x03
#define HID_UART_NO_PARITY 0x00
#define HID_UART_SHORT_STOP_BIT 0x00
#define HID_UART_NO_FLOW_CONTROL 0x00

int UT612ByteSource::setupUartForUT612(hid_device* handle)
{
	uint8_t report[64] = {
			GETSET_UART_CONFIG,
			uint8_t(9600 >> 24),
			uint8_t(9600 >> 16),
			uint8_t(9600 >> 8),
			uint8_t(9600),
			HID_UART_NO_PARITY,
			HID_UART_NO_FLOW_CONTROL,
			HID_UART_EIGHT_DATA_BITS,
			HID_UART_SHORT_STOP_BIT
	}; // zero initialization of the rest of the elements

	int result = hid_send_feature_report(handle, report, sizeof(report));
	if (result == -1)
	{
		std::cerr << "\nERROR: setupUartForUT612: hid device transfer failed!" << std::endl;
	}

	return result;
}

#define GETSET_UART_ENABLE	0x41
int UT612ByteSource::enableUartForUT612(hid_device* handle, bool enabled)
{
	uint8_t report[64] = {
			GETSET_UART_ENABLE,
			uint8_t(enabled ? 1:0)
	}; // zero initialization of the rest of the elements

	int result = hid_send_feature_report(handle, report, sizeof(report));
	if (result == -1)
	{
		std::cerr << "\nERROR: enableUartForUT612: hid device transfer failed!" << std::endl;
	}

	return result;
}


void UT612ByteSource::setupSIGINTHandler()
{
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = sigint_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
}


void UT612ByteSource::notifyAllByteSinks(uint8_t byte)
{
	std::set<IByteSink*>::iterator it;

	for (it = _byteSinks.begin(); it != _byteSinks.end(); ++it)
	{
		(*it)->processByte(byte);
	}
}
