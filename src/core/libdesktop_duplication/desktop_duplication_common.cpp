//
// file:			desktop_duplication_common.cpp
// path:			src/core/libdesktop_duplication/desktop_duplication_common.cpp
// created on:		2026 Jan 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <cinternal/internal_header.h>

#if !defined(_WIN32)

#include <libdeskdupl/deskdupl.h>
#include <cinternal/signals.h>
#include <cinternal/logger.h>
#include <cinternal/disable_compiler_warnings.h>
#include <thread>
#include <QImage>
#include <QPixmap>
#include <QCoreApplication>
#include <QScreen>
#include <QPainter>
#include <cinternal/undisable_compiler_warnings.h>


static QPixmap GetScreenshotStatic(QRect* a_rectAll_p);


static inline QImage GetScreenshotInlineImg(void) {
    const QPixmap aPixMapAll = GetScreenshotStatic(nullptr);
    if (aPixMapAll.isNull()) return QImage();
    const QImage img = aPixMapAll.toImage().convertToFormat(QImage::Format_ARGB32);
    return img;
}


CPPUTILS_BEGIN_C


static ::std::thread* s_screenshotsThread = nullptr;
static TypeDesktopChange s_clbk = nullptr;
static void* s_userData = nullptr;
static bool s_bRun = false;

static void ScreenshotThreadFunction(void);


LIBDESKDUPL_EXPORT int RegisterAndStartDesktopChangeCalbakc(void* a_userData, TypeDesktopChange a_clbk)
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

    s_clbk = a_clbk;
    s_userData = a_userData;
    s_bRun = true;
    s_screenshotsThread = new ::std::thread(&ScreenshotThreadFunction);

    return 0;
}


LIBDESKDUPL_EXPORT void UnregisterDesktopChangeCalbakc(void)
{
    if (s_screenshotsThread != nullptr)
    {
        return;
    }

    ::std::thread* const screenshotsThread = s_screenshotsThread;
    s_screenshotsThread = nullptr;

    s_bRun = false;
    screenshotsThread->join();
    delete screenshotsThread;
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
        const QPixmap aPixMapAll = GetScreenshotStatic(nullptr);
        if (aPixMapAll.isNull()) return;
        const QImage img = aPixMapAll.toImage().convertToFormat(QImage::Format_ARGB32);
        (*s_clbk)(s_userData, &img);
    }  //  if (s_clbk) {
}


CPPUTILS_END_C


CPPUTILS_DLL_PRIVATE QImage GetMouseQImagePrivate(QPoint* CPPUTILS_ARG_NN a_pCursorPos);


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


static QPixmap GetScreenshotStatic(QRect* a_rectAll_p)
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
        QPoint cursorPos;
        QImage cursorPixmap = GetMouseQImagePrivate(&cursorPos);
        if (!cursorPixmap.isNull()) {
            QPainter painter(&aPixMapAll);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver); // Ensures alpha blending
            painter.drawImage(cursorPos - aRectAll.topLeft(), cursorPixmap);  // Draw cursor at its screen position
            painter.end();
        }
        // end 2024 Sep 24  - Set mouse image

    }  //  if(screensCount){

    if (a_rectAll_p) {
        *a_rectAll_p = aRectAll;
    }

    return aPixMapAll;
}


#endif  //  #if !defined(_WIN32)
