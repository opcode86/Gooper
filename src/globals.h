#pragma once
#include <windows.h>

namespace globals {
	inline HANDLE g_hStdOutR = NULL;
	inline HANDLE g_hStdOutW = NULL;
};

class Globals {
public:
	bool Init() noexcept;
};