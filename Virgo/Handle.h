#pragma once

#include <windows.h>

namespace Virgo 
{
	template <class THandleImpl>
	class Handle
	{
	public:
		static constexpr const Handle<THandleImpl> INVALID{ INVALID_HANDLE_VALUE };

		explicit Handle(HANDLE& input)
			: handle_(input)
		{
		}

		explicit Handle(const Handle<THandleImpl>& other)
			: handle_(INVALID_HANDLE_VALUE)
		{
			// NOTE: Maybe get process handle from the handle and pass it onto DuplicateHandle
			// function for cross-process handle duplication.

			DWORD dwFlags{ 0 };
			BOOL getHandleInformationResult{ ::GetHandleInformation(other.handle_, &dwFlags) };
			BOOL duplicateHandleResult{ ::DuplicateHandle(
				::GetCurrentProcess(), other.handle_, 
				::GetCurrentProcess(), &handle_, 0, 
				dwFlags & HANDLE_FLAG_INHERIT, DUPLICATE_SAME_ACCESS) 
			};

			// TODO: Throw exception depending on GetLastError().
		}

		virtual ~Handle()
		{
			// Derived class destructor will be called.
		}
	private:
		HANDLE& handle_;
	};
}