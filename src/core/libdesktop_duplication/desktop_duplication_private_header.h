//
// repo:            lib_desktop_duplication
// file:			desktop_duplication_private_header.h
// path:			src/core/libdesktop_duplication/desktop_duplication_private_header.h
// created on:		2026 Jan 13
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//


#pragma once


#include <libdeskdupl/export_symbols.h>
#include <cinternal/disable_compiler_warnings.h>
#include <mutex>
#include <qtutils/disable_utils_warnings.h>
#include <QImage>
#include <cinternal/undisable_compiler_warnings.h>


extern CPPUTILS_DLL_PRIVATE QImage* gp_screenImageFromDupl_p;
extern CPPUTILS_DLL_PRIVATE ::std::mutex gp_mutexForLastScreenImage;
