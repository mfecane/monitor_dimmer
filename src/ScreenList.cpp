#pragma once
#include "ScreenList.h"

#include <QGuiApplication>
#include <QList>
#include <QScreen>
#include <QDebug>
#include "lib.h"
#include "BackEnd.h"

extern HINSTANCE g_hInst;
extern int g_nCmdShow;

ScreenList::ScreenList() :
    m_screens(QVector<Screen*>())
{
}

void ScreenList::setOpacity(int value) {
    m_opacity = value;
    applyOpacity();
}

void ScreenList::applyOpacity() {
    if(m_screens.size() > 0) {
        for(auto scr : m_screens) {
            if(m_dim) {
                scr->setOpacity(m_opacity);
            }
            else {
                scr->setOpacity(0);
            }
        }
    }
}

void ScreenList::findScreensQt() {
	QList<QScreen*> scrList = QGuiApplication::screens();
    for(int i = 0; i < scrList.size(); ++i) {
        qDebug() << "add screen #" << i;
        auto qtscreen = scrList.at(i);
        Screen* screen = new Screen(qtscreen);
        m_screens.push_back(screen);
	}
    applyOpacity();
}

void ScreenList::deleteScreens() {
    for (int i = 0; i < m_screens.size(); ++i) {
        auto pd = m_screens.at(i);
        delete pd;
    }
    m_screens.clear();
}

void ScreenList::start() {
    if(m_enabled == true) return;
    m_enabled = true;
    findScreensQt();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &ScreenList::checkWindows);
    m_timer->start(100);
}

void ScreenList::stop() {
    if(m_enabled == false) return;
    m_enabled = false;
    deleteScreens();
    delete m_timer;
}

BOOL CALLBACK ScreenList::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam) {
    ScreenList* sl = (ScreenList*)lParam;


    return true;
}

void ScreenList::findScreens() {
    LPARAM lParam = reinterpret_cast<LPARAM>(this);
    bool ret = EnumDisplayMonitors(NULL, NULL, ScreenList::MonitorEnumProc, lParam);
}

bool ScreenList::checkFullscreen() {
    HWND hwnd = GetForegroundWindow();
    return isFullscreen(hwnd);// || true
}

void ScreenList::checkWindows(){

    //object may be invalid
    if(m_enabled == false) return;


    bool ret = checkFullscreen();
    qDebug() << "Checking fullscreen: " << ret;

    for(Screen* scr : m_screens) {
        scr->updateScreen();
    }

//    if(checkFullscreen()) {
//        m_dim = true;
//        applyOpacity();
//    } else {
//        m_dim = false;
//        applyOpacity();
//    }


    //check if app is fullscreen
    //
    //check if monitor configuration changed
    // emit signal();
}
