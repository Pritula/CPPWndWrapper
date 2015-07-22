//////////////////////////////////////////////////////////////////////////
//
//	Based on original code Thunk32 from StackOverflow.com
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "thunk32.h"

namespace indev
{
	void* volatile Thunk32Base::bytecodeHeap = NULL;
	HANDLE Thunk32Base::hMutex = NULL;

	Thunk32Base::Thunk32Base()
	{
		// Double checked locking, guaranteed by Acquire/Release-semantics in Microsoft's
		// volatile-implementation.
		if(bytecodeHeap == NULL)
		{
			hMutex = CreateMutex(NULL, TRUE, NULL);
			while (hMutex == (HANDLE)NULL || hMutex == (HANDLE)ERROR_ALREADY_EXISTS || hMutex == (HANDLE)ERROR_ACCESS_DENIED) {
				Sleep(1000);
			}

			if(bytecodeHeap == NULL)
			{
				bytecodeHeap = HeapCreate(HEAP_CREATE_ENABLE_EXECUTE, 0, 0);
				if(bytecodeHeap == NULL)
				{
					throw std::exception("Heap creation failed!");
				}

				// Schedule the heap to be destroyed when the application terminates.
				// Until then, it will manage its own size.
				atexit(cleanupHeap);
			}
		}

		bytecode = reinterpret_cast<Bytecode*>(HeapAlloc(bytecodeHeap, 0, sizeof(Bytecode)));
		if(bytecode == NULL) 
		{
			throw std::exception("Bytecode allocation failed!");
		}
		new (bytecode) Bytecode;
	}

	Thunk32Base::~Thunk32Base()
	{
		if(bytecode)
		{
			bytecode->~Bytecode();
			HeapFree(bytecodeHeap, 0, bytecode);
		}
	}

	void Thunk32Base::cleanupHeap()
	{
		HeapDestroy(Thunk32Base::bytecodeHeap);
	}
};
