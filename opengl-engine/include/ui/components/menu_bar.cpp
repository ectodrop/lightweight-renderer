#ifndef FILE_LOADER_H
#define FILE_LOADER_H
#include <ui/ui_component.h>

class MenuBarComponent : public UIComponent {
	using UIComponent::UIComponent;
	void Render() override {
		{
			if (ImGui::BeginMainMenuBar()) {
				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Import .obj")) {
						// NEEDS TO HAVE MULTI-BYTE CHAR SET RATHER THAN UNICODE
						const TCHAR* FilterSpec = "All Files(.)\0*.*\0";
						const TCHAR* Title = "Open";
						OPENFILENAME ofn = { 0 };
						const TCHAR* myDir = "C:\\c_plus_plus_trial";
						TCHAR szFileName[MAX_PATH] = { '\0' };
						TCHAR szFileTitle[MAX_PATH] = { '\0' };


						ofn.lpstrFile = szFileName;

						/* fill in non-variant fields of OPENFILENAME struct. */
						ofn.lStructSize = sizeof(OPENFILENAME);

						ofn.hwndOwner = GetFocus();
						ofn.lpstrFilter = FilterSpec;
						ofn.lpstrCustomFilter = NULL;
						ofn.nMaxCustFilter = 0;
						ofn.nFilterIndex = 0;
						ofn.nMaxFile = MAX_PATH;
						// ofn.lpstrInitialDir = myDir; // Initial directory.
						ofn.lpstrFileTitle = szFileTitle;
						ofn.nMaxFileTitle = MAX_PATH;
						ofn.lpstrTitle = Title;
						ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;


						if (GetOpenFileName(&ofn) == 1) {
							std::cout << szFileName << std::endl;
							ui_callbacks["loader-open-file"](std::string(szFileName));
						}
						else {
							std::cout << "Failed to open file" << std::endl;
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}
			std::cout << ImGui::IsWindowHovered() << std::endl;
		}
	}
};

#endif