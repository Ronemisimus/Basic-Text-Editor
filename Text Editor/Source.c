#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include "resource.h"

const char g_szClassName[] = "myWindowClass";
const char g_szClassNametxt[] = "mytxtboxclass";
char *filepath;
char *file_txt;
HWND hEdit = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK TxtProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK FileProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
char* LoadFile(char* filepath);
char* SaveFile(char* filepath, int len);

//-------------------------------------------------------------------------------
//								A Real Text Editor
//-------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow) {

	int len_s = strlen(lpCmdLine) + 1;
	filepath = malloc(len_s);
	strcpy(filepath, lpCmdLine);
	file_txt = NULL;

	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;
	//Step 1: Registering the Window Class
#pragma region Registration
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_DBLCLKS;
	wc.lpszClassName = g_szClassName;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = IDR_MENU1;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "window registration failed!", "Error!", MB_ICONERROR | MB_OK);
		return 0;
	}
#pragma endregion

	// Step 2: Creating the Window and the edit
#pragma region create


	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, g_szClassName, "editor", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);
	
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	hEdit = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_OLE_PROPPAGE_LARGE), hwnd, TxtProc);
	if (hEdit != NULL) {
		ShowWindow(hEdit, SW_MAXIMIZE);
	}
	else
	{
		MessageBeep(MB_ICONERROR);
		MessageBox(GetModuleHandle(NULL), "Error setting the textbox", "Error", MB_ICONERROR);
		return 0;
	}
#pragma endregion

	// Step 3: get and handle messages
#pragma region Handle 
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#pragma endregion

	if (filepath != NULL)
		GlobalFree(filepath);
	if (file_txt != NULL)
		free(file_txt);

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) 
{
	static int done = 0;
	HDROP hDropInfo = NULL;
	switch (msg)
	{

	case WM_DROPFILES:
	{
		hDropInfo = (HDROP)wParam;
		filepath = GlobalAlloc(GPTR, MAX_PATH * sizeof(char));
		DragQueryFile(hDropInfo, 0, filepath, MAX_PATH);
		LoadFile(filepath);
		SetDlgItemText(hEdit, IDC_EDIT1, file_txt);
		break;
	}
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case ID_HELP_ABOUT:
				{
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROPPAGE_SMALL), hwnd, DlgProc);
					break;
				}
				case ID_FILE_EXIT:
				{
					SendMessage(hwnd, WM_CLOSE, wParam, lParam);
					break;
				}
				case ID_FILE_OPEN:
				{
					DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROPPAGE_LARGE), hwnd, FileProc);
					LoadFile(filepath);
					SetDlgItemText(hEdit, IDC_EDIT1, file_txt);
					break;
				}
				case ID_FILE_SAVE:
				{
					int len = GetWindowTextLength(GetDlgItem(hEdit, IDC_EDIT1));
					if (len == 0) 
					{
						len = 1;
						file_txt = malloc(sizeof(char));
						*file_txt = 0;
					}
					if (len > 0)
					{
						int i;
						char* buf;

						buf = (char*)GlobalAlloc(GPTR, len + 1);
						GetDlgItemText(hEdit, IDC_EDIT1, buf, len + 1);
						file_txt = malloc(sizeof(char) * len);
						strcpy(file_txt, buf);
						DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PROPPAGE_LARGE), hwnd, FileProc);
						SaveFile(filepath, len);
						GlobalFree((HANDLE)buf);
					}
					break;
				}
			}
			break;
		}
		default:
		{
			if (!done) {
				done = 1;
				LoadFile(filepath);
			}
			return DefWindowProc(hwnd, msg, wParam, lParam);
			break;
		}
	}
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_INITDIALOG:
		{
			return TRUE;
		}
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDC_BUTTON1:
				{
					EndDialog(hwnd, 0);
					return TRUE;
				}
				default:
					return FALSE;
			}
		}
		case WM_LBUTTONDOWN:
		{
			return TRUE;
		}
		default: 
		{
			return FALSE;
		}
	}
}

