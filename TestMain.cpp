//////////////////////////////////////////////////////////////////////////
//
//	Application Window creation WinAPI CPP wrapper
//	(c) pritula@gmail.com
//
//	Simple test to show usage of CWndBase and CPPWinThrd
//////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include <CommCtrl.h>
#include <Richedit.h>

#pragma comment (lib, "comctl32.lib")

#include "CWndBase/CWndBase.h"

class CEditWnd : public CWndBase {
public:
	CEditWnd () {
		InitCommonControls ();

		SETMSGHANDLERPROC (WM_CHAR, CEditWnd::onTestMsg);
	}

	~CEditWnd () {
		removeMsgHandler (WM_CHAR);
	}

	bool preRegisterClass () {
		return false;
	}

	bool preCreate (LPCREATESTRUCT pCS) {
		pCS->style = WS_POPUP | WS_SYSMENU | WS_CAPTION | WS_VISIBLE | WS_THICKFRAME | ES_MULTILINE;
		pCS->lpszClass = WC_EDIT;

		return true;
	}

	MSGHANDLERPROC (onTestMsg) {
		::SetWindowText (hWnd, L"onTestMessage.");
		return 1L;
	}
};

class CMyWnd : public CAppWnd {
public:
	CMyWnd () {
		SETMSGHANDLERPROC (WM_CREATE, CMyWnd::onCreate);
		SETMSGHANDLERPROC (WM_DESTROY, CMyWnd::onDestroy);
		SETMSGHANDLERPROC (WM_ERASEBKGND, CMyWnd::onEraseBkgnd);
		SETMSGHANDLERPROC (WM_PAINT, CMyWnd::onPaint);

		SETMSGHANDLERPROC (WM_CTLCOLOREDIT, CMyWnd::onEditCtl);
	}

// 	MSGHANDLERPROC (onCreate);
// 	MSGHANDLERPROC (onDestroy);
// 	MSGHANDLERPROC (onEraseBkgnd);
// 	MSGHANDLERPROC (onPaint);

	bool preCreate (LPCREATESTRUCT pCS) {
		//pCS->style |= WS_CLIPSIBLINGS; // WS_CLIPCHILDREN

		pCS->cx = 800;
		pCS->cy = 600;
		
		return true;
	}

	LRESULT onCreate (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (!edit.create (L"Edit", hWnd)) {
			::MessageBox (NULL, L"Can not create", L"Edit Window", MB_OK);
			return -1L;
		}

		//edit.setSubclass ();
		//::SendMessage (edit.getHWnd (), EM_SETBKGNDCOLOR, 0, RGB (1,0,0));

		pedit = new CEditWnd;
		pedit->create (L"Edit two", hWnd);
		pedit->setSubclass ();

		::MoveWindow (hWnd, 100, 100, 800, 600, TRUE);

		return 0L;
	}

	LRESULT onDestroy (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (pedit)
			delete pedit;

		::PostQuitMessage (0);
		return 0L;
	}

	LRESULT onEraseBkgnd (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

		HDC hDc = (HDC) wParam;
		RECT rc;
		::GetClientRect (hWnd, &rc);

		::FillRect (hDc, &rc, (HBRUSH) ::GetStockObject (BLACK_BRUSH));

		return 0L;
	}

	LRESULT onPaint (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		PAINTSTRUCT ps;
		HDC hDc = ::BeginPaint (hWnd, &ps);

		RECT rc;
		::GetClientRect (hWnd, &rc);

		int cx = rc.right / 2;
		int cy = rc.bottom / 2;

		rc.left = cx / 2;
		rc.top = cy / 2;
		rc.right = rc.right - rc.left;
		rc.bottom = rc.bottom - rc.top;

		::FillRect (hDc, &rc, (HBRUSH) ::GetStockObject (WHITE_BRUSH));

		const char *str = typeid (*this).name ();

		::DrawTextA (hDc, str, (int) ::strlen (str), &rc,
				DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_WORD_ELLIPSIS);

		::EndPaint (hWnd, &ps);
		return 0L;
	}

	LRESULT onEditCtl (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if ((HWND) lParam == edit.getHWnd ()) {
			::SetTextColor ((HDC) wParam, RGB (100,255,100));
			::SetBkColor ((HDC) wParam, RGB (0,0,0));
			return (LRESULT) ::GetStockObject (BLACK_BRUSH);
		}

		return 0L;
	}


public:
	CEditWnd edit;
	CEditWnd *pedit;

};

int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	CMyWnd wnd;
// 	wnd.Create (hInstance);
	wnd.run ();

//	MessageBox (NULL, L"WinMain", L"WinMain", MB_OK);
	return 0;
}
