//
// repo:            FocusT p01 monitor
// file:            monitor_mouse_pixmap.cpp
// path:			src/core/basic/monitor_mouse_pixmap.cpp
// created on:		2024 Sep 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <cinternal/internal_header.h>


#if !defined(FOCUST_P01_MON_USE_PRIVATE_APP) && defined(CPPUTILS_OS_MACOS)

#include <libdeskdupl/deskdupl.h>
#include <cinternal/disable_compiler_warnings.h>
#include <qtutils/disable_utils_warnings.h>
#include <QPoint>
#include <QImage>
#include <AppKit/AppKit.h>
#include <ApplicationServices/ApplicationServices.h>
#include <cinternal/undisable_compiler_warnings.h>



CPPUTILS_BEGIN_C

// Function to get the current cursor pixmap and position on macOS
LIBDESKDUPL_EXPORT int DeskDuplGetMouseQImage(void* CPPUTILS_ARG_NN a_qtImageBuffer, void* a_pCursorPos)
{
    @autoreleasepool {
        // Get current global cursor position
        const NSPoint nsMouseLocation = [NSEvent mouseLocation];
        const NSScreen* targetScreen = nil;
        for (NSScreen* screen in [NSScreen screens]) {
            if (NSPointInRect(nsMouseLocation, [screen frame])) {
                targetScreen = screen;
                break;
            }
        }

        if (!targetScreen) {
            targetScreen = [NSScreen mainScreen]; // fallback
        }

        const CGFloat screenHeight = targetScreen.frame.size.height;

        if (a_pCursorPos) {
            QPoint* const pCursorPos = (QPoint*)a_pCursorPos;
            *pCursorPos = QPoint(
                static_cast<int>(nsMouseLocation.x ),
                static_cast<int>( screenHeight - nsMouseLocation.y )
            );
        }

        // Get current cursor image
        const NSCursor* currentCursor = [NSCursor currentSystemCursor];
        if (!currentCursor) return 1;

        const NSImage* nsImage = [currentCursor image];
        if (!nsImage) return 1;

        const CGSize size = [nsImage size];
        NSRect rect = NSMakeRect(0, 0, size.width, size.height);
        const CGImageRef cgImage = [nsImage CGImageForProposedRect:&rect context:nil hints:nil];
        if (!cgImage) return 1;

        QImage* const qtImageBuffer = (QImage*)a_qtImageBuffer;
        *qtImageBuffer = QImage(CGImageGetWidth(cgImage),
                       CGImageGetHeight(cgImage),
                       QImage::Format_ARGB32);

        CGContextRef ctx = CGBitmapContextCreate(qtImageBuffer->bits(),
                                                 qtImageBuffer->width(),
                                                 qtImageBuffer->height(),
                                                 8,
                                                 qtImageBuffer->bytesPerLine(),
                                                 CGImageGetColorSpace(cgImage),
                                                 kCGImageAlphaPremultipliedLast);

        if (!ctx) return 1;

        CGContextDrawImage(ctx, CGRectMake(0, 0, qtImageBuffer->width(), qtImageBuffer->height()), cgImage);
        CGContextRelease(ctx);

        return 0;
    }
}

CPPUTILS_END_C

#endif  //  #ifdef CPPUTILS_OS_MACOS
