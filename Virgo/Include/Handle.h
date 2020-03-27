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

		/*
		 * Copy constructor.
		 */
		explicit Handle(const HandleType& other)
			: handle_(INVALID_HANDLE_VALUE)
		{
			// HELPREQUESTED: Maybe get process handle from the handle and pass it onto 
			// DuplicateHandle function for cross-process handle duplication.

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
		HandleType& operator=(const HandleType& other);

		/*
		 * Move assignment.
		 */
		HandleType& operator=(HandleType&& other);

		/*
		 * Destructor
		 */
		virtual ~Handle();

		/**
		 * Closes the underlying handle.
		 */
		void Close();

		/**
		 * Conversion operator for getting handle value.
		 */
		operator HANDLE&() const;

		/**
		 * Get underlying handle.
		 */
		HANDLE& GetHandle();

		/**
		 * Get underlying handle (const)
		 */
		const HANDLE& GetHandle() const;
	private:
		/**
		 * Duplicates a handle to b handle.
		 */
		static void DupHandle(const HANDLE& a, HANDLE& b);

		HANDLE handle_;
	};
}