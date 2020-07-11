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

Screen::Screen(HMONITOR hMonitor, LPRECT lprcMonitor, int opacity) {
    m_hMonitor = hMonitor;
    m_alpha = getAlpha(opacity);
    RECT rect = (*lprcMonitor);
    m_rect = QRect(rect.left,
                   rect.top,
                   rect.right - rect.left,
                   rect.bottom - rect.top);
    qDebug() << "Creating Window on rect " << m_rect;
    CreateDimWindow();
}

void Screen::CreateDimWindow() {
//    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

    m_hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"CoverWindow",
        L"DirectX Tut",
        WS_POPUP,
        m_rect.left(),
        m_rect.top(),
        m_rect.width(),
        m_rect.height(),
        g_hwnd,
        nullptr,
        g_hInst,
        this);

    if (!m_hwnd) {
        ErrorExit(TEXT("CreateWindow"));
    }

    const MARGINS margin = { -1 };
    SetLayeredWindowAttributes(m_hwnd, 0, m_alpha, LWA_ALPHA); // opacity
    DwmExtendFrameIntoClientArea(m_hwnd, &margin);

    Init();

	int nCmdShow = 8;
    ShowWindow(m_hwnd, nCmdShow);
}

void Screen::updateScreen() {
    HWND hwnd = GetForegroundWindow();
    HANDLE activescreenhandle = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
	qDebug() << "Checking active window";
    qDebug() << "m_hMonitor" << m_hMonitor;
    qDebug() << "activescreenhandle" << activescreenhandle;
    m_active = (m_hMonitor == activescreenhandle) ? true : false;
	//CloseHandle(hwnd);
	//CloseHandle(activescreenhandle);
}

byte Screen::getAlpha(int opacity) {
    return 255*(float)opacity/100;
}

Screen::~Screen() {
	if (m_renderTarget)
	{
		m_renderTarget->Release();
		m_renderTarget = nullptr;
		qDebug() << "m_renderTarget released";
	}
	if (m_factory)
	{
		m_factory->Release();
		m_factory = nullptr;
		qDebug() << "factory released";
	}
    DestroyWindow(m_hwnd);
	//CloseHandle(m_hMonitor);
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

	return true;
}

void Screen::setOpacity(int opacity) {
    m_alpha = getAlpha(opacity);
    byte alpha = m_active ? 0 : m_alpha;
    SetLayeredWindowAttributes(m_hwnd, 0, alpha, LWA_ALPHA);
}
