
#include "App.hpp"
#include <imgui.h>
#include <Windows.h>
#include <lm.h>
#include <stdexcept>
#include <codecvt>

#pragma comment(lib, "netapi32.lib")

std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

std::vector<std::string> get_users() {
	std::vector<std::string> result;
	DWORD dwResumeHandle = 0;
	LPTSTR pszServerName = NULL;
	DWORD dwLevel = 0;
	LPUSER_INFO_0 pBuf = NULL;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;
	auto nStatus = NetUserEnum(
		pszServerName,
		dwLevel,
		FILTER_NORMAL_ACCOUNT,
		(LPBYTE*)&pBuf,
		dwPrefMaxLen,
		&dwEntriesRead,
		&dwTotalEntries,
		&dwResumeHandle);

	if (nStatus == ERROR_MORE_DATA) {
		return result;
	}

	if (nStatus != NERR_Success) {
		throw std::runtime_error("NetUserEnum error");
	}

	for (DWORD i = 0; i < dwEntriesRead; i++)
	{
		LPCWSTR str = (pBuf + i)->usri0_name;
		std::string name = converter.to_bytes(str);
		result.push_back(name);
	}

	NetApiBufferFree(pBuf);

	return result;
}




App::App(const std::vector<std::string>& args): _args(args)
{
}


void App::render() {
	ImGui::Begin("Hello world");
	ImGui::SetWindowSize({300, 200});
	auto users = get_users();
	std::string text = "Users count: " + std::to_string(users.size());
	ImGui::Text(text.c_str());
	for (auto& user: users) {
		ImGui::Text(user.c_str());
	}
	ImGui::End();
}
