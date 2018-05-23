#pragma once
// Code injecting utils
// Author(s):       iFarbod <ifarbod@outlook.com>
//                  LINK/2012 <dma2012@hotmail.com>
//
// Copyright (c) 2015-2017 CTNorth Team
//
// Distributed under the MIT license (See accompanying file LICENSE or copy at
// https://opensource.org/licenses/MIT)

#pragma once

#define OFFSETOF(struc, member) \
    (reinterpret_cast<size_t>(&reinterpret_cast<const volatile char&>(((static_cast<struc*>(nullptr))->member))))
//#define VALIDATE_OFFSET(struc, member, offset) static_assert(OFFSETOF(struc, member) == offset, "The offset of " #struc "::" #member " is not " #offset)

#ifndef CTN_NO_FIXEDWIDTH_TYPES

// Types
using u8 = unsigned char;       // A single byte: 0-255.
using u16 = unsigned short;     // 2 bytes: 0 - 65535.
using u32 = unsigned int;       // 4 bytes: 0 - 4,294,967,295 ~ 4000 million or 4e9.
using u64 = unsigned long long; // 8 bytes: 18,446,744,073,709,551,615 ~1.8e19.

using s8 = signed char;         // A single byte: -128 - 127.
using s16 = signed short;       // 2 bytes: -32768 - 32767.
using s32 = int;                // 4 bytes signed: max 2,147,483,647 ~ 2000 million or 2e9.
using s64 = signed long long;   // 8 bytes signed. 9,223,372,036,854,775,807 ~ 9e18.

using f32 = float;              // Single-precision floating point number.
using f64 = double;             // Double-precision floating point number.

#endif


#include <Windows.h>
#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
#include <forward_list>

namespace Hook
{

	union MemoryPointer
	{
	public:
		// Default constructor.
		MemoryPointer() : m_ptr(nullptr) {}
		// Construct from nullptr.
		MemoryPointer(std::nullptr_t) : m_ptr(nullptr) {}
		// Copy constructor.
		MemoryPointer(const MemoryPointer& x) = default;
		// Construct from a pointer.
		MemoryPointer(void* x) : m_ptr(x) {}
		// Construct from an integral pointer.
		MemoryPointer(u32 x) : m_a(x) {}
		// Construct from a pointer with a specified type.
		template <class T> MemoryPointer(T* x) : m_ptr(reinterpret_cast<void*>(x)) {}

		// Returns true if the underlying pointer is a nullptr.
		bool IsNull() const { return this->m_ptr != nullptr; }
		// Return the underlying pointer as a u32.
		u32 AsInt() const { return this->m_a; }

		explicit operator bool() const { return IsNull(); }
		explicit operator u32() const { return this->m_a; }

		MemoryPointer Get() const { return *this; }
		template <class T> T* Get() const { return Get(); }
		template <class T> T* GetRaw() { return Get(); }

		template <class T> operator T*() const { return reinterpret_cast<T*>(m_ptr); }

		// Comparison
		bool operator==(const MemoryPointer& rhs) const { return this->m_a == rhs.m_a; }
		bool operator!=(const MemoryPointer& rhs) const { return this->m_a != rhs.m_a; }
		bool operator<(const MemoryPointer& rhs) const { return this->m_a < rhs.m_a; }
		bool operator<=(const MemoryPointer& rhs) const { return this->m_a <= rhs.m_a; }
		bool operator>(const MemoryPointer& rhs) const { return this->m_a > rhs.m_a; }
		bool operator>=(const MemoryPointer& rhs) const { return this->m_a >= rhs.m_a; }

		// Operators
		MemoryPointer operator+(const MemoryPointer& rhs) const { return MemoryPointer(this->m_a + rhs.m_a); }
		MemoryPointer operator-(const MemoryPointer& rhs) const { return MemoryPointer(this->m_a - rhs.m_a); }
		MemoryPointer operator*(const MemoryPointer& rhs) const { return MemoryPointer(this->m_a * rhs.m_a); }
		MemoryPointer operator/(const MemoryPointer& rhs) const { return MemoryPointer(this->m_a / rhs.m_a); }

