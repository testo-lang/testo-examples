
#include "App.hpp"
#include <imgui.h>
#include <experimental/filesystem>

App::App(const std::vector<std::string>& args): _args(args)
{
	ImGui::GetIO().IniFilename = nullptr;
	ImGui::StyleColorsLight();
}


void App::render() {
	ImGui::SetNextWindowSize({300, 100}, ImGuiCond_FirstUseEver);
	ImGui::Begin("Hello world");
	std::string text = "MySuperApp is working!";
	if (_args.size() > 1) {
		auto path = std::experimental::filesystem::path(_args.at(1));
		text = "Opened " + path.filename().string();
	}
	ImGui::Text(text.c_str());
	ImGui::End();
}
