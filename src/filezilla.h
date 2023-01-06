#pragma once
#include <string>

#include "goopbase.h"

class Filezilla : public GoopBase
{
public:
	Filezilla(std::string) noexcept;

private:
	size_t lengthOfSubstr(std::string str) noexcept;
};