#
# repo:		lib_desktop_duplication
# name:		flagsandsys_common.pri
# path:		prj/common/common_qt/flagsandsys_common.pri
# created on:   2023 Jun 21
# created by:   Davit Kalantaryan (davit.kalantaryan@desy.de)
# usage:	Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/flagsandsys_common.pri")

isEmpty(libDeskDuplFlagsAndSysCommonIncluded){
    # libDeskDuplRepoRoot
    libDeskDuplFlagsAndSysCommonIncluded = 1

    libDeskDuplRepoRoot = $${PWD}/../../..

    isEmpty(artifactRoot) {
        artifactRoot = $$(artifactRoot)
        isEmpty(artifactRoot) {
            artifactRoot = $${libDeskDuplRepoRoot}
        }
    }

    include("$${libDeskDuplRepoRoot}/contrib/qtutils/prj/common/common_qt/flagsandsys_common.pri")

    INCLUDEPATH += $${libDeskDuplRepoRoot}/include

    LIBS	+= -L$${libDeskDuplRepoRoot}/sys/$${CODENAME}/$$CONFIGURATION/lib
    LIBS	+= -L$${libDeskDuplRepoRoot}/sys/$${CODENAME}/$$CONFIGURATION/tlib

    OTHER_FILES += $$files($${PWD}/../common_mkfl/*.Makefile,true)
}
