//////////////////////////////////////////////////////////////////////////
//
//	Application Window creation WinAPI CPP wrapper
//	(c) pritula@gmail.com
//
//////////////////////////////////////////////////////////////////////////

#include <Windows.h>
#include "CWndBase.h"

CWndBase::CWndBase () : subclassed (false) {
	classProc_t.initialize (this, &CWndBase::classProc);
	wndProc_t.initialize (this, &CWndBase::wndProc);
}

CWndBase::~CWndBase () {
	std::vector <std::pair <UINT, msgHandle_t*>>::iterator it = msgHandlers.begin ();
	for (it; it < msgHandlers.end (); it++) {
		if ((*it).second) {
			delete (msgHandle_t*) (*it).second;
			(*it).second = NULL;
		}
	}
}

LRESULT CWndBase::wndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	std::vector <std::pair <UINT, msgHandle_t*>>::iterator it = msgHandlers.begin ();
	for (it; it < msgHandlers.end (); it++) {
		if (uMsg == (*it).first) {
			msgHandle_t* msgProcPtr = (*it).second;
			return (msgProcPtr->getCallback ()) (hWnd, uMsg, wParam, lParam);
		}
	}

	return ::CallWindowProc (m_OrigWndProc, hWnd, uMsg, wParam, lParam);
} 

LRESULT CWndBase::classProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	std::vector <std::pair <UINT, msgHandle_t*>>::iterator it = msgHandlers.begin ();
	for (it; it < msgHandlers.end (); it++) {
		if (uMsg == (*it).first) {
			msgHandle_t* msgProcPtr = (*it).second;
			return (msgProcPtr->getCallback ()) (hWnd, uMsg, wParam, lParam);
		}
	}

	return ::DefWindowProc (hWnd, uMsg, wParam, lParam);
} 

bool CWndBase::setSubclass (void) {
	if (subclassed)
		return false;

	_equTypeHelper._wndProc = wndProc_t.getCallback ();
	_equTypeHelper._lptrProc = ::SetWindowLongPtr (m_hWnd, GWL_WNDPROC, _equTypeHelper._lProc);
	m_OrigWndProc = _equTypeHelper._wndProc;

	if (m_OrigWndProc)
		return (subclassed = true);
	else
		return false;
}

bool CWndBase::resetSubclass (void) {
	if (!m_OrigWndProc)
		return false;

	_equTypeHelper._wndProc = m_OrigWndProc;
	if (::SetWindowLongPtr (m_hWnd, GWL_WNDPROC, _equTypeHelper._lProc)) {
		subclassed = false;
		return true;
	} else
		return false;
}

HWND CWndBase::getHWnd (void) {
	return m_hWnd;
}

HINSTANCE CWndBase::getHInst (void) {
	return m_hInst;
}

bool CWndBase::isSubclassed (void) {
	return subclassed;
}

// WNDPROC CWndBase::getWinProc (void) {
// 	if (subclassed)
// 		return m_OrigWndProc;
// 
// 	return wndProc_t.getCallback ();
// }

bool CWndBase::preRegisterClass (LPWNDCLASS wndClass) {
	return true;
}

bool CWndBase::preCreate (LPCREATESTRUCT pCS) {
	return true;
}

bool CWndBase::registerClass (void) {
	LPWNDCLASS wndClass = new WNDCLASS;

	if (!wndClass)
		return false;

	wndClass->style = CS_HREDRAW | CS_VREDRAW;
	wndClass->lpfnWndProc = (WNDPROC) classProc_t.getCallback ();
	wndClass->cbClsExtra = 0;
	wndClass->cbWndExtra = 0;
	wndClass->hInstance = (HINSTANCE) GetModuleHandle (NULL);
	wndClass->hIcon = NULL;
	wndClass->hCursor = ::LoadCursor (NULL, IDC_ARROW);
	wndClass->hbrBackground = (HBRUSH) COLOR_BTNSHADOW;
	wndClass->lpszMenuName = NULL;
	wndClass->lpszClassName = L"CWndBase Window";

	if (preRegisterClass (wndClass)) { 
		::memcpy (&m_wndClass, wndClass, sizeof (WNDCLASS));
		m_classAtom = ::RegisterClass (&m_wndClass);

// 		if (!m_classAtom && (1410 == ::GetLastError ())) // ERROR_ALREADY_REGISTERED
// 			setSubclass ();
	}

	delete (LPWNDCLASS) wndClass;

	return m_classAtom ? true : false;
}

bool CWndBase::create (wchar_t *wndTitle, HWND hParent) {

	int x = GetSystemMetrics( SM_CXFULLSCREEN ) / 2 - 320;
	int y = GetSystemMetrics( SM_CYFULLSCREEN ) / 2 - 210;

	return create (NULL, m_wndClass.lpszClassName, wndTitle,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			x, y, 640, 420,
			hParent, NULL, m_wndClass.hInstance, NULL);
}

bool CWndBase::create (
		DWORD dwExStyle,
		LPCTSTR lpClassName,
		LPCTSTR lpWindowName,
		DWORD dwStyle,
		int x, int y, int cx, int cy,
		HWND hWndParent,
		HMENU hMenu,
		HINSTANCE hInstance,
		LPVOID lpCreateParams
		) {

	bool created = false;
	LPCREATESTRUCT pcs = new CREATESTRUCT;

	pcs->dwExStyle = dwExStyle;
	pcs->lpszClass = lpClassName;
	pcs->lpszName = lpWindowName;
	pcs->style = dwStyle;
	pcs->x = x; pcs->y = y; pcs->cx = cx; pcs->cy = cy;
	pcs->hwndParent = hWndParent;
	pcs->hMenu = hMenu;
	pcs->hInstance = hInstance;
	pcs->lpCreateParams = lpCreateParams;

	created = create (pcs);

	delete (LPCREATESTRUCT) pcs;

	return created;
}

bool CWndBase::create (LPCREATESTRUCT pcs) {

	if (!pcs)
		return false;

	if (preCreate (pcs))
		m_hWnd = CreateWindowEx (
			pcs->dwExStyle,
			pcs->lpszClass,
			pcs->lpszName,
			pcs->style,
			pcs->x, pcs->y, pcs->cx, pcs->cy,
			pcs->hwndParent,
			pcs->hMenu,
			pcs->hInstance,
			pcs->lpCreateParams
			);

	return m_hWnd ? true : false;
}

void CWndBase::addMsgHandler (UINT uMsg, MSG_HANDLER_PROC proc) {
	msgHandle_t* tmp = new msgHandle_t;
	tmp->initialize (this, proc);

	msgHandlers.push_back (std::make_pair (uMsg, tmp));
}

void CWndBase::removeMsgHandler (UINT  uMsg) {
	std::vector <std::pair <UINT, msgHandle_t*>>::iterator it = msgHandlers.begin ();
	for (it; it < msgHandlers.end (); it++) {
		if (uMsg == (*it).first) {
			delete (msgHandle_t*) (*it).second;
			msgHandlers.erase (it);
			break;
		}
	}
}
