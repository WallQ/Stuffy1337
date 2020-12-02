#pragma once

#include <windows.h>
#include <string>
#include <vector>
#include <set>
#include <tchar.h>
#include <stdio.h>
#include <algorithm>

class FileOperations {
	public:
		static int SearchDirectory(std::vector < std::string >& refvecFiles,
			const std::string& refcstrRootDirectory,
			const std::set < std::string >& refcstrExtension,
			bool bSearchSubdirectories = true);

		static void SetDirectory(std::string const& path);
		static std::string GetDirectory();
};