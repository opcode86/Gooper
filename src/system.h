#pragma once
#include "goopbase.h"

class System : public GoopBase {
public:
	System() noexcept;

private:
	void GetSystemInfo() noexcept;
};