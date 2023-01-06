#pragma once

#include "goopbase.h"

#include "config.h"

// This class is messy...
class Chrome : public GoopBase {
public:
	Chrome(std::string path) noexcept;

private:
	DATA_BLOB key;

	bool IsVersion10(std::string key) noexcept;

	bool GetKey(std::string path) noexcept;

	std::string DecryptOld(DATA_BLOB encrypted) noexcept;

	std::string DecryptNew(std::string encrypted) noexcept;

	std::string CleanDecrypted(std::string decrypted) noexcept;
};