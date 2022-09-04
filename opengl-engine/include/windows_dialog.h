#pragma once
#include "pch.h"

static class WindowsDialog {
public:
	static bool OpenFileDialog(std::string& out_file) {
		// NEEDS TO HAVE MULTI-BYTE CHAR SET RATHER THAN UNICODE
		const TCHAR* FilterSpec = "All Files(.)\0*.*\0";
		const TCHAR* Title = "Open";
		OPENFILENAME ofn = { 0 };
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
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;


		if (GetOpenFileName(&ofn) == 1) {
			out_file = std::string(szFileName);
			return true;
		}
		else {
			std::cout << "Failed to open file" << std::endl;
			return false;
		}
	}

	static bool OpenSaveDialog(std::string& out_file) {
		//Save Dialog
		const TCHAR* FilterSpec = "All Files(.)\0*.*\0";
		OPENFILENAME saveFileDialog = { 0 };
		char szSaveFileName[MAX_PATH] = { 0 };
		saveFileDialog.lStructSize = sizeof(OPENFILENAME);
		saveFileDialog.hwndOwner = GetFocus();
		saveFileDialog.lpstrFilter = FilterSpec;
		saveFileDialog.lpstrCustomFilter = NULL;
		saveFileDialog.nMaxCustFilter = 0;
		saveFileDialog.nFilterIndex = 0;
		saveFileDialog.lpstrFile = szSaveFileName;
		saveFileDialog.nMaxFile = MAX_PATH;
		saveFileDialog.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

		if (GetSaveFileName(&saveFileDialog)) {
			out_file = std::string(saveFileDialog.lpstrFile);
			return true;
		}
		std::cout << "Could not save to file" << std::endl;
		return false;
	}
};