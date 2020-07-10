#pragma once

#include "windows.h"
#include "pch.h"
#include "lib.h"

#include <QScreen>

class Screen {

public:

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    Screen(QScreen *screen);
	~Screen();

	void Paint();
	bool Init();
	void setOpacity(int opacity);
    void CreateDimWindow();
    void checkForeground();
    void updateScreen();

	HWND getHwnd() { return m_hwnd; }

private:

	HWND m_hwnd = nullptr;

	ID2D1Factory* m_factory = nullptr;
	ID2D1HwndRenderTarget* m_renderTarget = nullptr;
	ID2D1SolidColorBrush* m_brush = nullptr;
    HINSTANCE m_hInstance = NULL;
    int m_nCmdShow = 8;
    QScreen* m_screen = nullptr;

};
