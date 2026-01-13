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

#QT -= core
#QT -= gui
#CONFIG -= qt

#DEFINES += CINTERNAL_LOAD_FROM_DLL
#LIBS += -lcinternal

win32 {

    #LIBS += -lWtsapi32
    #LIBS += -lUser32

} else:linux {

    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"

}

SOURCES += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.c,true)
SOURCES += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.cpp,true)
SOURCES += $${cinternalRepoRoot}/src/core/cinternal_core_logger.c

HEADERS += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.h,true)
HEADERS += $$files($${libDeskDuplRepoRoot}/src/core/libdesktop_duplication/*.hpp,true)
HEADERS += $$files($${libDeskDuplRepoRoot}/include/*.h,true)
HEADERS += $$files($${libDeskDuplRepoRoot}/include/*.hpp,true)

#OTHER_FILES += "$${systemMonitorRepoRoot}/scripts/unix_build_core_lib.sh"
#OTHER_FILES += "$${systemMonitorRepoRoot}/scripts/unix_prepare_files_in_dir.sh"
#OTHER_FILES += "$${systemMonitorRepoRoot}/scripts/windows_build_core_lib.bat"
#OTHER_FILES += "$${systemMonitorRepoRoot}/scripts/windows_prepare_files_in_dir.bat"
