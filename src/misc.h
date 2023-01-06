#pragma once
#include "config.h"

namespace misc
{
	bool SendData(std::string data) noexcept;

	void Cleanup() noexcept;

	void SelfDelete() noexcept;
}