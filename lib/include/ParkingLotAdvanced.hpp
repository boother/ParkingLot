#pragma once

#include <ctime>
#include <vector>

#include "types.hpp"
#include "IParkingLot.hpp"

namespace pl
{

class ParkingLotAdvanced : public IParkingLot
{
public:
	ParkingLotAdvanced();
	virtual ~ParkingLotAdvanced() {}

	bool process(const std::string& dataset_path) override;
	DateTimeRange getBusiestTime() override
	{
		return std::make_pair(cnvTimestampToDateTime(mBusiestTime.first), cnvTimestampToDateTime(mBusiestTime.second));
	}
	size_t getMaxCarNumber() const override { return mMaxCarNumber; }
	bool plot() const override;

private:
	void update(const CarStayInfoAdvanced& time);
	void calcBusiestTime();
	CarStayInfoAdvanced parseString(const std::string& line);
	std::time_t getEpochTime(const std::string& dateTime);
	DateTimeStruct cnvTimestampToDateTime(std::time_t timestamp);
	time_t cnvStringToTimestamp(const std::string& dateTimeStr);
	std::string getJsonValue(const std::string& field);
	
	CarStayInfoAdvanced mBusiestTime;
	std::vector<std::pair<std::time_t, size_t>> mHist;
	std::pair<std::vector<time_t>, std::vector<time_t>> mInputData;
	size_t mMaxCarNumber;
};

} // pl
