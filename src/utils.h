#pragma once
#include <vector>

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

	/**
	 * Spawns a sub-process and runs a command through it.
	 * 
	 * @param process Procescs to be ran, normal command prompt if NULL
	 * @param cmd Command to be passed to the process
	 * @return Boolean, true if nothing failed, false otherwise
	 */
	bool RunSubWorker(const char* process, const char* cmd);

	/**
	 * Checks if string contains ASCII characters.
	 * 
	 * @param str String to be checked
	 * @return Boolean, true if ASCII string, false otherwise
	 */
	bool isASCII(std::string str) noexcept;

	std::string ReadPipeData(void) noexcept;

	__forceinline const char* RandomString() noexcept
	{
		char retVal[32];

		for (int i = 0; i < 31; ++i)
		{
			unsigned int temp;

			do {
				temp = rand();
			} while (
				(temp > 90 || temp < 65) && // Capital letters
				(temp > 57 || temp < 48) && // Lower case letters
				(temp > 122 || temp < 97)   // Numbers
				);

			retVal[i] = (char)temp;
		}

		retVal[31] = '\0';

		return const_cast<char*>(retVal);
	}
}
