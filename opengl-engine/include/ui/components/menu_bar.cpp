#pragma once
#include <ui/ui_component.h>
#include <windows_dialog.h>
class MenuBarComponent : public UIComponent {
	using UIComponent::UIComponent;
public:
	void Render() override {
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Import .obj")) {
					std::string file_name;
					if (WindowsDialog::OpenFileDialog(file_name)) {
						InvokeUICallback("loader-open-file", file_name);
					}
				}
				if (ImGui::MenuItem("Save Scene")) {
					std::string file_name;
					if (WindowsDialog::OpenSaveDialog(file_name)) {
						InvokeUICallback("export-scene", file_name);
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Render")) {
				if (ImGui::MenuItem("render path traced scene")) {
					_is_rendering_scene = !_is_rendering_scene;
					InvokeUICallback("render-traced-scene", _is_rendering_scene);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Debug")) {
				if (ImGui::MenuItem("recompile shaders")) {
					InvokeUICallback("recompile-shaders", 0);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
private:
	bool _is_rendering_scene = false;
	
};
