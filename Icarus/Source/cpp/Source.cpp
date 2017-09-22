#include <Windows.h>
#include <direct.h>
#include <DirectXMath.h>
#include <iostream>
#include <stdio.h>
#include <winreg.h>
#include <cstdlib>
#include <ctime>

#define DIV 1024

void InitInstance();
bool IsOnlyInstance(LPCTSTR gameTitle);
bool CheckStorage(const DWORDLONG diskSpaceNeeded);
bool CheckMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded);
DWORD ReadCPUSpeed();

int main() {
	InitInstance();
	system("pause");
	return 0;
}

void InitInstance() {
	if (!IsOnlyInstance("Hello_World"))
		return;
	if (!CheckStorage(300))
		return;
	if (!CheckMemory(300, 300))
		return;

	std::cout << "CPU Speed: " << ReadCPUSpeed() << std::endl;
		
}

bool IsOnlyInstance(LPCTSTR gameTitle) {
	HANDLE handle = CreateMutex(NULL, TRUE, gameTitle);
	if (GetLastError() != ERROR_SUCCESS) {
		HWND hWnd = FindWindow(gameTitle, NULL);
		if (hWnd) {
			// An instance of your game is already running.
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetFocus(hWnd);
			SetForegroundWindow(hWnd);
			SetActiveWindow(hWnd);
			std::cout << "Other instance found" << std::endl;
			return false;
		}
	}
	std::cout << "No other instance found" << std::endl;
	return true;
}

bool CheckStorage(const DWORDLONG diskSpaceNeeded) {
	// Check for enough free disk space on the current disk.
	int const drive = _getdrive();
	struct _diskfree_t diskfree;
	_getdiskfree(drive, &diskfree);
	unsigned __int64 const neededClusters = diskSpaceNeeded /
		(diskfree.sectors_per_cluster*diskfree.bytes_per_sector);
	if (diskfree.avail_clusters < neededClusters) {
		// if you get here you don’t have enough disk space!
		std::cout << "CheckStorage Failure : Not enough physical storage." << std::endl;
		return false;
	}
	std::cout << "CheckStorage Success." << std::endl;
	return true;
}

bool CheckMemory(const DWORDLONG physicalRAMNeeded, const
	DWORDLONG virtualRAMNeeded) {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status); //Important, not sure why
	//https://msdn.microsoft.com/en-us/library/windows/desktop/aa366589(v=vs.85).aspx
	GlobalMemoryStatusEx(&status);
	if (status.ullTotalPhys < physicalRAMNeeded) {
		/* you don’t have enough physical memory. Tell the
		player to go get a real computer and give this one to
		his mother. */
		std::cout << "CheckMemory Failure : Not enough physical memory." << std::endl;
		return false;
	}
	// Check for enough free memory.
	if (status.ullAvailVirtual < virtualRAMNeeded) {
		// you don’t have enough virtual memory available.
		// Tell the player to shut down the copy of Visual Studio running in the background.
			std::cout << "CheckMemory Failure : Not enough virtual memory." << std::endl;
		return false;
	}
	char *buff = new char[virtualRAMNeeded];
	if (buff)
		delete[] buff;
	else {
		std::cout << "CheckMemory Failure : Not enough contiguous memory." << std::endl;
		return false;
	}

	std::cout << "CheckMemory Success:" << std::endl
		<< "	Physical RAM Available: " << status.ullTotalPhys /DIV/DIV << "MB" << std::endl
		<< "	Virtual RAM Available:  " << status.ullAvailVirtual /DIV/DIV << "MB" << std::endl;
	return true;
}

DWORD ReadCPUSpeed() {
	DWORD BufSize = sizeof(DWORD);
	DWORD dwMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;
	// open the key where the proc speed is hidden:
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);
	if (lError == ERROR_SUCCESS) {
		// query the key:
		RegQueryValueEx(hKey, "~MHz", NULL, &type, (LPBYTE)
			&dwMHz, &BufSize);
	}
	return dwMHz;
}