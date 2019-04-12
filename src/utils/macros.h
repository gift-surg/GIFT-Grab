#include <ciso646>  // needed for e.g. "and" to be recognised on Windows to mean "&&"

#ifndef __MACROS_H__
#define __MACROS_H__

#define DISALLOW_COPY_AND_ASSIGNMENT(TypeName) \
  TypeName(const TypeName&) = delete;   \
  void operator=(const TypeName&) = delete
  
#endif
