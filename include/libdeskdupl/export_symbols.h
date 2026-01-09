//
// file:			export_symbols.h
// path:			include/libdeskdupl/export_symbols.h
// created on:		2026 Jan 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef LIBDESKDUPL_INCLUDE_LIBDESKDUPL_EXPORT_SYMBOLS_H
#define LIBDESKDUPL_INCLUDE_LIBDESKDUPL_EXPORT_SYMBOLS_H

#include <cinternal/internal_header.h>


#ifndef LIBDESKDUPL_EXPORT
#if defined(LIBDESKDUPL_COMPILING_SHARED_LIB)
#define LIBDESKDUPL_EXPORT CPPUTILS_DLL_PUBLIC
#elif defined(LIBDESKDUPL_USING_STATIC_LIB_OR_OBJECTS)
#define LIBDESKDUPL_EXPORT
#elif defined(LIBDESKDUPL_LOAD_FROM_DLL)
#define LIBDESKDUPL_EXPORT CPPUTILS_IMPORT_FROM_DLL
#else
#define LIBDESKDUPL_EXPORT CPPUTILS_DLL_PRIVATE
#endif
#endif


#endif  // #ifndef LIBDESKDUPL_INCLUDE_LIBDESKDUPL_EXPORT_SYMBOLS_H
