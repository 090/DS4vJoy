#include "stdafx.h"
#include "resource.h"
#include "KeymapDataDlg.h"


KeymapDataDlg::KeymapDataDlg()
{
}


KeymapDataDlg::~KeymapDataDlg()
{
}

BOOL KeymapDataDlg::Open(HWND hWnd)
{
	HINSTANCE h = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	m_hWnd = hWnd;
	switch (DialogBoxParam(h, L"IDD_INPUTKEY", hWnd, (DLGPROC)KeymapDataDlg::Proc, (LPARAM)this))
	{
	case IDOK: {
		return TRUE;
	}
	}
	return FALSE;
}

INT_PTR KeymapDataDlg::Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	KeymapDataDlg *dlg;
	if (uMsg == WM_INITDIALOG) {
		SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);
		dlg = (KeymapDataDlg *)lParam;
	}
	else {
		dlg = (KeymapDataDlg *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}
	if (dlg) {
		return dlg->_proc(hWnd, uMsg, wParam, lParam);
	}
	else {
		return NULL;
	}
}
INT_PTR KeymapDataDlg::_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		m_hDlg = hWnd;
		initdialog();
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			keymapData.Enable = true;
			return EndDialog(hWnd, IDOK);
		case IDCANCEL:
			return EndDialog(hWnd, IDCANCEL);
		case IDC_KEYMAP_BTN:
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				keymapData.ButtonID = (vJoyButtonID)SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
			}
			break;
		case IDC_INPUT:
			switch (HIWORD(wParam))
			{
			case EN_CHANGE: {
				keymapData.GetState();
				SetWindowText((HWND)lParam, keymapData.StrVal());
				break;
			}
			default:
				return FALSE;
			}
		}

	default:
		return FALSE;
	}
	return TRUE;
}
INT_PTR KeymapDataDlg::InputProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	KeymapDataDlg *dlg = (KeymapDataDlg *)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (dlg)
		return dlg->_inputProc(hWnd, uMsg, wParam, lParam);
	return 0;
}
INT_PTR KeymapDataDlg::_inputProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_GETDLGCODE:
		return DLGC_WANTALLKEYS;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
	{
		keymapData.GetState();
		SetWindowText(hWnd, keymapData.StrVal());
		return TRUE;
	}
	break;
	}
	return CallWindowProc(m_defaultInputProc, hWnd, uMsg, wParam, lParam);
}

void KeymapDataDlg::initdialog()
{
	HWND hInput = GetDlgItem(m_hDlg, IDC_INPUT);
	SetWindowLongPtr(hInput,GWLP_USERDATA, (LONG_PTR)this);
	m_defaultInputProc = (WNDPROC)SetWindowLongPtr(hInput, GWLP_WNDPROC, (LONG_PTR)InputProc);
	SetFocus(hInput);
	SetWindowText(hInput, keymapData.StrVal());

	HWND hBtn = GetDlgItem(m_hDlg, IDC_KEYMAP_BTN);
	for (int i = 0; i < vJoyButtonID::button_Count; i++) {
		WCHAR *str = vJoyButton::String((vJoyButtonID)i);
		SendMessage(hBtn, CB_ADDSTRING, 0, (LPARAM)str);
	}
	SendMessage(hBtn, CB_SETCURSEL, (WPARAM)keymapData.ButtonID, 0);

}
