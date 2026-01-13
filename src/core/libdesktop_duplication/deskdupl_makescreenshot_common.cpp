//
// file:			desktop_duplication_common.cpp
// path:			src/core/libdesktop_duplication/desktop_duplication_common.cpp
// created on:		2026 Jan 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <libdeskdupl/deskdupl.h>
#include "desktop_duplication_private_header.h"
#include <cinternal/signals.h>
#include <cinternal/logger.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QPixmap>
#include <QCoreApplication>
#include <QScreen>
#include <QPainter>
#include <cinternal/undisable_compiler_warnings.h>


CPPUTILS_DLL_PRIVATE QImage* gp_screenImageFromDupl_p = nullptr;
CPPUTILS_DLL_PRIVATE ::std::mutex gp_mutexForLastScreenImage;

static QPixmap GetScreenshotStatic(QRect* a_rectAll_p, QPoint* a_cursorPos_p);


CPPUTILS_BEGIN_C


LIBDESKDUPL_EXPORT int DeskDuplGetCurrentScreen(void* CPPUTILS_ARG_NN a_qtImageBuffer, void* a_rectAll_p, void* a_pCursorPos)
{
    QImage* const qtImageBuffer = (QImage*)a_qtImageBuffer;
    bool bHasData = false;
    if(gp_screenImageFromDupl_p){
        ::std::lock_guard< ::std::mutex > aGuard(gp_mutexForLastScreenImage);
        if(gp_screenImageFromDupl_p){
            *qtImageBuffer = *gp_screenImageFromDupl_p;
            bHasData = true;
        }  //  if(gp_screenImageFromDupl_p){  --  2
    }  //  if(gp_screenImageFromDupl_p){  --  1

    if(bHasData){
        return 0;
    }

    QRect* const rectAll_p = (QRect*)a_rectAll_p;
    QPoint* const pCursorPos = (QPoint*)a_pCursorPos;
    const QPixmap aPixMapAll = GetScreenshotStatic(rectAll_p,pCursorPos);
    if (aPixMapAll.isNull()) return 1;
    *qtImageBuffer = aPixMapAll.toImage().convertToFormat(QImage::Format_ARGB32);
    return 0;
}


CPPUTILS_END_C



struct SPixAndRect {
    QRect m_rect;
    QPixmap m_pxMap;
};


static inline QPixmap ConcatenatePixmaps(
    QRect* CPPUTILS_ARG_NN a_pReturnRect,
    const QPixmap& a_pixmap1, const QPixmap& a_pixmap2,
    QRect a_rect1, QRect a_rect2)
{
    // Find the bounding rectangle that contains both a_rect1 and a_rect2
    *a_pReturnRect = a_rect1.united(a_rect2);

    // Create the result pixmap with the size of the united rectangle
    QPixmap result(a_pReturnRect->size());
    result.fill(Qt::transparent); // Fill the result pixmap with transparency

    // Create a QPainter to draw the pixmaps on the result pixmap
    QPainter painter(&result);

    // Draw the first pixmap at its designated position according to a_rect1
    painter.drawPixmap(a_rect1.topLeft() - a_pReturnRect->topLeft(), a_pixmap1);

    // Draw the second pixmap at its designated position according to a_rect2
    painter.drawPixmap(a_rect2.topLeft() - a_pReturnRect->topLeft(), a_pixmap2);

    return result;
}


static QPixmap GetScreenshotStatic(QRect* a_rectAll_p, QPoint* a_cursorPos_p)
{
    QRect aRectAll;
    QPixmap aPixMapAll;

    //const QList<QScreen*> allScreens = QGuiApplication::screens();
    //const qsizetype screensCount = allScreens.size();
    // instead of above code let's have following
    QList<SPixAndRect> allPixmaps;
    QMetaObject::invokeMethod(qApp, [&allPixmaps]() {
        try {
            QScreen* pScreen;
            const QList<QScreen*> allScreens = QGuiApplication::screens();
            const qsizetype screensCount = allScreens.size();
            for (qsizetype i(0); i < screensCount; ++i) {
                pScreen = allScreens.at(i);
                if (pScreen) {
                    QPixmap pxMp = pScreen->grabWindow(0);
                    if (!pxMp.isNull()) {
                        allPixmaps.push_back({ pScreen->geometry(),::std::move(pxMp) });
                    }
                }  //  if(pScreen){
            }  //  for(i=0; i<screensCount;++i){
        }
        catch (const ::std::bad_alloc& a_exc)
        {
            (void)a_exc;
            //CInternalLogCritical("bad alloc exception what: \"%s\"",excpWhat);
        }
        catch (...) {
            CInternalLogCritical("Unknown exception accured");
        }
    }, Qt::BlockingQueuedConnection);
    const qsizetype screensCount = allPixmaps.size();

    if (screensCount) {
        aRectAll = allPixmaps[0].m_rect;
        aPixMapAll = allPixmaps[0].m_pxMap;

        for (qsizetype i(1); i < screensCount; ++i) {
            aPixMapAll = ConcatenatePixmaps(&aRectAll, aPixMapAll, allPixmaps[i].m_pxMap, aRectAll, allPixmaps[i].m_rect);
        }  //  for(i=0; i<screensCount;++i){

        // 2024 Sep 24 - Set mouse image
        QImage cursorPixmap;
        QPoint cursorPos;
        if(!DeskDuplGetMouseQImage(&cursorPixmap,&cursorPos)){
            QPainter painter(&aPixMapAll);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver); // Ensures alpha blending
            painter.drawImage(cursorPos - aRectAll.topLeft(), cursorPixmap);  // Draw cursor at its screen position
            painter.end();
            if(a_cursorPos_p){
                *a_cursorPos_p = cursorPos;
            }
        }
        // end 2024 Sep 24  - Set mouse image

    }  //  if(screensCount){

    if (a_rectAll_p) {
        *a_rectAll_p = aRectAll;
    }

    return aPixMapAll;
}
