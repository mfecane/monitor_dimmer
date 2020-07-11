#pragma once

#include "windows.h"
#include "pch.h"
#include "lib.h"

#include <QScreen>

class Screen {

public:

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    Screen(HMONITOR hMonitor, LPRECT rect, int opacity);
	~Screen();

	void Paint();
	bool Init();
	void setOpacity(int opacity);
    void CreateDimWindow();
    void updateScreen();

	HWND getHwnd() { return m_hwnd; }

private:

    byte getAlpha(int opacity);

	HWND m_hwnd = nullptr;

	ID2D1Factory* m_factory = nullptr;
	ID2D1HwndRenderTarget* m_renderTarget = nullptr;
    HMONITOR m_hMonitor;
    QRect m_rect;
    bool m_active = true;
    byte m_alpha;

};
