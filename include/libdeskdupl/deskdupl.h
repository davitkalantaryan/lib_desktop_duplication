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


typedef void (*TypeDesktopChange)(void* usrData,const void* qtImage);

LIBDESKDUPL_EXPORT int DeskDuplRegisterAndStartDesktopChangeCalbakc(void* a_userData, TypeDesktopChange a_clbk);
LIBDESKDUPL_EXPORT void DeskDuplUnregisterDesktopChangeCalbakc(void);
LIBDESKDUPL_EXPORT int DeskDuplGetMouseQImage(void* CPPUTILS_ARG_NN a_qtImageBuffer, void* a_pCursorPos); // 1. QImage*, 2. QPoint* - can be null
LIBDESKDUPL_EXPORT int DeskDuplGetCurrentScreen(void* CPPUTILS_ARG_NN a_qtImageBuffer, void* a_rectAll_p, void* a_pCursorPos); // 1. QImage*, 2. QRect* - can be null, 3 QPoint*


CPPUTILS_END_C


#endif  //  #ifndef LIBDESKDUPL_INCLUDE_LIBDESKDUPL_DESKDUPL_H
