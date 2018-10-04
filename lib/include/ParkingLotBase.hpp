#pragma once

#include <ctime>
#include <vector>

#include "types.hpp"
#include "IParkingLot.hpp"

namespace pl
{

class ParkingLotBase : public IParkingLot
{
public:
	ParkingLotBase();
	virtual ~ParkingLotBase() {}

	bool process(const std::string& dataset_path) override;
	DateTimeRange getBusiestTime() override
	{
		auto ts = mBusiestTime.first;
		DateTimeStruct low_boundary{};
		low_boundary.tm_hour = ts.value / pl::MINS_IN_HOUR;
		low_boundary.tm_min = ts.value % pl::MINS_IN_HOUR;

		ts = mBusiestTime.second;
		DateTimeStruct upper_boundary{};
		upper_boundary.tm_hour = ts.value / pl::MINS_IN_HOUR;
		upper_boundary.tm_min = ts.value % pl::MINS_IN_HOUR;

		return std::make_pair(low_boundary, upper_boundary);
	}
	size_t getMaxCarNumber() const override { return mMaxCarNumber; }

	bool plot() const override;

private:
	void update(const CarStayInfo& time);
	void calcBusiestTime();
	CarStayInfo parseString(const std::string& line);
	
	CarStayInfo mBusiestTime;
	std::vector<size_t> mHist;
	size_t mMaxCarNumber;
};

}