		MemoryPointer operator+=(const MemoryPointer& rhs) const { return MemoryPointer(this->m_a + rhs.m_a); }
		MemoryPointer operator-=(const MemoryPointer& rhs) const { return MemoryPointer(this->m_a - rhs.m_a); }
		MemoryPointer operator*=(const MemoryPointer& rhs) const { return MemoryPointer(this->m_a * rhs.m_a); }
		MemoryPointer operator/=(const MemoryPointer& rhs) const { return MemoryPointer(this->m_a / rhs.m_a); }

	private:
		// Pointer.
		void* m_ptr;
		// Unsigned int32.
		u32 m_a;
	};

	template <u32 addr>
	struct LazyPointer
	{
	public:
		// Returns the final raw pointer
		static MemoryPointer Get() { return xGet().Get(); }

		template <class T> static T* Get() { return Get().Get<T>(); }

	private:
		// Returns the final pointer
		static MemoryPointer xGet()
		{
			static void* ptr = nullptr;
			if (!ptr)
				ptr = MemoryPointer(addr).Get();
			return MemoryPointer(ptr);
		}
	};

	template <u32 addr> inline MemoryPointer LazyPtr() { return LazyPointer<addr>::Get(); }

	// Memory protection

	inline bool ProtectMemory(MemoryPointer addr, size_t size, DWORD protection)
	{
		return !!VirtualProtect(addr.Get(), size, protection, &protection);
	}

	inline bool UnprotectMemory(MemoryPointer addr, size_t size, DWORD& out_oldprotect)
	{
		return !!VirtualProtect(addr.Get(), size, PAGE_EXECUTE_READWRITE, &out_oldprotect);
	}

	class ScopedUnprotect
	{
	public:
		ScopedUnprotect(MemoryPointer addr, size_t size)
		{
			if (size == 0)
				m_unprotected = false;
			else
				m_unprotected = UnprotectMemory(m_addr = addr.Get<void>(), m_size = size, m_oldProtect);
		}

		~ScopedUnprotect()
		{
			if (m_unprotected) ProtectMemory(m_addr.Get(), m_size, m_oldProtect);
		}

	private:
		MemoryPointer m_addr;
		size_t m_size;
		DWORD m_oldProtect;
		bool m_unprotected;
	};

	// Methods for reading/writing memory

	// Gets contents from a memory address
	template <class T>
	inline T MemRead(MemoryPointer addr)
	{
		return *addr.Get<T>();
	}

	inline void MemSet(MemoryPointer addr, s32 value, size_t size)
	{
		ScopedUnprotect xprotect(addr, size);
		memset(addr.Get<void>(), value, size);
	}

	inline void MemCpy(MemoryPointer addr, const void* src, size_t size)
	{
		ScopedUnprotect xprotect(addr, size);
		memcpy(addr.Get<void>(), src, size);
	}

	template <class T>
	inline void MemWrite(MemoryPointer addr, T value)
	{
		ScopedUnprotect xprotect(addr, sizeof(T));

		if (MemRead<T>(addr) != value)
		{
			*addr.Get<T>() = value;
		}
	}

	// Searches in the range [|addr|, |addr| + |maxSearch|] for a pointer in the range [|defaultBase|, |defaultEnd|] and replaces
	// it with the proper offset in the pointer |replacementBase|.
	// does memory unprotection if |vp| is true.
	inline MemoryPointer AdjustPointer(MemoryPointer addr, MemoryPointer replacementBase, MemoryPointer defaultBase,
		MemoryPointer defaultEnd, size_t maxSearch = 8)
	{
		ScopedUnprotect xprotect(addr, maxSearch + sizeof(void*));
		for (size_t i = 0; i < maxSearch; ++i)
		{
			MemoryPointer ptr = MemRead<void*>(addr + i);
			if (ptr >= defaultBase.Get() && ptr <= defaultEnd.Get())
			{
				auto result = replacementBase + (ptr - defaultBase.Get());
				MemWrite<void*>(addr + i, result.Get());
				return result;
			}
		}
		return nullptr;
	}

