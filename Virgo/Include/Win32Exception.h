#pragma once

#include <windows.h>

namespace virgo
{
  /**
   * Makes exception to be thrown according to errorCode.
   */
  std::system_error NewException(DWORD errorCode)
  {
    LPSTR psz{ nullptr };
    const DWORD cchMessage{ ::FormatMessageA(
      FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER,
      NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&psz), 0, NULL) };

    if (cchMessage > 0)
    {
      auto destructor = [](void* p) { ::LocalFree(p); };
      std::unique_ptr<CHAR, decltype(destructor)> ptr{ psz, destructor };
      std::string string{ ptr.get(), cchMessage };
      return std::system_error(errorCode, std::system_category(), string.c_str());
    }
    else
    {
      auto error_code{ ::GetLastError() };
      throw std::system_error(errorCode, std::system_category(), "Failed to retrieve error text from system");
    }
  }
}