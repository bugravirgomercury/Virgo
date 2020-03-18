#pragma once

#include <windows.h>
#include <utility>
#include <type_traits>

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

		/**
		 * Specifies invalid handle value of this type of handle.
		 */
		static constexpr const HandleType INVALID{ INVALID_HANDLE_VALUE };

		/*
		 * Initialize from existing HANDLE.
		 */
		explicit Handle(HANDLE& input)
			: handle_(input)
		{
		}

		/*
		 * Copy constructor.
		 */
		explicit Handle(const HandleType& other)
			: handle_(INVALID_HANDLE_VALUE)
		{
			// HELPREQUESTED: Maybe get process handle from the handle and pass it onto 
			// DuplicateHandle function for cross-process handle duplication.

			// HELPREQUESTED: How to apply Don't-Repeat-Yourself on this code block?
			DWORD dwFlags{ 0 };
			BOOL getHandleInformationResult{ ::GetHandleInformation(other.handle_, &dwFlags) };
			BOOL duplicateHandleResult{ ::DuplicateHandle(
				::GetCurrentProcess(), other.handle_, 
				::GetCurrentProcess(), &handle_, 0, 
				dwFlags & HANDLE_FLAG_INHERIT, DUPLICATE_SAME_ACCESS) 
			};

			// TODO: Throw exception depending on GetLastError().
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
			if (this != &other)
			{
				DWORD dwFlags{ 0 };
				BOOL getHandleInformationResult{ ::GetHandleInformation(other.handle_, &dwFlags) };
				BOOL duplicateHandleResult{ ::DuplicateHandle(
					::GetCurrentProcess(), other.handle_,
					::GetCurrentProcess(), &handle_, 0,
					dwFlags & HANDLE_FLAG_INHERIT, DUPLICATE_SAME_ACCESS)
				};

				// TODO: Throw exception depending on GetLastError().
			}

			return *this;
		}

		/*
		 * Move assignment.
		 */
		HandleType& operator=(HandleType&& other)
		{
			if (this != &other)
			{
				if (handle_ != INVALID_HANDLE_VALUE)
				{
					// TODO: Check for result and throw exception accordingly.
					other.Close();
				}

				handle_ = other.handle_;

				other.handle_ = INVALID_HANDLE_VALUE;
			}

			return *this;
		}

		/*
		 * Destructor
		 */
		virtual ~Handle()
		{
			Close();
		}

		/**
		 * Closes the underlying handle.
		 */
		void Close()
		{
			static_cast<THandleImpl*>(this)->Close();
		}

		/*
		 * Conversion operator for getting handle value.
		 */
		operator HANDLE&() const
		{
			return handle_;
		}
	private:
		HANDLE& handle_;
	};
}