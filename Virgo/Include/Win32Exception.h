#pragma once

#include <windows.h>

namespace Virgo
{
	/**
	 * Defines types used inside exception class.
	 */
	template <class TTargetClass>
	struct Win32ExceptionTraits
	{
		/*
		 * Defines the type of error code this kind of exception uses.
		 * Definition of this type definition is mandatory as base class
		 * retrieves and uses it. Its definition must be complete type
		 * as it is used for declaring a member field in the base class.
		 *
		 * Default type is void, to enforce redefinition of this trait.
		 */
		using ExceptionCodeType = VOID;
	};

	// NOTE: Add forward declarations of exception classes here as traits definitions
	// need it.

	class Win32ErrorException;

	/*
	 * Traits for Win32ErrorException.
	 */
	template <>
	struct Win32ExceptionTraits<Win32ErrorException>
	{
		/**
		 * GetLastError returns DWORD results.
		 */
		using ExceptionCodeType = DWORD;
	};

	/**
	 * Exception template class for encapsulating different
	 * kinds of error return types (GetLastError, HRESULT etc)
	 */
	template <
		class TWin32ExceptionImpl, 
		typename Traits = Win32ExceptionTraits<TWin32ExceptionImpl>>
	class Win32Exception
	{
	public:
		/**
		 * Gets rid of unnecessary template argument redundancy.
		 */
		using Win32ExceptionType = Win32Exception<TWin32ExceptionImpl>;

		/**
		 * Retrieves error code type which child class is able to handle.
		 */
		using Win32ExceptionCodeType = typename Traits::ExceptionCodeType;

		/**
		 * Construct exception with defined error code.
		 */
		explicit Win32Exception(Win32ExceptionCodeType const &error)
			: code_(error)
		{
		}

		/**
		 * Get error code resulting in this instance of exception.
		 */
		const Win32ExceptionCodeType& GetCode() const
		{
			return code_;
		}

		/*
		 * NOTE: I don't add conversion operator 'operator Win32ExceptionCodeType'
		 * as an implicit cast as this class doesn't directly represent an error 
		 * code itself, it represents the error itself. For code clarity though,
		 * we recommend using Win32Exception::GetCode().
		 */
		explicit operator Win32ExceptionCodeType() const
		{
			return code_;
		}

	private:
		/**
		 * Holds the error code.
		 */
		Win32ExceptionCodeType code_;
	};

	/**
	 * Encapsulates GetLastError() kind of errors.
	 */
	class Win32ErrorException : public Win32Exception<Win32ErrorException>
	{
	public:
		/**
		 * Construct from given error code.
		 */
		explicit Win32ErrorException(DWORD const &getLastErrorCode)
			: Win32ExceptionType::Win32Exception(getLastErrorCode)
		{
		}

		/**
		 * Construct from last error code returned by GetLastError().
		 */
		explicit Win32ErrorException()
			: Win32ErrorException::Win32ErrorException(::GetLastError())
		{
		}
	};
}