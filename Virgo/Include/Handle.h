#pragma once

#include <windows.h>
#include <utility>
#include <type_traits>

#ifdef _DEBUG
#include <sstream>
#endif

#include "Win32Exception.h"

namespace Virgo 
{
	/**
	 * Handle template class for abstracting handle types.
	 */
	template <class THandleImpl>
	class Handle
	{
	public:
		/**
		 * Gets the template argument out of unnecessary sight.
		 */
		using HandleType = Handle<THandleImpl>;

		/*
		 * Initialize from existing HANDLE.
		 */
		explicit Handle(HANDLE& input)
			: handle_(input)
		{
		}

		/**
		 * Copy constructor.
		 */
		explicit Handle(const HandleType& other)
			: handle_(INVALID_HANDLE_VALUE)
		{
			// HELPREQUESTED: Maybe get process handle from the handle and pass it onto 
			// DuplicateHandle function for cross-process handle duplication.

			HANDLE& selfHandle{ static_cast<THandleImpl*>(this)->handle_ };
			HANDLE& otherHandle{ static_cast<THandleImpl*>(other)->handle_ };

			DupHandle(other.handle_, handle_);
		}

		/*
		 * Move constructor.
		 */
		explicit Handle(HandleType&& other)
			: handle_(std::move(other.handle_))
		{
			// No need to use DuplicateHandle as we're only transferring ownership to this
			// instance of Handle.

			other.handle_ = INVALID_HANDLE_VALUE;
		}

		/*
		 * Copy assignment.
		 */
		HandleType& operator=(const HandleType& other)
		{
			THandleImpl& self{ static_cast<THandleImpl&>(*this) };


			if (this != &other)
			{
				DupHandle(other.handle_, handle_);
			}

			return *this;
		}

		/*
		 * Move assignment.
		 */
		HandleType& operator=(HandleType&& other_)
		{
			THandleImpl& self{ static_cast<THandleImpl&>(*this) };
			THandleImpl& other{ static_cast<THandleImpl&>(other_) };

			if (this != &other)
			{
				if (self.handle_ != INVALID_HANDLE_VALUE)
				{
					// TODO: Check for result and throw exception accordingly.
					other.Close();
				}

				self.handle_ = other.handle_;

				other.handle_ = INVALID_HANDLE_VALUE;
			}

			return *this;
		}

		/*
		 * Destructor
		 */
		virtual ~Handle()
		{
			try
			{
				Close();
			}
			catch (std::system_error const& ex)
			{
#ifdef _DEBUG
				std::stringstream stream{};
				stream << "Error occurred whilst closing handle - error #" << std::hex << ex.code();
				OutputDebugStringA(static_cast<LPCSTR>(stream.str().c_str()));
#endif
			}
		}

		/**
		 * Closes the underlying handle.
		 */
		void Close()
		{
			static_cast<THandleImpl&>(*this).Close();
		}

		/*
		 * Conversion operator for getting handle value.
		 */
		operator HANDLE&() const
		{
			return static_cast<THandleImpl const&>(*this).operator HANDLE&();
		}
	private:
		/**
		 * Duplicates a handle to b handle.
		 */
		static void DupHandle(const HANDLE& a, HANDLE& b)
		{
			DWORD dwFlags{ 0 };
			BOOL getHandleInformationResult{ ::GetHandleInformation(a, &dwFlags) };
			BOOL duplicateHandleResult{ ::DuplicateHandle(
				::GetCurrentProcess(), a,
				::GetCurrentProcess(), &b, 0,
				dwFlags & HANDLE_FLAG_INHERIT, DUPLICATE_SAME_ACCESS)
			};

			// TODO: Throw exception depending on GetLastError().
			if (!duplicateHandleResult)
			{
				throw NewException(::GetLastError());
			}
		}

		HANDLE handle_;
	};
}