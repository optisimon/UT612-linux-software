/*
 * UT612ByteSource.hpp
 *
 *  Created on: Jan 11, 2016
 *
 *  Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
 */

#pragma once


#include "IByteSink.hpp"

#include <set>


typedef struct hid_device_ hid_device;


class UT612ByteSource {
public:
	/**
	 * Register a byte sink to be passed every byte read out from the
	 * UT612 LCR Meter.
	 *
	 * @note Multiple byte sinks can be registered by repeatedly calling this.
	 * @note Each sink is only called once even if registered multiple times.
	 */
	void registerByteSink(IByteSink& sink);

	/**
	 * Reads bytes from the UT612 LCR meter, and
	 * passes them on to all registered ByteSinks.
	 *
	 * @warning This call won't return until Ctrl-C pressed
	 */
	int run();

	/**
	 * Print a detailed list of all detected HID devices to stdout.
	 */
	void printAllHidDevices();

private:
	/** Print device information to stdout */
	void printHidDeviceInfo(hid_device* handle);

	int setupUartForUT612(hid_device* handle);

	int enableUartForUT612(hid_device* handle, bool enabled=true);

	/** Setup capturing of Ctrl-C pressed (SIGINT) */
	void setupSIGINTHandler();

	/** Forward a byte to all registered byte sinks */
	void notifyAllByteSinks(uint8_t byte);

	std::set<IByteSink*> _byteSinks;
};
