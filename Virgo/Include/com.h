#pragma once

#include "win32defs.h"
#include "win32exception.h"
#include <objbase.h>

namespace virgo {
  //
  // Gives GUID information to the class.
  //
  template <GUID const& _Guid = GUID_NULL>
  struct com_traits {
    const GUID& Guid = _Guid;
  };

  //
  // Combines GUID information with the IUnknown.
  //
  template <typename TSelf, GUID const& _Guid = GUID_NULL>
  struct com_interface : com_traits<_Guid> {

  };

  // {2611D11F-CDE2-4FA5-9C95-CB90A518E603}
  static const GUID IID_IVirgo =
  { 0x2611d11f, 0xcde2, 0x4fa5, { 0x9c, 0x95, 0xcb, 0x90, 0xa5, 0x18, 0xe6, 0x3 } };

  class IVirgo : com_interface<IVirgo, IID_IVirgo> {
  };
}