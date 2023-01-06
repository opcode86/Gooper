#include <string>

#include "goopbase.h"

bool GoopBase::GetStatus() noexcept
{
	return !(this->data.empty());
}

std::string GoopBase::GetData() noexcept
{
	return this->data;
}