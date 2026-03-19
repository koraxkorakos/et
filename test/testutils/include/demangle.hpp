#pragma once
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// This file declares a function to demangle symbol names, which might e.g. be
/// usefule to log exception type names.
///
/// \author Christian Kotz
/// \version 0.1 beta
/// \date 10/01/2020
///
/// \file header collection all headers
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       \copyright Cargometer GesmbH 2020
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <string>

namespace testing {
std::string demangle(char const symbolName[]);

#ifdef __GNUG__
/// \brief exception thrown if demangling fails
class DemanglingFailed : public std::runtime_error {
 public:
  /// \brief error cause reported by the compiler
  enum Cause : int {
    success = 0,
    outOfMemory = -1,
    noMangledNamed = -2,
    invalidArgument = -3,
    // more unnamed codes could exist is gcc does not stoick to its ABI
  };
  explicit DemanglingFailed(Cause cause, char const *symbol = nullptr);
  Cause cause;
  char const *symbol;
};
#endif

template <typename T> inline auto demangle_t() {
  return demangle(typeid(T).name());
}
}  // namespace testing
