#pragma once

namespace utils {
	/**
	 * Base64 decodes a given string and returns it as a BYTE vector
	 * Stolen from: https://gist.github.com/williamdes/308b95ac9ef1ee89ae0143529c361d37
	 *
	 * @param in String to be decoded
	 * @return Resulting BYTE vector
	 */
	std::vector<BYTE> base64_decode(const std::string in) noexcept;

	/**
	 * Validates file
	 *
	 * @param name Name of the file to be examined
	 * @return True if no errors during check, otherwise false
	 */
	bool isFile(std::string path) noexcept;

	bool isFolder(std::string path) noexcept;

	/**
	 * Converst a const unisgned char pointer to a string
	 *
	 * @param str Variable to be converted
	 * @return Converted string
	 */
	std::string uCharToString(const unsigned char* str) noexcept;

	template <typename T>
	bool bCheckForDuplicate(std::vector<T> a, T b) noexcept
	{
		return std::find(a.begin(), a.end(), b) != a.end();
	}

	bool RunSubWorker(const char* process, const char* cmd);
}