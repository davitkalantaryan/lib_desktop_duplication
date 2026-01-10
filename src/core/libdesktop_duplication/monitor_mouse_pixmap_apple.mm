//
// repo:            FocusT p01 monitor
// file:            monitor_mouse_pixmap.cpp
// path:			src/core/basic/monitor_mouse_pixmap.cpp
// created on:		2024 Sep 09
// created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
//

#include <cinternal/internal_header.h>


#if !defined(FOCUST_P01_MON_USE_PRIVATE_APP) && defined(CPPUTILS_OS_MACOS)

#include <monitor/core/mouse_pixmap.hpp>
#include <cinternal/disable_compiler_warnings.h>
#include <AppKit/AppKit.h>
#include <ApplicationServices/ApplicationServices.h>
#include <cinternal/undisable_compiler_warnings.h>


namespace focust { namespace monitor { namespace globals{


// Function to get the current cursor pixmap and position on macOS
CPPUTILS_DLL_PRIVATE QImage GetMouseQImagePrivate(QPoint* CPPUTILS_ARG_NN a_pCursorPos)
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
            *a_pCursorPos = QPoint(
                static_cast<int>(nsMouseLocation.x ),
                static_cast<int>( screenHeight - nsMouseLocation.y )
            );
        }

        // Get current cursor image
        const NSCursor* currentCursor = [NSCursor currentSystemCursor];
        if (!currentCursor) return QImage();

        const NSImage* nsImage = [currentCursor image];
        if (!nsImage) return QImage();

        const CGSize size = [nsImage size];
        NSRect rect = NSMakeRect(0, 0, size.width, size.height);
        const CGImageRef cgImage = [nsImage CGImageForProposedRect:&rect context:nil hints:nil];
        if (!cgImage) return QImage();

        QImage qtImage(CGImageGetWidth(cgImage),
                       CGImageGetHeight(cgImage),
                       QImage::Format_ARGB32);

        CGContextRef ctx = CGBitmapContextCreate(qtImage.bits(),
                                                 qtImage.width(),
                                                 qtImage.height(),
                                                 8,
                                                 qtImage.bytesPerLine(),
                                                 CGImageGetColorSpace(cgImage),
                                                 kCGImageAlphaPremultipliedLast);

        if (!ctx) return QImage();

        CGContextDrawImage(ctx, CGRectMake(0, 0, qtImage.width(), qtImage.height()), cgImage);
        CGContextRelease(ctx);

        return qtImage;
    }
}


}}}  //  namespace focust { namespace monitor { namespace globals{

#endif  //  #ifdef CPPUTILS_OS_MACOS
