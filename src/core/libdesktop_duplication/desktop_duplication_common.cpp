//
// file:			desktop_duplication_common.cpp
// path:			src/core/libdesktop_duplication/desktop_duplication_common.cpp
// created on:		2026 Jan 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <libdeskdupl/export_symbols.h>

#ifndef LIBDESKDUPL_HAS_DDAPI

#include <libdeskdupl/deskdupl.h>
#include "desktop_duplication_private_header.h"
#include <cinternal/signals.h>
#include <cinternal/logger.h>
#include <cinternal/disable_compiler_warnings.h>
#include <thread>
#include <qtutils/disable_utils_warnings.h>
#include <QImage>
#include <QPixmap>
#include <QCoreApplication>
#include <QScreen>
#include <QPainter>
#include <cinternal/undisable_compiler_warnings.h>


CPPUTILS_BEGIN_C


static ::std::thread* s_screenshotsThread = nullptr;
static TypeDesktopChange s_clbk = nullptr;
static void* s_userData = nullptr;
static bool s_bRun = false;

static void ScreenshotThreadFunction(void);


LIBDESKDUPL_EXPORT int DeskDuplRegisterAndStartDesktopChangeCalbakc(void* a_userData, TypeDesktopChange a_clbk)
{
    if (s_screenshotsThread != nullptr)
    {
        // Already registered/running
        return 0;
    }

    if (a_clbk == nullptr) {
        // bad callback
        return -1;
    }

    gp_screenImageFromDupl_p = new QImage();
    s_clbk = a_clbk;
    s_userData = a_userData;
    s_bRun = true;
    s_screenshotsThread = new ::std::thread(&ScreenshotThreadFunction);

    return 0;
}


LIBDESKDUPL_EXPORT void DeskDuplUnregisterDesktopChangeCalbakc(void)
{
    if (s_screenshotsThread == nullptr)
    {
        return;
    }

    QImage* const screenImageFromDupl_p = gp_screenImageFromDupl_p;
    ::std::thread* const screenshotsThread = s_screenshotsThread;
    s_screenshotsThread = nullptr;
    gp_screenImageFromDupl_p = nullptr;

    s_bRun = false;
    screenshotsThread->join();
    delete screenshotsThread;
    delete screenImageFromDupl_p;
}


static void MakeScreenshotsAndCallCallback(void);

static void ScreenshotThreadFunction(void)
{
    MakeScreenshotsAndCallCallback();
    while (s_bRun) {
        CinternalSleepInterruptableMs(200);
        MakeScreenshotsAndCallCallback();
    } // while (s_bRun) {
}


static void MakeScreenshotsAndCallCallback(void)
{
    if (s_clbk) {
        QImage img;
        if(!DeskDuplGetCurrentScreen(&img,nullptr,nullptr)){

            {  //  lock start
                ::std::lock_guard<::std::mutex> aGuard(gp_mutexForLastScreenImage);
                *gp_screenImageFromDupl_p = img;
            }  //  lock end

            (*s_clbk)(s_userData, &img);
        }  //  if(!DeskDuplGetCurrentScreen(&img,nullptr,nullptr)){
    }  //  if (s_clbk) {
}


CPPUTILS_END_C



#endif  //  #ifndef LIBDESKDUPL_HAS_DDAPI
