#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <tchar.h>
#include <stdio.h>
#include <wincrypt.h>
#include <conio.h>
#include <ctime>

class Crypt {
public:
	static bool ValidatePassword(std::string const& key);
	static std::string Base64Decode(const std::string& in);
	static std::string XOR(std::string data);
	bool UnlockFile(LPTSTR szSource, LPTSTR szDestination, LPTSTR szPassword);
};