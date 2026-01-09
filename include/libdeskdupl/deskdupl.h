//
// file:			deskdupl.h
// path:			include/libdeskdupl/deskdupl.h
// created on:		2026 Jan 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#ifndef LIBDESKDUPL_INCLUDE_LIBDESKDUPL_DESKDUPL_H
#define LIBDESKDUPL_INCLUDE_LIBDESKDUPL_DESKDUPL_H

#include <libdeskdupl/export_symbols.h>

CPPUTILS_BEGIN_C


typedef void (*TypeDesktopChange)(void*,const void*);

LIBDESKDUPL_EXPORT int RegisterAndStartDesktopChangeCalbakc(void* a_userData, TypeDesktopChange a_clbk);
LIBDESKDUPL_EXPORT void UnregisterDesktopChangeCalbakc(void);


CPPUTILS_END_C


#endif  //  #ifndef LIBDESKDUPL_INCLUDE_LIBDESKDUPL_DESKDUPL_H
