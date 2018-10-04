#include <string>
#include <iostream>
#include <memory>

#include "ParkingLotBase.hpp"
#include "ParkingLotAdvanced.hpp"

int main(int argc, char* argv[])
{
	std::string dataset_path;
	size_t working_mode = 0;

	if(argc == 3)
	{
		working_mode = std::stoi(argv[1]);
		dataset_path = argv[2];
		std::cout << "Working mode: " << ( (working_mode == 0) ? "BASE" : "ADVANCED") << '\n';
		std::cout << "Dataset Path: " << dataset_path << '\n';
	}
	else
	{
		std::cout << "Example usage: " << argv[0] << " 0<BASE> OR 1<ADVANCED> path/to/dataset.txt" << '\n';
		return -1;
	}

	std::unique_ptr<pl::IParkingLot> parking_lot;
	switch (static_cast<pl::Mode>(working_mode))
	{
		case pl::Mode::BASE:
			parking_lot.reset(new pl::ParkingLotBase());
		break;
		case pl::Mode::ADVANCED:
			parking_lot.reset(new pl::ParkingLotAdvanced());
		break;
		default:
			std::cerr << "Unknown mode" << '\n';
			return -1;
		break;
	}

	bool state = parking_lot->process(dataset_path);

	if (!state)
	{
		return -1;
	}

	std::pair<pl::DateTimeStruct, pl::DateTimeStruct> res = parking_lot->getBusiestTime();
	std::cout << "\nBusiest Time: ";
	if (static_cast<pl::Mode>(working_mode) == pl::Mode::BASE)
	{
		std::cout << "from " << res.first.tm_hour << ":" << res.first.tm_min << " ";
		std::cout << "to "   << res.second.tm_hour << ":" << res.second.tm_min << "\n";
	}
	else
	{
		char buffer[30];
		strftime(buffer, sizeof(buffer), pl::format.c_str(), &res.first);

		std::cout << "at " << std::string(buffer) << "\n";
	}

#if 0
	parking_lot->plot();
#endif

	return 0;
}
