//////////////////////////////////////////////////////////////////////////
//
//	Application Window creation WinAPI CPP wrapper
//	(c) pritula@gmail.com
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include <vector>

#include <typeinfo.h>

#include "thunk32.h"
#include "CPPWinThrd.h"

class CWndBase {
public:
	union {
		LPVOID		_pVoid;
		LONG		_lProc;
		LONG_PTR	_lptrProc;
		WNDPROC		_wndProc;
	} _equTypeHelper;

	typedef LRESULT (CWndBase::*MSG_HANDLER_PROC) (HWND, UINT, WPARAM, LPARAM);

	#define MSGHANDLERPROC(x) virtual LRESULT x (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	#define LPMSGHANDLERPROC(x) (CWndBase::MSG_HANDLER_PROC) &x
	#define SETMSGHANDLERPROC(x,y) addMsgHandler (x, LPMSGHANDLERPROC (y))

private:
	typedef indev::Thunk32_4<CWndBase, LRESULT, HWND, UINT, WPARAM, LPARAM> msgHandle_t;

	WNDCLASS m_wndClass;
	ATOM m_classAtom;
	HINSTANCE m_hInst;
	HWND m_hWnd;
	WNDPROC m_OrigWndProc;
	bool subclassed;

	indev::Thunk32_4<CWndBase, LRESULT, HWND, UINT, WPARAM, LPARAM> classProc_t;
	indev::Thunk32_4<CWndBase, LRESULT, HWND, UINT, WPARAM, LPARAM> wndProc_t;
	std::vector <std::pair <UINT, msgHandle_t*>> msgHandlers;

	LRESULT classProc (HWND, UINT, WPARAM, LPARAM);
	LRESULT wndProc (HWND, UINT, WPARAM, LPARAM);

public:
	CWndBase ();
	~CWndBase ();

	virtual bool preRegisterClass (LPWNDCLASS wndClass = NULL);
	virtual bool preCreate (LPCREATESTRUCT pCS = NULL);

	bool registerClass (void);
	bool create (wchar_t *wndTitle = NULL, HWND hParent = NULL);
	bool create (LPCREATESTRUCT pcs);
	bool create (
		DWORD dwExStyle = 0,
		LPCTSTR lpClassName = NULL,
		LPCTSTR lpWindowName = NULL,
		DWORD dwStyle = NULL,
		int x = CW_USEDEFAULT, int y = CW_USEDEFAULT,
		int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT,
		HWND hWndParent = NULL,
		HMENU hMenu = NULL,
		HINSTANCE hInstance = NULL,
		LPVOID lpCreateParams = NULL
		);

	bool setSubclass (void);
	bool resetSubclass (void);

	bool isSubclassed (void);
	WNDPROC getWinProc (void);

	void addMsgHandler (UINT, MSG_HANDLER_PROC);
	void removeMsgHandler (UINT);

	HWND getHWnd (void);
	HINSTANCE getHInst (void);

};

class CAppWnd : public CWndBase, CPPWinThread {

	DWORD threadProc (LPVOID) {

		registerClass ();

		if (!create (L"CAppWnd"))
			return 0L;

		DWORD dwCode = 0;
		MSG msg;
		while (::GetMessage (&msg, NULL, 0, 0)) {
			::TranslateMessage (&msg);
			::DispatchMessage (&msg);
		}

		return dwCode;
	}

public:

	DWORD run (DWORD peroid = INFINITE) {
		CPPWinThread::run ();
		return CPPWinThread::wait (peroid);
	}

};
