#pragma once

#include <ctime>
#include <vector>

#include "types.hpp"

namespace pl
{

class IParkingLot
{
public:
	virtual ~IParkingLot() {};
	virtual bool process(const std::string& dataset_path) = 0;
	virtual DateTimeRange getBusiestTime() = 0;
	virtual bool plot() const = 0;
	virtual size_t getMaxCarNumber() const = 0;
};

} // pl
