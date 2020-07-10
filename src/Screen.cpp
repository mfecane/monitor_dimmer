#pragma once

#include "pch.h"
#include "winuser.h"
#include "Screen.h"
#include <iostream>
#include "lib.h"
#include <QDebug>

extern HWND g_hwnd;

extern HINSTANCE g_hInst;
extern int g_nCmdShow;

LRESULT CALLBACK Screen::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	Screen* pThis;

	if (uMsg == WM_NCCREATE)
	{
		pThis = static_cast<Screen*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

		SetLastError(0);
		if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis)))
		{
			if (GetLastError() != 0)
				return FALSE;
		}
	}
	else
	{
		pThis = reinterpret_cast<Screen*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	if (uMsg == WM_DESTROY) {
        //PostQuitMessage(0);
		return 0;
	}

	if (uMsg == WM_PAINT) {
		pThis->Paint();
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Screen::Screen(QScreen* screen) {
    m_screen = screen;
    CreateDimWindow();
}

void Screen::CreateDimWindow() {
//    RECT rect = { 0, 0, 800, 600 };
//    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

    QRect qrect = m_screen->geometry();

//    RECT rect;
//    rect.left = qrect.left();
//    rect.top = qrect.top();
//    rect.right = qrect.right();
//    rect.bottom = qrect.bottom();

    m_hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"CoverWindow",
        L"DirectX Tut",
        WS_POPUP,
        qrect.left(),
        qrect.top(),
        qrect.width(),
        qrect.height(),
        g_hwnd,
        nullptr,
        m_hInstance,
        this);

    if (!m_hwnd) {
        ErrorExit(TEXT("CreateWindow"));
    }

    const MARGINS margin = { -1 };
    SetLayeredWindowAttributes(m_hwnd, 0, (int)0, LWA_ALPHA); // opacity
    DwmExtendFrameIntoClientArea(m_hwnd, &margin);

    Init();

    ShowWindow(m_hwnd, 8);
}

void Screen::updateScreen() {
    HWND hwnd = GetForegroundWindow();
    HANDLE screenhandle = (HANDLE)m_screen->handle();
    HANDLE activescreenhandle = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    qDebug() << "foreground hwnd" << hwnd;
    qDebug() << "activescreenhandle" << activescreenhandle;
    qDebug() << "screenhandle" << screenhandle;
}

Screen::~Screen() {
	if (m_factory) m_factory->Release();
	if (m_renderTarget) m_renderTarget->Release();
	if (m_brush) m_brush->Release();
    DestroyWindow(m_hwnd);
}


void Screen::Paint() {
	m_renderTarget->BeginDraw();
	m_renderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
	m_renderTarget->EndDraw();
}

bool Screen::Init() {

	HRESULT res = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_factory);
	if (res != S_OK) return false;

	RECT rect;
	GetClientRect(m_hwnd, &rect);

	res = m_factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)),
		D2D1::HwndRenderTargetProperties(m_hwnd, D2D1::SizeU(rect.right, rect.bottom)),
		&m_renderTarget);
	if (res != S_OK) return false;

	ID2D1SolidColorBrush* m_brush;
	m_renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(.5f, .5f, .5f, .5f)), &m_brush);

	return true;
}

void Screen::setOpacity(int opacity) {
	byte alpha = 255*(float)opacity/100;
	SetLayeredWindowAttributes(m_hwnd, 0, alpha, LWA_ALPHA);
}
