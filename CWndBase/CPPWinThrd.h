//////////////////////////////////////////////////////////////////////////
//
//	WinAPI Threads CPP wrapper
//	(c) pritula@gmail.com
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include <Windows.h>
#include "thunk32.h"

class CPPWinThread
{
	typedef void (*EXCEPTION_PROC) (int, _EXCEPTION_POINTERS*);

	HANDLE	handle;
	DWORD	id;
	DWORD	exitCode;

	DWORD exCode;
	_EXCEPTION_POINTERS *expLocal;
	EXCEPTION_PROC thrdExProc;

	LPTHREAD_START_ROUTINE thrdProc; // DWORD (__stdcall *LPTHREAD_START_ROUTINE) (LPVOID)
	LPVOID thrdParam;

	indev::Thunk32_0 <CPPWinThread, DWORD> thrdCallBack;

	DWORD exceptionHandler (unsigned int code, struct _EXCEPTION_POINTERS *ep) {
		exCode = code;
		expLocal = ep;

		return EXCEPTION_EXECUTE_HANDLER;
	}

	DWORD threadFn (void) {
		__try {

			if (thrdProc)
				exitCode = thrdProc (thrdParam);
			else
				threadProc (thrdParam);

		} __except (exceptionHandler (GetExceptionCode (), GetExceptionInformation ())) {

			if (thrdExProc)
				thrdExProc (exCode, expLocal);
			else
				exceptionProc (exCode, expLocal);

			return 0L;
		}

		return 1L;
	};

public:

	CPPWinThread (LPTHREAD_START_ROUTINE tProc = NULL, LPVOID tParam = NULL, EXCEPTION_PROC exProc = NULL) :
			thrdProc (tProc), thrdParam (tParam), thrdExProc (exProc), exitCode (0)
	{
		thrdCallBack.initialize (this, &CPPWinThread::threadFn);
		handle = ::CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE) thrdCallBack.getCallback (),
				thrdParam, CREATE_SUSPENDED, &id);

		if (thrdProc) {
		  run ();
		}
	}

	HANDLE	getHandle (void) {
		return handle;
	}

	DWORD	getId (void) {
		return id;
	}

	LPTHREAD_START_ROUTINE setProc (LPTHREAD_START_ROUTINE tProc  = NULL, LPVOID tParam = NULL) {
		LPTHREAD_START_ROUTINE current = thrdProc;

		if (tProc)
			thrdProc = tProc;

		if (tParam)
			thrdParam = tParam;

		return current;
	}

	void setExProc (EXCEPTION_PROC exProc  = NULL) {
		if (exProc)
			thrdExProc = exProc;
	}

	void run (void) {
		::ResumeThread (handle);
	}

	void suspend (void) {
		::SuspendThread (handle);
	}

	void kill (void) {
		::TerminateThread (handle, 0L);
	}

	DWORD wait (DWORD period = INFINITE) {
		return ::WaitForSingleObject (handle, period);
	}

	DWORD getExitCode (void) {
		return exitCode;
	}

	virtual DWORD threadProc (LPVOID) {
		return 0L;
	}

	virtual void exceptionProc (unsigned int code, struct _EXCEPTION_POINTERS *ep) {
		wchar_t *wstrExMsg = new wchar_t [256];
		::wsprintf (wstrExMsg, L"Thread caught an exception with Code: %u (%X hex)", code, code);
		::MessageBox (NULL, wstrExMsg, L"Exception", MB_ICONERROR | MB_OK);
		delete [] wstrExMsg;
	}
};
