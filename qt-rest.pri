CONFIG += c++17 exceptions

HEADERS += \
	$$PWD/src/contenthandler.h \
	$$PWD/src/jsoncontenthandler.h \
	$$PWD/src/restbuilder.h \
	$$PWD/src/restbuilder_p.h

SOURCES += \
	$$PWD/src/jsoncontenthandler.cpp \
	$$PWD/src/restbuilder.cpp

INCLUDEPATH += $$PWD/src

QDEP_PACKAGE_EXPORTS += QTREST_EXPORT
!qdep_build: DEFINES += "QTREST_EXPORT="
QDEP_DEPENDS += Skycoder42/qt-json@1.0.0
