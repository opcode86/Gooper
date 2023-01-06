#pragma once

/* ------------------------------- */
/* Base class for all goop classes */
/* ------------------------------- */

class GoopBase
{
public:
	bool GetStatus() noexcept;

	std::string GetData() noexcept;

protected:
	std::string data;
};