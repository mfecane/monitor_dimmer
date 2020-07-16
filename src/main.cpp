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

void RegisterScreenWindowClass();

int main(int argc, char *argv[])
{
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, L"MonitorDimmer.0");
    if (!hMutex) {
        hMutex = CreateMutex(0, 0, L"MonitorDimmer.0");
    } else {
        return 0;
    }

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
