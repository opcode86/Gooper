#pragma once

class AntiVM {
public:
	AntiVM();

	bool GetDetected() noexcept;

private:
	bool detected;

	bool CheckDrivers() noexcept;

	bool CheckScreen() noexcept;

	bool CheckDeviceVendors() noexcept;

	bool CheckDlls() noexcept;
};