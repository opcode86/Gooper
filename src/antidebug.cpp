#include <vector>
#include <string>
#include <windows.h>
#include <winternl.h>
#include <tlhelp32.h>

#include "config.h"

#include "antidebug.h"

std::vector<std::string> IllegalPrograms = {
	OBF("ida"),
	OBF("x32dbg"),
	OBF("x64dbg"),
	OBF("ida64"),
	OBF("x86_64-SSE4-AVX2"),
	OBF("wireshark"),
	OBF("processhacker"),
	OBF("netstat"),
	OBF("netmon"),
	OBF("tcpview"),
	OBF("filemon"),
	OBF("regmon"),
	OBF("cain")
};

typedef struct _S_PEB
{
	BYTE pad1[2];
	BOOLEAN BeingDebugged;
	BYTE pad2[17];
	PPEB_LDR_DATA Ldr;
	BYTE pad3[156];
	ULONG NtGlobalFlag;
} S_PEB, * PS_PEB;

using f_NtQueryInformationProcess = NTSTATUS(__stdcall*)(
	_In_ HANDLE,
	_In_  unsigned int,
	_Out_ PVOID,
	_In_ ULONG,
	_Out_ PULONG
	);


AntiDebug::AntiDebug()
{
	this->detected = false;

	if (!config::antidebug)
		return;

	// We want to avoid running as many checks as possible if debugger is detected.
	if (this->CheckRunningProcesses())
		return;
	
	if (this->CloseInvalidHandle())
		return;

	if (this->CheckPEB())
		return;
}

bool AntiDebug::GetDetected() noexcept
{
	return this->detected;
}


bool AntiDebug::CheckRunningProcesses() noexcept
{

	HANDLE ProcSnap = NULL;

	if (ProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); ProcSnap == INVALID_HANDLE_VALUE)
		return false;

	PROCESSENTRY32 currProc32;

	currProc32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(ProcSnap, &currProc32))
		return false;

	while (Process32Next(ProcSnap, &currProc32))
		for (auto x : IllegalPrograms)
			if (strcmp((char*)currProc32.szExeFile, (x + OBF(".exe")).c_str()) == 0)
			{
				this->detected = true;
				return true;
			}

	CloseHandle(ProcSnap);
	return false;
}

bool AntiDebug::CloseInvalidHandle() noexcept
{
	HANDLE h = (HANDLE)0xDEADBEEF;

	__try {
		CloseHandle(h);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return true;
	}

	return false;
}

bool AntiDebug::CheckPEB() noexcept
{
	PROCESS_BASIC_INFORMATION ProcInfo = { 0 };
	ULONG length = 0;

	f_NtQueryInformationProcess NtQueryInformationProcess = (f_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA(OBF("ntdll.dll")), OBF("NtQueryInformationProcess"));

	if (NtQueryInformationProcess == NULL)
		return false;

	if (NT_SUCCESS(NtQueryInformationProcess(GetCurrentProcess(), ProcessBasicInformation, &ProcInfo, sizeof(ProcInfo), &length)))
	{
		PS_PEB Peb = NULL;

		if (Peb = (PS_PEB)ProcInfo.PebBaseAddress; Peb == NULL)
			return false;

		if (Peb->BeingDebugged)
		{
			this->detected = true;
			return true;
		}

		if (Peb->NtGlobalFlag != 0) //0x70 when debugger is attached it seems
		{
			this->detected = true;
			return true;
		}
	}

	return false;
}

bool AntiDebug::CheckRemoteDebugger() noexcept
{
	BOOL retVal;

	if (CheckRemoteDebuggerPresent(NULL, &retVal))
	{
		this->detected = retVal;
		return retVal;
	}

	return false;
}

bool AntiDebug::CheckBeingDebugged() noexcept
{
	BOOL retVal = IsDebuggerPresent();

	this->detected = retVal;
	return retVal;
}