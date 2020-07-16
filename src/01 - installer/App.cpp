
#include "App.hpp"
#include <imgui.h>
#include <experimental/filesystem>

App::App(const std::vector<std::string>& args): _args(args)
{
}


void App::render() {
	ImGui::Begin("Hello world");
	ImGui::SetWindowSize({300, 100});
	std::string text = "MySuperApp is working!";
	if (_args.size() > 1) {
		auto path = std::experimental::filesystem::path(_args.at(1));
		text = "Opened " + path.filename().string();
	}
	ImGui::Text(text.c_str());
	ImGui::End();
}
