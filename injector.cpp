#include "afxwin.h"
//#include "stdafx.h"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{

	/*
	 * Load library in which we'll be hooking our functions.
	 */
	//HMODULE dll = LoadLibrary(L"C:\\drivers\\dllinject.dll");
	HMODULE dll = LoadLibrary("dllmain.dll");
	if(dll == NULL) {
		printf("The DLL could not be found.\n");
		getchar();
		return -1;
	}

	/*
	 * Get the address of the function inside the DLL.
	 */
	HOOKPROC addr = (HOOKPROC)GetProcAddress(dll, "meconnect");
	if(addr == NULL) {
		printf("The function was not found.\n");
		getchar();
		return -1;
	}

	/*
	 * Hook the function.
	 */
	HWND windowHandle = FindWindow(NULL, _T("FTL")); // What if there are two windows with the same name?
	printf("%d\n", (int) windowHandle);
	DWORD process_id;
	DWORD threadId = GetWindowThreadProcessId(windowHandle, &process_id);
	HHOOK handle = SetWindowsHookEx(WH_KEYBOARD, addr, dll, threadId);
	if(handle == NULL) {
		printf("The KEYBOARD could not be hooked.\n");
	}

	//HANDLE process_handle = OpenProcess(DELETE, 1, process_id);
	//TerminateProcess(process_handle, 0);
	// This doesn't work.
	
	HANDLE thread_handle = OpenThread(THREAD_TERMINATE, TRUE, threadId);
	//TerminateThread(thread_handle, 0);
	// But this does.

	/*
	 * Unhook the function.
	 */
	printf("Program successfully hooked.\nPress enter to unhook the function and stop the program.\n");
	MSG msg;
	while (1)
	{
		PeekMessage(&msg, windowHandle, 0, 0, PM_REMOVE);

		// do stuff
	}
	UnhookWindowsHookEx(handle);

	return 0;
}
