#pragma once
#include <string>
#include <windows.h>

namespace globals {
	inline HANDLE g_hStdOutR = NULL;
	inline HANDLE g_hStdOutW = NULL;

	inline std::string g_sGoopFileName;
	inline std::string g_sSendFileName;
};

class Globals {
public:
	bool Init() noexcept;
};