	inline void MemFill(MemoryPointer addr, u8 value, size_t size = 1)
	{
		MemSet(addr, value, size);
	}

	inline void CopyStr(MemoryPointer addr, char const* value)
	{
		ScopedUnprotect xprotect(addr, 1);
		strcpy(addr, value);
	}

	inline void CopyStrEx(MemoryPointer addr, char const* value, size_t count)
	{
		ScopedUnprotect xprotect(addr, count);
		strncpy(addr, value, count);
	}

	inline void ZeroMem(MemoryPointer at, size_t count = 1)
	{
		MemFill(at, 0, count);
	}

	inline void MakeNop(MemoryPointer at, size_t count = 1)
	{
		MemFill(at, 0x90, count);
	}

	inline void MakeRangedNop(MemoryPointer at, MemoryPointer until)
	{
		return MakeNop(at, size_t(until.GetRaw<char>() - at.GetRaw<char>()));
	}

	// C3 RET
	inline void MakeRet(MemoryPointer at)
	{
		MemWrite<u8>(at, 0xC3);
	}

	// C2 RET (2Bytes)
	inline void MakeRet(MemoryPointer at, u16 pop)
	{
		MemWrite<u8>(at, 0xC2);
		MemWrite<u16>(at + 1, pop);
	}

	inline void MakeRETEx(MemoryPointer at, u8 ret = 1)
	{
		MemWrite<u8>(at, 0xB0); // mov al, @ret
		MemWrite<u8>(0x5DF8F0 + 1, ret);
		MakeRet(0x5DF8F0 + 2, 4);
	}

	// for making functions return 0
	inline void MakeRet0(MemoryPointer at)
	{
		MemWrite<u8>(at, 0x33); // xor eax, eax
		MemWrite<u8>(at + 1, 0xC0);
		MakeRet(at + 2);
	}

	inline MemoryPointer GetAbsoluteOffset(size_t relValue, MemoryPointer endOfInstruction)
	{
		return endOfInstruction.Get<char>() + relValue;
	}

	inline int GetRelativeOffset(MemoryPointer absValue, MemoryPointer endOfInstruction)
	{
		return static_cast<u32>(absValue.Get<char>() - endOfInstruction.Get<char>());
	}

	inline MemoryPointer ReadRelativeOffset(MemoryPointer at, size_t sizeofAddr = 4)
	{
		switch (sizeofAddr)
		{
		case 1:
			return (GetAbsoluteOffset(MemRead<s8>(at), at + sizeofAddr));
		case 2:
			return (GetAbsoluteOffset(MemRead<s16>(at), at + sizeofAddr));
		case 4:
			return (GetAbsoluteOffset(MemRead<s32>(at), at + sizeofAddr));
		default:
			return nullptr;
		}
	}

	inline void MakeRelativeOffset(MemoryPointer at, MemoryPointer dest, size_t sizeofAddr = 4)
	{
		switch (sizeofAddr)
		{
		case 1:
			MemWrite<s8>(at, static_cast<s8>(GetRelativeOffset(dest, at + sizeofAddr)));
			break;
		case 2:
			MemWrite<s16>(at, static_cast<s16>(GetRelativeOffset(dest, at + sizeofAddr)));
			break;
		case 4:
			MemWrite<s32>(at, static_cast<s32>(GetRelativeOffset(dest, at + sizeofAddr)));
			break;
		}
	}

