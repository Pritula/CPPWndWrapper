//////////////////////////////////////////////////////////////////////////
//
//	Based on original code Thunk32 from StackOverflow.com
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include <wtypes.h>
#include <crtdbg.h>

#include <exception>
#include <new>

namespace indev
{
	class Thunk32Base
	{
	protected:
		Thunk32Base();
		~Thunk32Base();

		struct Bytecode
		{
#pragma pack(push, 1)
			unsigned short stub1;      // lea ecx, 
			unsigned long  nThisPtr;   // this
			unsigned char  stub2;      // mov eax,
			unsigned long  nJumpProc;  // pointer to destination function
			unsigned short stub3;      // jmp eax
#pragma pack(pop)
			Bytecode() :
			stub1	  (0x0D8D),
			nThisPtr  (0),
			stub2	  (0xB8),
			nJumpProc (0),
			stub3	  (0xE0FF)
			{}
		} *bytecode;

		static void* volatile bytecodeHeap;
		static HANDLE hMutex;

	private:
		// Thunks will not be copyable
		Thunk32Base(const Thunk32Base&); // not implemented
		const Thunk32Base& operator=(const Thunk32Base&); // not implemented

		static void __cdecl cleanupHeap();
	};

	template<class T, typename R, typename P0, typename P1, typename P2, typename P3>
	class Thunk32_4 : Thunk32Base
	{
	public:
		typedef R (__stdcall *callback_type)(P0, P1, P2, P3);
		typedef R (__thiscall T::*memfun_type)(P0, P1, P2, P3);

		void initialize(T* pInstance, memfun_type memberFunction)
		{
			_ASSERT(sizeof(pInstance) == sizeof(ULONG));

#pragma warning(push)
#pragma warning(suppress: 4311)
			bytecode->nThisPtr = reinterpret_cast<ULONG>(pInstance);
			bytecode->nJumpProc = *reinterpret_cast<ULONG*>(&memberFunction);
#pragma warning(pop)

			FlushInstructionCache(GetCurrentProcess(), bytecode, sizeof(Bytecode));
		}

		callback_type getCallback() const
		{
			_ASSERT(bytecode->nThisPtr != 0);
			_ASSERT(bytecode->nJumpProc != 0);
			return reinterpret_cast<callback_type>(bytecode);
		}
	};

	template<class T, typename R>
	class Thunk32_0 : Thunk32Base
	{
	public:
		typedef R (__stdcall *callback_type)(void);
		typedef R (__thiscall T::*memfun_type)(void);

		void initialize(T* pInstance, memfun_type memberFunction)
		{
			_ASSERT(sizeof(pInstance) == sizeof(ULONG));

#pragma warning(push)
#pragma warning(suppress: 4311)
			bytecode->nThisPtr = reinterpret_cast<ULONG>(pInstance);
			bytecode->nJumpProc = *reinterpret_cast<ULONG*>(&memberFunction);
#pragma warning(pop)

			FlushInstructionCache(GetCurrentProcess(), bytecode, sizeof(Bytecode));
		}

		callback_type getCallback() const
		{
			_ASSERT(bytecode->nThisPtr != 0);
			_ASSERT(bytecode->nJumpProc != 0);
			return reinterpret_cast<callback_type>(bytecode);
		}
	};

}