BOOL CALLBACK TxtProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	HDROP hDropInfo = NULL;
	switch (Message)
	{
		case WM_DROPFILES:
		{
			hDropInfo = (HDROP)wParam;
			filepath = GlobalAlloc(GPTR, MAX_PATH * sizeof(char));
			DragQueryFile(hDropInfo, 0, filepath, MAX_PATH);
			LoadFile(filepath);
			SetDlgItemText(hEdit, IDC_EDIT1, file_txt);
			break;
		}
		default:
		{
			return FALSE;
		}
	}
}

BOOL CALLBACK FileProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_INITDIALOG:
	{
		return TRUE;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
			case IDC_OK:
			{
				int len = GetWindowTextLength(GetDlgItem(hwnd, IDC_EDIT2));
				GlobalFree(filepath);
				filepath = NULL;
				if (len > 0)
				{
					int i;

					filepath = (char*)GlobalAlloc(GPTR, len + 1);
					GetDlgItemText(hwnd, IDC_EDIT2, filepath, len + 1);
				}
				EndDialog(hwnd, 0);
				return TRUE;
			}
			default:
				return FALSE;
		}
	}
	case WM_LBUTTONDOWN:
	{
		return TRUE;
	}
	default:
	{
		return FALSE;
	}
	}
}

char* LoadFile(char* filepath)
{
	char *tmp = NULL;
	BOOL buckup = FALSE;
	BOOL errors = FALSE;
	BOOL rl = FALSE;
	BOOL nl = FALSE;
	if (file_txt != NULL) 
	{
		buckup = TRUE;
		tmp = malloc(sizeof(char) * (strlen(file_txt) + 1));
		strcpy(tmp, file_txt);
	}
	FILE *fp = NULL;
	if(filepath!=NULL)
	{
		fp = fopen(filepath, "rt");
	}
	else 
	{
		errors = TRUE;
	}
	
	if (!errors && fp != NULL) 
	{
		int len = 1;
		char buff;
		if (fscanf(fp, "%c", &buff) != 1) 
		{
			errors = TRUE;
		}
		else 
		{
			file_txt = realloc(file_txt, ++len * sizeof(char));
			file_txt[len - 2] = buff;
			while (!feof(fp) && !errors) 
			{
				if (fscanf(fp, "%c", &buff) != 1 && !feof(fp)) 
				{
					errors = TRUE;
				}
				else if (feof(fp)) 
				{

				}
				else 
				{
					if (buff == '\r') {
						rl = TRUE;
					}
					if (buff == '\n')
					{
						nl = TRUE;
					}
					if ((rl && nl) || (!rl && !nl) || (rl && !nl)) {
						file_txt = realloc(file_txt, ++len * sizeof(char));
						file_txt[len - 2] = buff;
						if (rl && nl) {
							rl = FALSE;
							nl = FALSE;
						}
					}
					else {
						len += 2;
						file_txt = realloc(file_txt, len * sizeof(char));
						file_txt[len - 3] = '\r';
						file_txt[len - 2] = '\n';
						nl = FALSE;
					}
				}
			}
			file_txt[len - 1] = 0;
		}
		if (ferror(fp)) 
		{
			errors = TRUE;
		}

		fclose(fp);
	}
	else 
	{
		errors = TRUE;
	}
	if (errors && buckup) 
	{
		if (file_txt != NULL)
			free(file_txt);
		file_txt = NULL;
		file_txt = malloc(sizeof(char) * (strlen(tmp) + 1));
		strcpy(file_txt, tmp);
	}
	if (errors) 
	{
		MessageBox(NULL, "Error loading file(path could be wrong or file is in use)", "Error!", MB_ICONERROR | MB_OK);
	}
}

char* SaveFile(char* filepath, int len)
{
	BOOL errors = FALSE;
	FILE *fp = NULL;
	if (filepath != NULL)
	{
		fp = fopen(filepath, "wt");
	}
	else
	{
		errors = TRUE;
	}

	if (!errors && fp != NULL)
	{
		for (int i = 0; i < len; i++) 
		{
			if (fprintf(fp, "%c", file_txt[i]) != 1) {
				errors = TRUE;
			}
		}
		if (ferror(fp)) {
			errors = TRUE;
		}
		fclose(fp);
	}

	if (errors) 
	{
		MessageBox(NULL, "Error saving file(path could be wrong or file is in use)", "Error!", MB_ICONERROR | MB_OK);
	}
}
