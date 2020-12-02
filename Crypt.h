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
		static std::string GenerateRandomPassword();
		static std::string XOR(std::string data);
		static std::string Base64Encode(const std::string& in);
		bool LockFile(LPTSTR szSource, LPTSTR szDestination, LPTSTR szPassword);
};