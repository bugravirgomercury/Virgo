#pragma once

#include <iostream>
#include <memory>
#include "win32defs.h"
#include "win32exception.h"

namespace virgo {
  //
  // Streambuf class for file-like HANDLEs
  //
  class hbuf : public std::streambuf {
  public:
    static constexpr const int HBUFSIZE = 256;

    hbuf(HANDLE& handle) : h(handle) {
      setg(gbuf, gbuf + HBUFSIZE, gbuf + HBUFSIZE);
      setp(pbuf, pbuf + HBUFSIZE);
    }

    ~hbuf() {
      sync();
    }

    //
    // Get data from handle to get area
    //
    int underflow() {
      DWORD dwRead = 0;
      if (auto f = !ReadFile(h, gbuf, HBUFSIZE, &dwRead, NULL)) {
        return -1;
      }

      if (dwRead <= 0) return -1;
      setg(gbuf, gbuf, gbuf + dwRead); // Set get area pointers.
      return 0;
    }

    //
    // Put data from put area into handle
    //
    int overflow(int c) {
      DWORD dwWritten = 0;
      if (c == EOF) return sync();
      if (auto f = !WriteFile(h, pbuf, HBUFSIZE, &dwWritten, NULL)) {
        return -1;
      }

      setp(pbuf, pbuf + dwWritten);   // Set put area pointers
      *pptr() = c;                    // Write out the last one
      pbump(1);                       // Advance put pointer
      return c;                       // ???
    }

    //
    // Flush file buffers.
    //
    int sync() {
      // Check whether there is data waiting to be flushed.
      if (pptr() > pbase()) {
        DWORD dwWritten = 0;
        if (auto f = !WriteFile(h, pbuf, pptr() - pbuf, &dwWritten, NULL)) {
          return -1;
        } else if (dwWritten != pptr() - pbuf) {
          return -1;
        }
      }

      setp(pbuf, pbuf + HBUFSIZE);    // Set put area.
      return 0;
    }

  private:
    char gbuf[HBUFSIZE];
    char pbuf[HBUFSIZE];
    HANDLE h;
  };

  //
  // Wraps a HANDLE into iostream compatible class.
  //
  class hstream : public std::iostream {
  public:
    hstream(std::unique_ptr<hbuf>& hb) : std::iostream(hb.get()) { }
    hstream(HANDLE& h) : std::iostream(new hbuf(h)) { }
  };
}