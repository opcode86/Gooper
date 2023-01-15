#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <vector>
#include <string>
#include <windows.h>
#include <fstream>
#include <filesystem>

#include "config.h"
#include "globals.h"

#include "utils.h"

static const std::string abc = OBF("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");//=

std::vector<BYTE> utils::base64_decode(const std::string in) noexcept
{
	std::vector<BYTE> out;

	std::vector<int> T(256, -1);
	for (int i = 0; i < 64; i++) T[abc[i]] = i;

	int val = 0, valb = -8;
	for (unsigned char c : in) {
		if (T[c] == -1) break;
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0) {
			out.push_back(((val >> valb) & 0xFF));
			valb -= 8;
		}
	}
	return out;
}

bool utils::isFile(std::string path) noexcept
{
	bool status = false;
	std::ifstream file(path.c_str());

	if (file)
		status = true;

	file.close();

	return status;
}

bool utils::isFolder(std::string path) noexcept
{
	return std::filesystem::is_directory(path);
}

std::string utils::uCharToString(const unsigned char* str) noexcept
{
	return (std::string)reinterpret_cast<char*>(const_cast<unsigned char*>(str));
}

bool utils::RunSubWorker(const char* process, const char* cmd)
{
	bool retVal = false;

	char pathBuffer[MAX_PATH] = { 0 };
	GetCurrentDirectoryA(MAX_PATH, pathBuffer);


	PROCESS_INFORMATION ProcInfo;
	STARTUPINFOA StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	StartupInfo.hStdOutput = globals::g_hStdOutW;
	StartupInfo.hStdError = globals::g_hStdOutW;
	StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;


	if (CreateProcessA(process, const_cast<char*>(cmd), NULL, NULL, TRUE, NULL, NULL, pathBuffer, &StartupInfo, &ProcInfo))
	{
		WaitForSingleObject(ProcInfo.hProcess, INFINITE);

		CloseHandle(ProcInfo.hProcess);
		CloseHandle(ProcInfo.hThread);

		retVal = true;
	}

	return retVal;
}

bool utils::isASCII(std::string str) noexcept
{
	for (auto x : str)
		if (static_cast<unsigned char>(x) > 127)
			return false;

	return true;
}


std::string utils::ReadPipeData(void) noexcept
{
	if (globals::g_hStdOutR == INVALID_HANDLE_VALUE)
		return { 0 };

	DWORD read;
	char buffer[4096];
	bool status = false;
	std::string retVal;

	for(;;)
	{
		if (!PeekNamedPipe(globals::g_hStdOutR, NULL, 0, NULL, &read, 0))
			break;

		if (read == 0)
			break;

		status = ReadFile(globals::g_hStdOutR, buffer, 4096, &read, NULL);
		if (!status || read == 0)
			break;

		retVal += buffer;
	}

	return retVal;
}