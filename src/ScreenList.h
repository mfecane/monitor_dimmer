#pragma once
#include "pch.h"
#include "Screen.h"
#include <vector>
#include <QTimer>

#define ScreenManager ScreenList //TODO: rename
#define sloths slots // For lol'zZz

class ScreenList : public QObject {

    Q_OBJECT

public:

    ScreenList();

    void findScreensQt();
    void deleteScreens();

    bool checkFullscreen();

    void setOpacity(int value);

    void start();
    void stop();
    void findScreens();

    static BOOL CALLBACK MonitorEnumProc(HMONITOR Arg1, HDC Arg2, LPRECT Arg3, LPARAM Arg4);

public sloths:

    void checkWindows();

private:

    bool m_enabled = false;
    bool m_dim = false;
    bool temporarydim = false; // TODO: while mouse down on control
	WNDCLASSEX m_windowclass;
    QVector<Screen*> m_screens;
    QTimer* m_timer;
    int m_opacity = 0;
    int m_maxopacity = 0;

    void applyOpacity();
};
