#pragma once

#include <windows.h>
#include <utility>
#include <type_traits>

#ifdef _DEBUG
#include <sstream>
#endif

#include "win32exception.h"

namespace virgo
{
  /**
   * Handle template class for abstracting handle types.
   */
  template <class THandleImpl>
  class handle
  {
  public:
    /**
     * Gets the template argument out of unnecessary sight.
     */
    using handle_type = handle<THandleImpl>;

    /*
     * Initialize from existing HANDLE.
     */
    explicit handle(HANDLE& input)
      : handle_(input)
    {
    }

    /**
     * Copy constructor.
     */
    explicit handle(const handle_type& other)
      : handle_(INVALID_HANDLE_VALUE)
    {
      // HELPREQUESTED: Maybe get process handle from the handle and pass it onto 
      // DuplicateHandle function for cross-process handle duplication.

      HANDLE& selfHandle{ static_cast<THandleImpl*>(this)->handle_ };
      HANDLE& otherHandle{ static_cast<THandleImpl*>(other)->handle_ };

      duplicate(other.handle_, handle_);
    }

    /*
     * Move constructor.
     */
    explicit handle(handle_type&& other)
      : handle_(std::move(other.handle_))
    {
      // No need to use DuplicateHandle as we're only transferring ownership to this
      // instance of Handle.

      other.handle_ = INVALID_HANDLE_VALUE;
    }

    /*
     * Copy assignment.
     */
    handle_type& operator=(const handle_type& other)
    {
      THandleImpl& self{ static_cast<THandleImpl&>(*this) };


      if (this != &other)
      {
        duplicate(other.handle_, handle_);
      }

      return *this;
    }

    /*
     * Move assignment.
     */
    handle_type& operator=(handle_type&& other_)
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
    virtual ~handle()
    {
      try
      {
        close();
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
    void close()
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
    static void duplicate(const HANDLE& a, HANDLE& b)
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