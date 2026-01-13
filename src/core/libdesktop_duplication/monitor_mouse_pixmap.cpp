//
// repo:            FocusT p01 monitor
// file:            monitor_mouse_pixmap.cpp
// path:			src/core/basic/monitor_mouse_pixmap.cpp
// created on:		2024 Sep 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <libdeskdupl/export_symbols.h>

#if defined(FOCUST_P01_MON_USE_PRIVATE_APP) || !defined(CPPUTILS_OS_MACOS)

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <cinternal/logger.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QScreen>
#include <QGuiApplication>
#include <QImage>
#include <QPoint>
#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
typedef HBITMAP MonImageRef;
#elif defined(Q_OS_MACOS)
#include <ApplicationServices/ApplicationServices.h>
typedef CGImageRef MonImageRef;
extern "C" CGImageRef CGSGetCursorImage(void);
#elif defined(Q_OS_LINUX)
#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
typedef XFixesCursorImage* MonImageRef;
#else
#error this platform is not supported
#endif
#include <cinternal/undisable_compiler_warnings.h>


#ifndef _WIN32
static QImage XCursorImageToQImage(MonImageRef hBitmap);
#endif

CPPUTILS_BEGIN_C

#ifdef _WIN32

// Function to get the current cursor pixmap and position
LIBDESKDUPL_EXPORT int DeskDuplGetMouseQImage(void* CPPUTILS_ARG_NN a_qtImageBuffer, void* a_pCursorPos)
{
    CURSORINFO ci{};
    ci.cbSize = sizeof(CURSORINFO);
    if (!GetCursorInfo(&ci)) return QImage();
    if (!(ci.flags & CURSOR_SHOWING) || !ci.hCursor) return QImage();

    HCURSOR hCursor = ci.hCursor;

    // --- Get size + hotspot ---
    ICONINFO ii{};
    if (!GetIconInfo(hCursor, &ii)){
        return 1;
    }

    // Compute logical cursor size (mask-only cursors store AND+XOR stacked)
    int w = 0, h = 0;
    BITMAP bm{};
    if (ii.hbmColor) {
        if (GetObject(ii.hbmColor, sizeof(bm), &bm)) { w = bm.bmWidth; h = bm.bmHeight; }
    } else if (ii.hbmMask) {
        if (GetObject(ii.hbmMask, sizeof(bm), &bm)) { w = bm.bmWidth; h = bm.bmHeight / 2; }
    }
    const POINT hot{ LONG(ii.xHotspot), LONG(ii.yHotspot) };

    // --- Render to 32-bpp top-down DIB using DrawIconEx (handles alpha + mono) ---
    BITMAPINFO bi{};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = w;
    bi.bmiHeader.biHeight      = -h;                 // top-down
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC screenDC = GetDC(nullptr);
    if (!screenDC) {
        if (ii.hbmColor) DeleteObject(ii.hbmColor);
        if (ii.hbmMask)  DeleteObject(ii.hbmMask);
        return 1;
    }

    HBITMAP dib  = CreateDIBSection(screenDC, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HDC memDC    = CreateCompatibleDC(screenDC);
    HGDIOBJ old  = SelectObject(memDC, dib);
    // Clear to transparent to avoid garbage under partially-transparent cursors
    memset(bits, 0, size_t(w) * size_t(h) * 4);
    DrawIconEx(memDC, 0, 0, hCursor, w, h, 0, nullptr, DI_NORMAL);
    SelectObject(memDC, old);
    DeleteDC(memDC);
    ReleaseDC(nullptr, screenDC);

    // Wrap DIB memory into QImage, then detach so we can free the DIB
    QImage img(reinterpret_cast<uchar*>(bits), w, h, QImage::Format_ARGB32_Premultiplied);
    QImage* const qtImageBuffer = (QImage*)a_qtImageBuffer;
    *qtImageBuffer = img.copy();
    DeleteObject(dib);

    // Cleanup ICONINFO allocations
    if (ii.hbmColor) DeleteObject(ii.hbmColor);
    if (ii.hbmMask)  DeleteObject(ii.hbmMask);

    // --- Map physical pixels → Qt logical (handles HiDPI) ---
    // Get monitor DPI at the cursor point.
    qreal scale = 1.0;
    {
        // Prefer Win10 per-monitor DPI
        HMONITOR hMon = MonitorFromPoint(ci.ptScreenPos, MONITOR_DEFAULTTONEAREST);
        UINT dpiX = 96, dpiY = 96;

        // Use GetDpiForMonitor if available (runtime load to avoid hard link requirement)
        using GetDpiForMonitorFn = HRESULT (WINAPI *)(HMONITOR, int /*MDT_*/ , UINT*, UINT*);
        static GetDpiForMonitorFn pGetDpiForMonitor = []() -> GetDpiForMonitorFn {
            HMODULE shcore = LoadLibraryW(L"Shcore.dll");
            if (!shcore) return nullptr;
#pragma warning (disable:4191)
            return reinterpret_cast<GetDpiForMonitorFn>(GetProcAddress(shcore, "GetDpiForMonitor"));
        }();

        if (pGetDpiForMonitor) {
            // MDT_EFFECTIVE_DPI = 0
            if (SUCCEEDED(pGetDpiForMonitor(hMon, /*MDT_EFFECTIVE_DPI*/ 0, &dpiX, &dpiY))) {
                scale = qMax<qreal>(1.0, dpiX / 96.0);
            }
        } else {
            // Fallback to Qt's view of DPR for the screen under the cursor
            const QPoint physPt(ci.ptScreenPos.x, ci.ptScreenPos.y);
            QScreen* scr = nullptr;
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
            scr = QGuiApplication::screenAt(physPt);
#endif
            if (!scr) scr = QGuiApplication::primaryScreen();
            if (scr) {
                // Approximate: DPI/96 if available; else devicePixelRatio.
                const qreal dpi = scr->logicalDotsPerInch();
                const qreal guess = dpi > 0 ? (dpi / 96.0) : scr->devicePixelRatio();
                scale = qMax<qreal>(1.0, guess);
            }
        }
    }

    // Apply hotspot + scale to the position returned via a_pCursorPos (Qt logical)
    const qreal inv = 1.0 / scale;
    const qreal x = (ci.ptScreenPos.x - hot.x) * inv;
    const qreal y = (ci.ptScreenPos.y - hot.y) * inv;
    if(a_pCursorPos){
        *a_pCursorPos = QPoint(qFloor(x + 0.5), qFloor(y + 0.5)); // center-round
    }

    // Scale the image to Qt logical so drawImage uses same coord space
    if (scale != 1.0) {
        const QSize target(qMax(1, int(qRound(out.width()  * inv))),
                           qMax(1, int(qRound(out.height() * inv))));
        out = out.scaled(target, Qt::IgnoreAspectRatio, Qt::FastTransformation); // crisp
    }

    return 0;
}


#elif defined(Q_OS_MACOS)

// Function to get the current cursor pixmap and position on macOS
LIBDESKDUPL_EXPORT int DeskDuplGetMouseQImage(void* CPPUTILS_ARG_NN a_qtImageBuffer, void* a_pCursorPos)
{
    CGImageRef cursorImage = CGSGetCursorImage();
    if (!cursorImage) {
        return 1;
    }

    // Create a dummy event to get current mouse location
    CGEventRef event = CGEventCreate(NULL);
    if (!event) {
        CGImageRelease(cursorImage);
        return 1;
    }

    CGPoint mouseLocation = CGEventGetLocation(event);
    CFRelease(event);

    if (a_pCursorPos) {
        *a_pCursorPos = QPoint(static_cast<int>(mouseLocation.x), static_cast<int>(mouseLocation.y));
    }

    QImage* const qtImageBuffer = (QImage*)a_qtImageBuffer;
    *qtImageBuffer = XCursorImageToQImage(cursorImage);
    CGImageRelease(cursorImage);
    return 0;
}


#elif defined(Q_OS_LINUX)

// Function to get the current cursor pixmap and position on Linux (X11)
LIBDESKDUPL_EXPORT int DeskDuplGetMouseQImage(void* CPPUTILS_ARG_NN a_qtImageBuffer, void* a_pCursorPos)
{
    Display* display = XOpenDisplay(NULL);
    if (!display) {
        CInternalLogWarning("Unable to open X display.");
        return 1;
    }

    // Step 1: Get the root window and cursor position using XQueryPointer
    Window root = DefaultRootWindow(display);
    Window retRoot, retChild;
    int rootX, rootY, winX, winY;
    unsigned int mask;
    XQueryPointer(display, root, &retRoot, &retChild, &rootX, &rootY, &winX, &winY, &mask);

    // Set the cursor position if a valid pointer is provided
    if (a_pCursorPos) {
        QPoint* const pCursorPos = (QPoint*)a_pCursorPos;
        *pCursorPos = QPoint(rootX, rootY);
    }

    // Step 2: Get the cursor image using XFixesGetCursorImage
    XFixesCursorImage* xCursorImage = XFixesGetCursorImage(display);
    if (!xCursorImage) {
        CInternalLogWarning("Unable to get cursor image.");
        XCloseDisplay(display);
        return 1;
    }

    // Step 3: Convert the XFixesCursorImage to QImage
    QImage* const qtImageBuffer = (QImage*)a_qtImageBuffer;
    *qtImageBuffer = XCursorImageToQImage(xCursorImage);

    // Step 4: Free the XFixesCursorImage data
    XFree(xCursorImage);

    // Close the X display connection
    XCloseDisplay(display);

    return 0;
}

#endif  //  #ifdef _WIN32


CPPUTILS_END_C


#ifdef _WIN32



#elif defined(Q_OS_MACOS)

// Convert CGImageRef to QImage
static QImage XCursorImageToQImage(MonImageRef cgImage)
{
    if (!cgImage) {
        return QImage();
    }

    size_t width = CGImageGetWidth(cgImage);
    size_t height = CGImageGetHeight(cgImage);
    size_t bytesPerRow = CGImageGetBytesPerRow(cgImage);

    // Get the image data
    CFDataRef dataRef = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    const unsigned char* data = CFDataGetBytePtr(dataRef);

    // Create a QImage from the raw data
    QImage img(data, width, height, bytesPerRow, QImage::Format_ARGB32);

    // Release the data
    CFRelease(dataRef);

    return img;
}

#elif defined(Q_OS_LINUX)

// Convert XFixes cursor image to QImage
static QImage XCursorImageToQImage(MonImageRef xCursorImage) {
    if (!xCursorImage) {
        return QImage();
    }

    // Create a QImage from the X cursor image data (which is in ARGB format)
    QImage img(reinterpret_cast<uchar*>(xCursorImage->pixels),
               xCursorImage->width,
               xCursorImage->height,
               QImage::Format_ARGB32);

    return img;
}

#endif  //  #ifdef _WIN32

#endif  //  #if defined(FOCUST_P01_MON_USE_PRIVATE_APP) || !defined(CPPUTILS_OS_MACOS)

