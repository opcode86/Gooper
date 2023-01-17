#include "globals.h"
#include "utils.h"

bool Globals::Init() noexcept
{
	globals::g_sGoopFileName = std::string(utils::RandomString());
	globals::g_sGoopFileName.append(".txt");

	globals::g_sSendFileName = std::string(utils::RandomString());
	globals::g_sSendFileName.append(".ps1");

	SECURITY_ATTRIBUTES attr;

	attr.nLength				= sizeof(SECURITY_ATTRIBUTES);
	attr.bInheritHandle			= TRUE;
	attr.lpSecurityDescriptor	= NULL;

	if (!CreatePipe(&globals::g_hStdOutR, &globals::g_hStdOutW, &attr, 0))
		return false;

	if (!SetHandleInformation(globals::g_hStdOutR, HANDLE_FLAG_INHERIT, 0))
		return false;

	return true;
}