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

std::string Crypt::GenerateRandomPassword() {
	static const char data[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"*/&%!=-";
	char s[RANDOM_PASSWORD_LENGTH + 1];
	srand(time(NULL));

	for (int i = 0; i < RANDOM_PASSWORD_LENGTH; ++i) {
		s[i] = data[rand() % (sizeof(data) - 1)];
	}

	s[RANDOM_PASSWORD_LENGTH] = 0;
	std::string password(s);
	return password;
}

std::string Crypt::Base64Encode(const std::string& in) {
	std::string out;

	int val = 0, valb = -6;
	for (unsigned char c : in) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}
	if (valb > -6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val << 8) >> (valb + 8)) & 0x3F]);
	while (out.size() % 4) out.push_back('=');
	return out;
}

string Crypt::XOR(string data) {
	char key[] = PASSWORD;

	string xorstring = data;
	for (int i = 0; i < xorstring.size(); i++) {
		xorstring[i] = data[i] ^ key[i % (sizeof(key) / sizeof(char))];
	}
	return xorstring;
}

bool Crypt::LockFile(LPTSTR pszSourceFile, LPTSTR pszDestinationFile, LPTSTR pszPassword) {
	bool fReturn = false;

	HANDLE hSourceFile = INVALID_HANDLE_VALUE;
	HANDLE hDestinationFile = INVALID_HANDLE_VALUE;

	HCRYPTPROV hCryptProv = NULL;
	HCRYPTKEY hKey = NULL;
	HCRYPTHASH hHash = NULL;

	PBYTE pbBuffer = NULL;
	DWORD dwBlockLen;
	DWORD dwBufferLen;
	DWORD dwCount;

	hSourceFile = CreateFile(pszSourceFile, FILE_READ_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hSourceFile) {
		std::cout << "The source plaintext file is open." << endl;
	}
	else {
		std::cout << "Error opening source plaintext file! " + GetLastError() << endl;
	}

	hDestinationFile = CreateFile(pszDestinationFile, FILE_WRITE_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hDestinationFile) {
		std::cout << "The destination file is open." << endl;
	}
	else {
		std::cout << "Error opening destination  file! " + GetLastError() << endl;
	}

	if (CryptAcquireContext(&hCryptProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0)) {
		std::cout << "A cryptographic provider has been acquired." << endl;
	}
	else {
		std::cout << "Error during CryptAcquireContext! " + GetLastError() << endl;
	}

	if (CryptCreateHash(hCryptProv, CALG_MD5, 0, 0, &hHash)) {
		std::cout << "A hash object has been created." << endl;
	}
	else {
		std::cout << "Error during CryptCreateHash! " + GetLastError() << endl;
	}

	if (CryptHashData(hHash, (BYTE*)pszPassword, lstrlen(pszPassword), 0)) {
		std::cout << "The password has been added to the hash." << endl;
	}
	else {
		std::cout << "Error during CryptHashData. " + GetLastError() << endl;
	}

	if (CryptDeriveKey(hCryptProv, ENCRYPT_ALGORITHM, hHash, KEYLENGTH, &hKey)) {
		std::cout << "An encryption key is derived from the password hash." << endl;
	}
	else {
		std::cout << "Error during CryptDeriveKey! " + GetLastError() << endl;
	}

	dwBlockLen = 1000 - 1000 % ENCRYPT_BLOCK_SIZE;

	if (ENCRYPT_BLOCK_SIZE > 1) {
		dwBufferLen = dwBlockLen + ENCRYPT_BLOCK_SIZE;
	}
	else {
		dwBufferLen = dwBlockLen;
	}

	if (pbBuffer = (BYTE*)malloc(dwBufferLen)) {
		std::cout << "Memory has been allocated for the buffer." << endl;
	}
	else {
		std::cout << "Out of memory. " + E_OUTOFMEMORY << endl;
	}

	bool fEOF = FALSE;

	do {
		if (!ReadFile(hSourceFile, pbBuffer, dwBlockLen, &dwCount, NULL)) {
			std::cout << "Error reading plaintext! " + GetLastError() << endl;
		}
		if (dwCount < dwBlockLen) {
			fEOF = TRUE;
		}
		if (!CryptEncrypt(hKey, NULL, fEOF, 0, pbBuffer, &dwCount, dwBufferLen)) {
			std::cout << "Error during CryptEncrypt. " + GetLastError() << endl;
		}
		if (!WriteFile(hDestinationFile, pbBuffer, dwCount, &dwCount, NULL)) {
			std::cout << "Error writing ciphertext. " + GetLastError() << endl;
		}

	} while (!fEOF);

	fReturn = true;

	if (hSourceFile) {
		CloseHandle(hSourceFile);
	}
	if (hDestinationFile) {
		CloseHandle(hDestinationFile);
	}
	if (pbBuffer) {
		free(pbBuffer);
	}

	if (hHash) {
		if (!(CryptDestroyHash(hHash))) {
			std::cout << "Error during CryptDestroyHash!" << GetLastError() << endl;
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