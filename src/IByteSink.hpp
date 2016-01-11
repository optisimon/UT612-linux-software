/*
 * IByteSink.hpp
 *
 *  Created on: Jan 11, 2016
 *
 *  Copyright (c) 2016 Simon Gustafsson (www.optisimon.com)
 */

#pragma once

#include <stdint.h>

class IByteSink {
public:
	virtual ~IByteSink() {};

	virtual void processByte(uint8_t byte) = 0;
};
