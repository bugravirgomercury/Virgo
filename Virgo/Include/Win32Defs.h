#pragma once

#include <windows.h>

#include <string>
#include <memory>
#include <new>
#include <limits>

namespace virgo
{
  /**
   * Defines allocator traits for LocalAlloc/LocalFree pair.
   */
  template <class T>
  class localalloc_allocator
  {
  public:
    using value_type = T;

    localalloc_allocator() = default;

    template <class U> constexpr localalloc_allocator(const localalloc_allocator<U>&) noexcept
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
  bool operator==(const localalloc_allocator<T>&, const localalloc_allocator<U>&)
  {
    return true;
  }

  template <class T, class U>
  bool operator!=(const localalloc_allocator<T>&, const localalloc_allocator<U>&)
  {
    return false;
  }

  /**
   * Defines string class usable with Win32 APIs.
   */
  using tstring = std::basic_string<TCHAR>;
  using cstring = std::basic_string<CHAR>;
}