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
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &ScreenList::checkWindows);
}

void ScreenList::setOpacity(int value) {
    m_opacity = value;
    for(auto scr : m_screens) {
        scr->setOpacity(m_opacity);
    }
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
    findScreens();
    m_timer->start(100);
}

void ScreenList::stop() {
    if(m_enabled == false) return;
    m_enabled = false;
    deleteScreens();
    m_timer->stop();
}

int ScreenList::getOpacity() {
    return m_opacity;
}

BOOL CALLBACK ScreenList::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam) {
    ScreenList* sl = (ScreenList*)lParam;

    Screen* scr = new Screen(hMonitor, lprcMonitor, sl->getOpacity());
    qDebug() << "add screen #" << hMonitor;
    sl->m_screens.push_back(scr);

    return true;
}

void ScreenList::findScreens() {
    LPARAM lParam = reinterpret_cast<LPARAM>(this);
    bool ret = EnumDisplayMonitors(NULL, NULL, ScreenList::MonitorEnumProc, lParam);
}

void ScreenList::checkWindows(){
    if(m_enabled == false) return;

    for(Screen* scr : m_screens) {
        scr->updateScreen();
        setOpacity(m_opacity);
    }
}
