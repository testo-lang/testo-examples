
#include "App.hpp"
#include <imgui.h>
#include <Windows.h>
#include <lm.h>
#include <stdexcept>
#include <codecvt>
#include <Shlobj.h>

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


void add_user(const std::string& name) {
	USER_INFO_1 ui;
	DWORD dwLevel = 1;
	DWORD dwError = 0;
	NET_API_STATUS nStatus;

	std::wstring wname = converter.from_bytes(name);

	ui.usri1_name = (LPWSTR)wname.c_str();
	ui.usri1_password = (LPWSTR)wname.c_str();
	ui.usri1_priv = USER_PRIV_USER;
	ui.usri1_home_dir = NULL;
	ui.usri1_comment = NULL;
	ui.usri1_flags = UF_SCRIPT;
	ui.usri1_script_path = NULL;

	nStatus = NetUserAdd(NULL, dwLevel, (LPBYTE)&ui, &dwError);

	if (nStatus != NERR_Success) {
		throw std::runtime_error("NetUserAdd error");
	}
}


App::App(const std::vector<std::string>& args): _args(args)
{
	ImGui::GetIO().IniFilename = nullptr;
}


void App::render() {
	{
		ImGui::SetNextWindowSize({300, 200}, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos({50, 50}, ImGuiCond_FirstUseEver);
		auto users = get_users();
		std::string text = "Users count: " + std::to_string(users.size());
		ImGui::Begin(text.c_str());
		for (auto& user: users) {
			ImGui::Text(user.c_str());
		}
		ImGui::End();
	}

	{
		ImGui::SetNextWindowSize({400, 100}, ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos({50, 300}, ImGuiCond_FirstUseEver);
		ImGui::Begin("New user");
		if (IsUserAnAdmin()) {
			static char user_name[256] = {};
			ImGui::Text("Enter user name here:");
			ImGui::SameLine();
			ImGui::InputText("", user_name, sizeof(user_name));
			if (ImGui::Button("Add!")) {
				add_user(user_name);
				memset(user_name, 0, sizeof(user_name));
			}
		} else {
			ImGui::Text("You need to run this app as an admin to add users");
		}
		ImGui::End();
	}
}