	inline MemoryPointer GetBranchDestination(MemoryPointer at)
	{
		switch (MemRead<u8>(at))
		{
			// We need to handle other instructions (and prefixes) later...
		case 0xE8: // call rel
		case 0xE9: // jmp rel
			return ReadRelativeOffset(at + 1, 4);
		case 0xEB:
			return ReadRelativeOffset(at + 1, 1);
		case 0xFF:
		{
			switch (MemRead<u8>(at + 1))
			{
			case 0x15: // call dword ptr [addr]
			case 0x25: // jmp dword ptr [addr]
				return *(MemRead<u32*>(at + 2));
			}
			break;
		}
		}
		return nullptr;
	}

	// Jump Near
	inline MemoryPointer MakeJmp(MemoryPointer at, MemoryPointer dest = nullptr)
	{
		auto p = GetBranchDestination(at);
		MemWrite<u8>(at, 0xE9);

		if (!!dest.AsInt())
		{
			MakeRelativeOffset(at + 1, dest, 4);
		}

		return p;
	}

	inline MemoryPointer MakeCall(MemoryPointer at, MemoryPointer dest = nullptr)
	{
		auto p = GetBranchDestination(at);
		MemWrite<u8>(at, 0xE8);

		if (!!dest.AsInt())
		{
			MakeRelativeOffset(at + 1, dest, 4);
		}

		return p;
	}

	inline MemoryPointer MakeShortJmp(MemoryPointer at, MemoryPointer dest = nullptr)
	{
		auto p = GetBranchDestination(at);
		MemWrite<u8>(at, 0xEB);

		if (!!dest.AsInt())
		{
			MakeRelativeOffset(at + 1, dest, 1);
		}

		return p;
	}

	inline MemoryPointer MakePushOffset(MemoryPointer at, MemoryPointer dest = nullptr)
	{
		auto p = GetBranchDestination(at);
		MemWrite<u8>(at, 0x68);

		if (!!dest.AsInt())
		{
			MakeRelativeOffset(at + 1, dest, 4);
		}

		return p;
	}

	inline void RedirectAllCalls(int startaddr, int endaddr, MemoryPointer with, MemoryPointer dest)
	{
		for (int i = startaddr; i < endaddr; i++)
		{
			if (MemRead<BYTE>(i) == (BYTE)0xE8)
			{
				MemoryPointer at = ReadRelativeOffset(i + 1);
				if (at == with)
				{
					MakeCall(i, dest);
				}
			}
		}
	}


	// TODO: std::forward-less
	template <class Ret = void, class... Args>
	inline Ret Call(MemoryPointer p, Args... a)
	{
		return reinterpret_cast<Ret(__cdecl*)(Args...)>(p.Get<void>())(std::forward<Args>(a)...);
	}

	template <class Ret = void, class... Args>
	inline Ret StdCall(MemoryPointer p, Args... a)
	{
		return reinterpret_cast<Ret(__stdcall*)(Args...)>(p.Get<void>())(std::forward<Args>(a)...);
	}

	template <u32 addr, class Ret = void, class... Args>
	inline Ret Call(Args... a)
	{
		return Call<Ret>(LazyPtr<addr>(), std::forward<Args>(a)...);
	}

	template <u32 addr, class Ret = void, class... Args>
	inline Ret StdCall(Args... a)
	{
		return StdCall<Ret>(LazyPtr<addr>(), std::forward<Args>(a)...);
	}

	template <class Ret = void, class... Args>
	inline Ret ThisCall(MemoryPointer p, Args... a)
	{
		return reinterpret_cast<Ret(__thiscall*)(Args...)>(p.Get<void>())(std::forward<Args>(a)...);
	}

	template <u32 addr, class Ret = void, class... Args>
	inline Ret ThisCall(Args... a)
	{
		return ThisCall<Ret>(LazyPtr<addr>(), std::forward<Args>(a)...);
	}

	template <size_t index>
	struct Vtbl
	{
		template <class Ret, class... Args>
		static Ret Call(Args... a)
		{
			auto obj = MemoryPointer(std::get<0>(std::forward_as_tuple(a...)));
			auto p = MemoryPointer((*obj.template Get<void**>())[i]);
			return ThisCall<Ret>(p, std::forward<Args>(a)...);
		}
	};
}