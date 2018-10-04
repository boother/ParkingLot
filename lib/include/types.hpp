#pragma once

#ifdef CHECK
#	undef CHECK
#endif

#define CHECK(condition)              										\
  if (!(condition)) 														\
     throw std::runtime_error(std::string("Check failed: ") + #condition " ");

namespace pl
{
constexpr char TS_SEPARATOR{ ':' };
constexpr uint8_t MINS_IN_HOUR{ 60 };
constexpr uint8_t HOURS_IN_DAY{ 24 };
constexpr uint16_t MINS_IN_DAY = MINS_IN_HOUR * HOURS_IN_DAY;

const std::string format = "%Y-%m-%dT%H:%M:%S";

using DateTimeStruct = std::tm;

enum class Mode
{
	BASE,
	ADVANCED
};

struct BusiestTime
{
	BusiestTime() {};
	BusiestTime(time_t t0, time_t t1, size_t numel)
		: t0(t0)
		, t1(t1)
		, numel(numel)
	{

	}

	time_t t0 = 0;
	time_t t1 = 0;
	size_t numel = 0;
};

struct Timestamp
{
	Timestamp()
		: value{}
	{}

	Timestamp(uint16_t timestamp)
	{
		CHECK(timestamp <= MINS_IN_DAY)
			value = timestamp;
	}

	Timestamp(uint8_t hour, uint8_t min)
	{
		CHECK(hour < pl::HOURS_IN_DAY)
			CHECK(min < pl::MINS_IN_HOUR)

			value = cvtToTimestamp(hour, min);
	}

	uint16_t cvtToTimestamp(uint8_t hour, uint8_t min)
	{
		return hour * MINS_IN_HOUR + min;
	}

	void fromString(const std::string& str, size_t pos)
	{
		CHECK(str.size() >= 5 && pos != std::string::npos)

			uint16_t hour = std::stoi(str.substr(pos - 2, pos));
		uint16_t min = std::stoi(str.substr(pos + 1, pos + 3));

		CHECK(hour < pl::HOURS_IN_DAY)
			CHECK(min < pl::MINS_IN_HOUR)

			value = cvtToTimestamp(hour, min);
	}

	uint16_t value;
};

using CarStayInfo = std::pair<Timestamp, Timestamp>;
using CarStayInfoAdvanced = std::pair<std::time_t, std::time_t>;

using DateTimeRange = std::pair<DateTimeStruct, DateTimeStruct>;

} // pl