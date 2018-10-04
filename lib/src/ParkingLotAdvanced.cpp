#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <ctime>

#include "types.hpp"
#include "ParkingLotAdvanced.hpp"

using namespace pl;
using namespace std;

ParkingLotAdvanced::ParkingLotAdvanced()
	: mBusiestTime{}
	, mHist{}
	, mMaxCarNumber{}
{
}

// Converts UTC time string to a time_t value.
std::time_t ParkingLotAdvanced::getEpochTime(const std::string& dateTime)
{
	std::tm tm_object{};
	std::istringstream istr(dateTime);
//	input.imbue(std::locale(std::setlocale(LC_ALL, nullptr)));
	istr >> std::get_time(&tm_object, format.c_str());

	CHECK(!istr.fail())

    // Convert the tm structure to time_t value and return.
    return std::mktime(&tm_object);
}

DateTimeStruct ParkingLotAdvanced::cnvTimestampToDateTime(std::time_t timestamp)
{
	std::tm * time_info;
	time_info = localtime(&timestamp);

	return *time_info;
}

time_t ParkingLotAdvanced::cnvStringToTimestamp(const std::string& dateTimeStr)
{
	std::time_t res = getEpochTime(dateTimeStr);
	cnvTimestampToDateTime(res);

	return res;
}

std::string ParkingLotAdvanced::getJsonValue(const std::string& field)
{
	static const size_t RECORD_SIZE = sizeof("2016-05-01T14:17:00"); // from example

	auto pos0 = field.find(":"); 	 		 // find ':' after key
	auto pos1 = field.find("\"", pos0 + 1);  // find open'"' before value
	auto pos2 = field.find("\"", pos1 + 1);  // find close '"' after value

	return ((pos2 - pos1) == RECORD_SIZE) ? field.substr(pos1 + 1, pos2 - pos1 - 1) : std::string();
}


CarStayInfoAdvanced ParkingLotAdvanced::parseString(const std::string& raw_item)
{
	static const std::string ARRIVALE_KEY = "\"ArrivalTime\"";
	static const std::string LEAVE_KEY = "\"LeaveTime\"";
	bool state = true;
	
	std::string item = raw_item + ","; // add separator at the string end to simplify parsing
	size_t numel = std::count(item.begin(), item.end(), ',');
	CHECK(3 == numel) // invalid number of fields in json item

	std::vector<std::string> field_array(numel);
	size_t begin = 0;
	size_t pos = 0;

	for (size_t i = 0; i < field_array.size(); ++i)
	{
		auto pos = item.find(",", begin);
		field_array[i] = item.substr(begin, pos - begin);
		begin = pos + 1;
	}

	time_t arrival_date;
	time_t leave_date;

	for (size_t i = 0; i < field_array.size(); ++i)
	{
		// e.g. "ArrivalTime": "2016-05-01T14:17:00"
		auto apos = field_array[i].find(ARRIVALE_KEY);
		if(apos != std::string::npos)
		{
			std::string value_str = getJsonValue(field_array[i]);
			CHECK(!value_str.empty())
			arrival_date = cnvStringToTimestamp(value_str);
		}		 
	
		auto lpos = field_array[i].find(LEAVE_KEY);
		if(lpos != std::string::npos)
		{
			std::string value_str = getJsonValue(field_array[i]);
			CHECK(!value_str.empty())
			leave_date = cnvStringToTimestamp(value_str);
		}
	}
	return CarStayInfoAdvanced{arrival_date, leave_date};
}

void ParkingLotAdvanced::calcBusiestTime()
{
	auto& arrivalData = mInputData.first;
	auto& leaveData = mInputData.second;

	// sort arrival and leave data independently
	std::sort(arrivalData.begin(), arrivalData.end()); 
	std::sort(leaveData.begin(), leaveData.end());

	// input data size validation
	const size_t numel = (arrivalData.size() == leaveData.size()) ? arrivalData.size() : 0;
	CHECK(0 != numel)

	size_t arr_idx = 1;
	size_t lv_idx = 0;
	size_t car_last = 0;
	int32_t car_number = 1;
	int32_t car_max = 1;
	time_t arr_time = arrivalData[0];
	time_t lv_time = arrivalData[0];
	time_t busiest_time = 0;
	time_t max_dt = 0;

	mHist.clear(); // clear hist from possible previous phase
	mHist.emplace_back(arr_time, car_number); // Store first arrival event to histogram

	while (arr_idx < numel) // iterate while all cars is arrived
	{
		// If arrival -> increment car_in 
		if (arrivalData[arr_idx] <= leaveData[lv_idx])
		{
			car_number++;
			arr_time = arrivalData[arr_idx]; // store last arrival time

			// check for new max value and busiest time range
			if (car_number >= car_max)
			{
				car_max = car_number;
				busiest_time = arr_time; // store busiest time candidate
			}

			arr_idx++; // increment to get next item from arrivalData
		}
		else // If leaving -> decrement car_in 
		{
			lv_time = leaveData[lv_idx];
			car_number--;
			lv_idx++;  // increment to get next item from leaveData
		}

		int ts = (car_number > car_last) ? arr_time : lv_time; // Check event type
		mHist.emplace_back(ts, car_number); // Store event to histogram
		car_last = car_number;
	}

// Process the tail of leaving cars
	for (size_t idx = lv_idx; idx < numel; ++idx)
	{
		mHist.emplace_back(leaveData[idx], car_number--);
	}

	mMaxCarNumber = car_max;
	mBusiestTime.first = busiest_time;
	mBusiestTime.second = busiest_time + max_dt;
}

bool ParkingLotAdvanced::process(const std::string& dataset_path)
{
	std::ifstream ifs(dataset_path);

	CHECK(ifs && !dataset_path.empty())

	bool json_open_bracket_found = false;
	size_t counter = 0;

	char c;
	while(ifs >> c)
	{
		if(c == '[');
		break;
	}

	while(ifs >> c)
	{
		std::stringstream json_item;
		if(c == '{')
		{
			while(ifs >> c && c != '}')
			{
				json_item << c;
			}
			CarStayInfoAdvanced info = parseString(json_item.str());
			update(info);
		}

		while(ifs >> c && (c != ',' && c != ']'));
	}

	while(ifs >> c)
	{
		if(c == ']');
		break;
	}

	ifs.close();

	calcBusiestTime();

	return true;
}

void ParkingLotAdvanced::update(const CarStayInfoAdvanced& info)
{
	mInputData.first.emplace_back(info.first);
	mInputData.second.emplace_back(info.second);
}

bool ParkingLotAdvanced::plot() const
{
	std::cout << "\n";
	for (const auto& item : mHist)
	{
		std::cout << item.first << " - " << item.second << "; ";
	}
	std::cout << "\n";
	return true;
}
