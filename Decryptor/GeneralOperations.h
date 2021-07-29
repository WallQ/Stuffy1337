#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <Lmcons.h>
#include <iomanip>

class GeneralOperations {
	public:
		static std::string ComputerName();
		static std::string Username();
		static void GetHardDiskDrivesNames(std::vector<std::string>& strIPAddresses);
		static void UnlockFiles(std::string const& key);
};