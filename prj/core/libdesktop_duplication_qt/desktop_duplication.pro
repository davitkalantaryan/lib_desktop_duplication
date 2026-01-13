#   
# file:		libapp_and_browser_monitor.pro
# path:		prj/core/libapp_and_browser_monitor_qt/libapp_and_browser_monitor.pro
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  

TEMPLATE = lib
message("!!! $${_PRO_FILE_}")
include ( "$${PWD}/../../common/common_qt/flagsandsys_common.pri" )
include ( "$${cinternalRepoRoot}/prj/common/common_qt/core_lib_flags.pri" )

DEFINES += LIBDESKDUPL_COMPILING_SHARED_LIB

win32 {

    #LIBS += -lWtsapi32
    #LIBS += -lUser32
    # Import a .props file
    #QMAKE_EXTRA_TARGETS += import_props

    # For MSVC, use the /p option to pass properties
    #QMAKE_CXXFLAGS += /p:ForceImportBeforeCppTargets="$${libDeskDuplRepoRoot}/prj/common/common_vs/libdeskdupl_hlslconfig.props"

    # Alternative: Use QMAKE_LFLAGS for linker properties
    #QMAKE_LFLAGS += /p:ForceImportAfterCppTargets="$${libDeskDuplRepoRoot}/prj/common/common_vs/libdeskdupl_hlslconfig.props"

    HLSL_PROPS_FILE = $${libDeskDuplRepoRoot}/prj/common/common_vs/libdeskdupl_hlslconfig.props

} else:linux {

    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
    LIBS += -lXfixes
    LIBS += -lX11

}

SOURCES += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.c,true)
SOURCES += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.cpp,true)
SOURCES += $${cinternalRepoRoot}/src/core/cinternal_core_logger.c

HEADERS += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.h,true)
HEADERS += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.hpp,true)
HEADERS += $$files($${libDeskDuplRepoRoot}/include/*.h,true)
HEADERS += $$files($${libDeskDuplRepoRoot}/include/*.hpp,true)

OTHER_FILES += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.mm,true)
