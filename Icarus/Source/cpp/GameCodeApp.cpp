class GameCodeApp {
public:
	bool isOnlyInstance() {
		HANDLE handle = CreateMutex(NULL, TRUE, gameTitle);
		if (GetLastError() != ERROR_SUCCESS) {
			HWND hWnd = FindWindow(gameTitle, NULL);
			if (hWnd) {
				// An instance of your game is already running.
				ShowWindow(hWnd, SW_SHOWNORMAL);
				SetFocus(hWnd);
				SetForegroundWindow(hWnd);
				SetActiveWindow(hWnd);
				return false;
			}
		}
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
			std::cout << "CheckStorage Failure : Not enough physical
				storage.");
			return false;
		}
		return true;
	}


};