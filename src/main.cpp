#pragma once

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")

#include "pch.h"

#include <iostream>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QMessageBox>
#include <QObject>
#include <QUrl>
#include <QWindow>

#include "BackEnd.h"
#include "ScreenList.h"
#include "res.h"

HINSTANCE g_hInst = NULL;
int g_nCmdShow = 9;
ScreenList* g_screenList = nullptr;
HWND g_hwnd;

class __declspec(uuid("2D8187F2-29DF-440E-836C-CB406BCA29A0")) TrayIcon;

void RegisterScreenWindowClass();
HWND CreateMainWindow();
bool CreateNotificationIcon(HWND hwnd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;

//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmd, int nCmdShow) {
//    g_hInst = hInstance;
//    g_nCmdShow = nCmdShow;
//
//    RegisterScreenWindowClass();
//    HWND hwnd = CreateMainWindow();
//
//    if (hwnd) {
//        ShowWindow(hwnd, SW_HIDE);
//		RECT rect2 = {0,0,0,0};
//        Screen* scr = new Screen(hInstance, nCmdShow, rect2); // TODO: remove
//
//        MSG message;
//        while (GetMessage(&message, NULL, 0, 0)) {
//            DispatchMessage(&message);
//        }
//    }
//    else {
//        ErrorExit(TEXT("CreateWindow"));
//        return -1;
//    }
//
//    return 0;
//}

int main(int argc, char *argv[]) {
    Q_INIT_RESOURCE(res);

    g_hInst = (HINSTANCE)::GetModuleHandle(NULL);
    g_nCmdShow = 10;

    RegisterScreenWindowClass();

    QCoreApplication::setOrganizationName("Gavitka software");
    QCoreApplication::setOrganizationDomain("gavitka.com");
    QCoreApplication::setApplicationName("Dimmer");

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    //QGuiApplication::setQuitOnLastWindowClosed(false);

    QApplication app(argc, argv);
    QQmlApplicationEngine engine;
   
    engine.addImportPath("qrc:/imports");

    g_screenList = new ScreenList();

    qmlRegisterSingletonType<BackEnd>("io.qt.examples.backend", 1, 0, "BackEnd", &BackEnd::qmlInstance);

    engine.load(QUrl(QStringLiteral("qrc:/ui.qml")));
	QObject* m_rootObject = engine.rootObjects().first();
	if (m_rootObject) {
		QWindow* window = qobject_cast<QWindow*>(m_rootObject);
		if (window) {
			g_hwnd = (HWND)window->winId();
		}
    }

    return app.exec();
    delete g_screenList;
}


void RegisterScreenWindowClass() {
    WNDCLASSEX windowclass;
    ZeroMemory(&windowclass, sizeof(WNDCLASSEX));
    windowclass.cbSize = sizeof(WNDCLASSEX);
    windowclass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    windowclass.hInstance = g_hInst;
    windowclass.lpfnWndProc = Screen::WindowProc;
    windowclass.lpszClassName = L"CoverWindow";
    windowclass.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClassEx(&windowclass);
}


HWND CreateMainWindow() {
    WNDCLASSEX windowclass;
    ZeroMemory(&windowclass, sizeof(WNDCLASSEX));
    windowclass.cbSize = sizeof(WNDCLASSEX);
    windowclass.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    windowclass.hInstance = g_hInst;
    windowclass.lpfnWndProc = WndProc;
    windowclass.lpszClassName = L"ShitWindow";
    windowclass.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClassEx(&windowclass);

    return CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        L"ShitWindow",
        L"MainWindow2",
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        250,
        200,
        NULL,
        NULL,
        g_hInst,
        NULL);
}

bool CreateNotificationIcon(HWND hwnd) {
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.hWnd = hwnd;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP | NIF_GUID;
	nid.guidItem = __uuidof(TrayIcon);
	nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;    LoadIconMetric(g_hInst, MAKEINTRESOURCE(IDR_TRAYICON), LIM_SMALL, &nid.hIcon);
	StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Hui");
	Shell_NotifyIcon(NIM_ADD, &nid);
	nid.uVersion = NOTIFYICON_VERSION_4;
	return Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

bool DeleteNotificationIcon() {
	NOTIFYICONDATA nid = { sizeof(nid) };
	nid.uFlags = NIF_GUID;
	nid.guidItem = __uuidof(TrayIcon);
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

void ShowContextMenu(HWND hwnd, POINT pt) {
	HMENU hMenu = LoadMenu(g_hInst, MAKEINTRESOURCE(IDC_CONTEXTMENU));
	if (hMenu)
	{
		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (hSubMenu)
		{
			// our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
			SetForegroundWindow(hwnd);

			// respect menu drop alignment
			UINT uFlags = TPM_RIGHTBUTTON;
			if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
			{
				uFlags |= TPM_RIGHTALIGN;
			}
			else
			{
				uFlags |= TPM_LEFTALIGN;
			}

			TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hwnd, NULL);
		}
		DestroyMenu(hMenu);
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_CREATE:
		// add the notification icon
		if (!CreateNotificationIcon(hwnd)) {
			MessageBox(hwnd,
				L"Error adding icon",
				L"Error adding icon", MB_OK);
			return -1;
		}
		break;
	case WM_COMMAND: {
		int const wmId = LOWORD(wParam);
		switch (wmId) {
		case IDM_EXIT:
			DestroyWindow(hwnd);
			break;
		}
	}
				   break;
	case WMAPP_NOTIFYCALLBACK:
		switch (LOWORD(lParam)) {
		case WM_CONTEXTMENU: {
			POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
			ShowContextMenu(hwnd, pt);
		}
						   break;
		}
		break;
	case WM_DESTROY:
		DeleteNotificationIcon();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}
