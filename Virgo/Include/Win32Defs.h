#pragma once

#include <windows.h>

#include <string>
#include <memory>
#include <new>
#include <limits>

namespace Virgo 
{
	/**
	 * Defines allocator traits for LocalAlloc/LocalFree pair.
	 */
	template <class T>
	class LocalAllocator
	{
	public:
		using value_type = T;

		LocalAllocator() = default;

		template <class U> constexpr LocalAllocator(const LocalAllocator<U>&) noexcept
		{
		}

		T* allocate(std::size_t n)
		{
			if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
				throw std::bad_alloc();

			if (auto p = static_cast<T*>(::LocalAlloc(LPTR, n * sizeof(T))))
				return p;

			throw std::bad_alloc();
		}

		void deallocate(T* p, std::size_t) noexcept
		{
			::LocalFree(p);
		}
	};
	
	template <class T, class U>
	bool operator==(const LocalAllocator<T>&, const LocalAllocator<U>&)
	{
		return true;
	}

	template <class T, class U>
	bool operator!=(const LocalAllocator<T>&, const LocalAllocator<U>&)
	{
		return false;
	}

	/**
	 * Defines string class usable with Win32 APIs.
	 */
	using TCharString = std::basic_string<TCHAR>;
	using WCharString = std::basic_string<WCHAR>;
	using CCharString = std::basic_string<CHAR>;
}