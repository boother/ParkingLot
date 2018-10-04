#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>

#include "types.hpp"
#include "ParkingLotBase.hpp"

using namespace pl;
using namespace std;


ParkingLotBase::ParkingLotBase()
	: mBusiestTime{}
	, mHist(MINS_IN_DAY, 0)
	, mMaxCarNumber{}
{
}


CarStayInfo ParkingLotBase::parseString(const std::string& item)
{
	CarStayInfo res; 

	auto pos = item.find(TS_SEPARATOR);
	res.first.fromString(item, pos);

	pos = item.find(TS_SEPARATOR, pos + 1);
	res.second.fromString(item, pos);

	return res;
}

void ParkingLotBase::calcBusiestTime()
{
//  max value in a range
	size_t max_value = 0;

// check for busiest time range
	size_t lbid = 0; // low boundary id
	size_t ubid = 0; // upper boundary id
	size_t counter = 0;
	size_t max_counter = 0;

	for(size_t i = 0; i < mHist.size(); ++i)
	{
		if (mHist[i] > max_value)
		{
			max_value = mHist[i];
			lbid = 0;
			ubid = 0;
			counter = 1;
			max_counter = 0;
		}
		else if(mHist[i] == max_value)
		{
			++counter;
		}
		else if (counter)
		{
			if (max_counter < counter)
			{
				max_counter = counter;
				ubid = i;
				lbid = i - counter;	
			}
			counter = 0;
		}
	}
	mMaxCarNumber = max_value;
	mBusiestTime.first.value = lbid;
	mBusiestTime.second.value = ubid;
}


bool ParkingLotBase::process(const std::string& dataset_path)
{
	std::ifstream ifs(dataset_path);

	CHECK(ifs && !dataset_path.empty())

 	std::string line;
	while(getline (ifs, line))
	{
		CarStayInfo info = parseString(line);
		update(info);
	}

	ifs.close();

	calcBusiestTime();

	return true;
}

void ParkingLotBase::update(const CarStayInfo& info)
{
	auto entry = info.first.value;
	auto exit = info.second.value;

// update histogram
	CHECK(entry <= exit)
	std::transform(&mHist[entry], &mHist[exit], &mHist[entry], [] (size_t val) { return ++val; });
}

bool ParkingLotBase::plot() const
{
	std::cout << "\n";
	for (auto const& item : mHist) // mHist element index stores timestamp
	{
		std::cout << item << " ";
	}
	std::cout << '\n';

	return true;
}

