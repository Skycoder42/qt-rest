CONFIG += c++17 exceptions

HEADERS +=

SOURCES +=

INCLUDEPATH += $$PWD/src

QDEP_PACKAGE_EXPORTS += QTREST_EXPORT
!qdep_build: DEFINES += "QTREST_EXPORT="
QDEP_DEPENDS += Skycoder42/qt-json@1.0.0
