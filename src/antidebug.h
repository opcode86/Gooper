#pragma once

// Methods taken from my AntiDebug/AntiVM repo
// https://github.com/koiradog/AntiDebug-AntiVM/tree/main/AntiDebug

class AntiDebug {
public:
	AntiDebug();

	bool GetDetected() noexcept;

private:
	bool detected;

	bool CheckRunningProcesses() noexcept;

	bool CloseInvalidHandle() noexcept;

	bool CheckPEB() noexcept;

	bool CheckRemoteDebugger() noexcept;

	bool CheckBeingDebugged() noexcept;
};