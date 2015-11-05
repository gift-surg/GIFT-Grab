#ifndef __MACROS_H__
#define __MACROS_H__

#define DISALLOW_COPY_AND_ASSIGNMENT(TypeName) \
  TypeName(const TypeName&) = delete;   \
  void operator=(const TypeName&) = delete
  
#endif
