#define NTDDI_VERSION NTDDI_WIN7
#define _WIN32_WINNT _WIN32_WINNT_WIN7

#include "afxwin.h"
#include <windows.h>
#include <Psapi.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

//define DLL_PATH "C:\\Users\\italo\\Documents\\howtogetinside\\dllmain2.dll"
#define DLL_PATH "dllmain2.dll"

HANDLE get_process_by_name(CString);

//int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int _tmain(int argc, _TCHAR* argv[])
{
	//OutputDebugString("Trying to inject...\n");
	printf("Trying to inject...\n");

	HANDLE proc_handle = get_process_by_name("FTLGame.exe");
	//HANDLE proc_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	assert(proc_handle); // TODO show error message
	
	char dll_path[400];
	GetFullPathName(DLL_PATH, 399, dll_path, NULL);

	LPVOID dll_path_addr = (LPVOID) VirtualAllocEx(proc_handle, 0, 380, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	assert(dll_path_addr);

	WriteProcessMemory(proc_handle, dll_path_addr, dll_path, strlen(dll_path), NULL);

	LPVOID load_lib_a_addr = (LPVOID) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
	assert(load_lib_a_addr);

	
	HANDLE threadID = CreateRemoteThread(proc_handle, NULL, 0, (LPTHREAD_START_ROUTINE) load_lib_a_addr, dll_path_addr, NULL, NULL);
	if(threadID == NULL) printf("Error: the remote thread could not be created.\n");
	else printf("Success: the remote thread was successfully created.\n");
	
	CloseHandle(proc_handle);

	return 0;
}

HANDLE get_process_by_name(CString process_name)
{
	assert(process_name);

	DWORD processes[1024], processes_size_bytes, processes_size;
	if ( !EnumProcesses( processes, sizeof(processes), &processes_size_bytes ) )
		return NULL;

	// Calculate how many process identifiers were returned.
	processes_size = processes_size_bytes / sizeof(DWORD);

	// Print the name and process identifier for each process.
	for ( unsigned int i = 0; i < processes_size; i++ )
	{
		DWORD process_id = processes[i];
		// Get a handle to the process.
		HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);

		// Get the process name.
		TCHAR process_retrieved_name[MAX_PATH];
		if (NULL != process_handle)
		{
			HMODULE hMod;
			DWORD cbNeeded;

			if ( EnumProcessModules( process_handle, &hMod, sizeof(hMod), &cbNeeded))
			{
				GetModuleBaseName( process_handle, hMod, process_retrieved_name, sizeof(process_retrieved_name)/sizeof(TCHAR) );
			}
		}

		if(process_name.CompareNoCase(process_retrieved_name) == 0)
			return process_handle;

		CloseHandle(process_handle);
	}

	return NULL;
}
