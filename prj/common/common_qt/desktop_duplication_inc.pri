#   
# file:		desktop_duplication_inc.pri
# path:		prj/core/libapp_and_browser_monitor_qt/libapp_and_browser_monitor.pro
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  

message("!!! $${PWD}/$${PWD}/.pri")
include ( "$${PWD}/../../common/common_qt/flagsandsys_common.pri" )

win32 {

    contains(QMAKE_TARGET.arch, x86_64) {
        CODENAMEBASE = x64
    } else {
        CODENAMEBASE = unknown
    }

    vs_common_dir = $${libDeskDuplRepoRoot}/prj/common/common_vs
    varEq=$$system(call msbuild  /p:Configuration=$${CONFIGURATION} /p:Platform=$${CODENAMEBASE}  $${vs_common_dir}\build_hlsl.vcxproj)
    INCLUDEPATH += $${libDeskDuplRepoRoot}/hlsls_out
    # d3d11.lib;dxgi.lib;Gdi32.lib;User32.lib
    LIBS += -ld3d11
    LIBS += -lGdi32
    LIBS += -lUser32

} else:linux {

    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    LIBS += -lXfixes
    LIBS += -lX11

}

# LIBDESKDUPL_DDAPI_NOT_NEEDED
isEmpty(LIBDESKDUPL_DDAPI_NOT_NEEDED) {
    LIBDESKDUPL_DDAPI_NOT_NEEDED = $$(LIBDESKDUPL_DDAPI_NOT_NEEDED)
    isEmpty(LIBDESKDUPL_DDAPI_NOT_NEEDED) {
        message("-- LIBDESKDUPL_DDAPI_NOT_NEEDED is not defined")
    } else {
        DEFINES += LIBDESKDUPL_DDAPI_NOT_NEEDED
        message("++ LIBDESKDUPL_DDAPI_NOT_NEEDED is defined")
    }
} else {
    DEFINES += LIBDESKDUPL_DDAPI_NOT_NEEDED
    message("++ LIBDESKDUPL_DDAPI_NOT_NEEDED is defined")
}


OBJECTIVE_SOURCES += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.mm,true)

SOURCES += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.c,true)
SOURCES += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.cpp,true)

HEADERS += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.h,true)
HEADERS += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.hpp,true)
HEADERS += $$files($${libDeskDuplRepoRoot}/include/*.h,true)
HEADERS += $$files($${libDeskDuplRepoRoot}/include/*.hpp,true)

OTHER_FILES += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.mm,true)
