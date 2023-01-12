#include "globals.h"

bool Globals::Init() noexcept
{
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