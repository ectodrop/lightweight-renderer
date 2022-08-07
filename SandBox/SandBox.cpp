// SandBox.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <any>

#include <iostream>
#include <Windows.h>
#include <commdlg.h>
#include <vector>
#include <functional>

OPENFILENAME ofn;
DWORD error_value;

using namespace std;

struct SomeStruct {
	int x;
};

bool openFileDialog(TCHAR szFileName[])
{
	const TCHAR* FilterSpec = TEXT("All Files(.)\0*.*\0");
	const TCHAR* Title = TEXT("Open");

	const TCHAR* myDir = TEXT("C:\\c_plus_plus_trial");

	TCHAR szFileTitle[MAX_PATH] = { '\0' };

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	*szFileName = 0;

	/* fill in non-variant fields of OPENFILENAME struct. */
	ofn.lStructSize = sizeof(OPENFILENAME);

	ofn.hwndOwner = GetFocus();
	ofn.lpstrFilter = FilterSpec;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = myDir; // Initial directory.
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrTitle = Title;
	//ofn.lpstrDefExt = 0; // I've set to null for demonstration
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	// false if failed or cancelled
	if (GetOpenFileName(&ofn) == 1)
	{
		cout << "SUCCESS !!! \n";
		return 1;
	}
	else
	{
		cout << "Failure :( \n";
		error_value = CommDlgExtendedError();
		cout << std::hex << error_value;

		return 0;
	}
}

std::unique_ptr<int> GenPointer() {
	auto pointer = std::make_unique<int>(12);

	return pointer;
}

int main()
{
	std::function<void(std::any)> func = [](std::any a) {
		cout << std::any_cast<int>(a) << endl;
	};
	func(3);
}

