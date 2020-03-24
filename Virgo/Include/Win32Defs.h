#pragma once

#include <windows.h>

#include <string>
#include <memory>

namespace Virgo 
{
	/**
	 * Defines allocator traits for LocalAlloc/LocalFree pair.
	 */

	

	/**
	 * Defines string class usable with Win32 APIs.
	 */
	using TCharString = std::basic_string<TCHAR>;
}