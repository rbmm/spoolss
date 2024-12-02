#include "stdafx.h"

_NT_BEGIN
#include "MiniWnd.h"

LRESULT CSimpWnd::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_NCDESTROY:
		_bittestandreset(&_dwCallCount, 31);
		break;
	}

	return DefWinProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CSimpWnd::WrapperWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	_dwCallCount++;
	lParam = WindowProc(hwnd, uMsg, wParam, lParam);
	if (!--_dwCallCount)
	{
		AfterLastMessage();
		Release();
	}
	return lParam;
}

LRESULT WINAPI CSimpWnd::_WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return reinterpret_cast<CSimpWnd*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA))->WrapperWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT WINAPI CSimpWnd::StartWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		CSimpWnd* This = reinterpret_cast<CSimpWnd*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)This);
		SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)_WindowProc);
		This->AddRef();
		This->_dwCallCount = 1 << 31;
		return This->WrapperWindowProc(hwnd, uMsg, wParam, lParam);
	}

	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

static const WCHAR cls_name[] = L"93C8AF95BC114ed5BE9F09CC5626CCF3";

HWND CSimpWnd::Create(
					  DWORD dwExStyle,
					  PCWSTR lpWindowName,
					  DWORD dwStyle,
					  int x,
					  int y,
					  int nWidth,
					  int nHeight,
					  HWND hWndParent,
					  HMENU hMenu
					  )
{
	return CreateWindowExW(dwExStyle, cls_name, lpWindowName, dwStyle, x, y, 
		nWidth, nHeight, hWndParent, hMenu, (HINSTANCE)&__ImageBase, this);
}

ULONG CSimpWnd::Register()
{
	WNDCLASSW cls = {
		0, StartWindowProc, 0, 0, (HINSTANCE)&__ImageBase, 0, 0, 0, 0, cls_name
	};

	if (s_atom = RegisterClassW(&cls))
	{
		return NOERROR;
	}

	return GetLastError();
}

void CSimpWnd::Unregister()
{
	if (s_atom && !UnregisterClassW(cls_name, (HINSTANCE)&__ImageBase)) __debugbreak();
}

//////////////////////////////////////////////////////////////////////////

INT_PTR CALLBACK YDlg::_S_DlgProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
	YDlg* dlg = reinterpret_cast<YDlg*>(GetWindowLongPtrW(hwnd, DWLP_USER));

	dlg->_dwCallCount++;

	lParam = dlg->DlgProc(hwnd, umsg, wParam, lParam);

	if (!--dlg->_dwCallCount)
	{
		dlg->AfterLastMessage();
		dlg->Release();
	}

	return lParam;
}

INT_PTR CALLBACK YDlg::DlgProcStart(HWND hwnd, UINT umsg, WPARAM /*wParam*/, LPARAM lParam)
{
	if (WM_INITDIALOG == umsg)
	{
		SetWindowLongPtrW(hwnd, DWLP_USER, lParam);
		SetWindowLongPtrW(hwnd, DWLP_DLGPROC, (LPARAM)_S_DlgProc);
		reinterpret_cast<YDlg*>(lParam)->AddRef();
		reinterpret_cast<YDlg*>(lParam)->_dwCallCount = 1 << 31;
		return reinterpret_cast<YDlg*>(lParam)->OnInitDialog(hwnd);
	}

	return 0;
}

INT_PTR YDlg::DlgProc(HWND /*hwnd*/, UINT umsg, WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	switch (umsg)
	{
	case WM_NCDESTROY:
		OnDestroy();
		break;
	}

	return 0;
}

_NT_END