#   
# file:		libapp_and_browser_monitor.pro
# path:		prj/core/libapp_and_browser_monitor_qt/libapp_and_browser_monitor.pro
# created on:	2010 May 27 
# created by:	D. Kalantaryan (davit.kalantaryan@gmail.com)  
#  

message("!!! $${_PRO_FILE_}")
include ( "$${PWD}/../../common/common_qt/desktop_duplication_inc.pri" )

SOURCES += $${libDeskDuplRepoRoot}/src/tests/main_deskdupl_test01.cpp
SOURCES += $${cinternalRepoRoot}/src/core/cinternal_core_logger.c
