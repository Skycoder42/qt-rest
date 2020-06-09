CONFIG += c++17 exceptions

HEADERS += \
	$$PWD/src/contenthandler.h \
	$$PWD/src/jsoncontenthandler.h \
	$$PWD/src/qtrest_exceptions.h \
	$$PWD/src/qtrest_global.h \
	$$PWD/src/restbuilder.h \
	$$PWD/src/restbuilder_data.h \
	$$PWD/src/restbuilder_decl.h \
	$$PWD/src/restbuilder_impl.h \
	$$PWD/src/restreply.h

SOURCES += \
	$$PWD/src/jsoncontenthandler.cpp \
	$$PWD/src/qtrest_exceptions.cpp \
	$$PWD/src/restbuilder.cpp \
	$$PWD/src/restreply.cpp

INCLUDEPATH += $$PWD/src

QDEP_PACKAGE_EXPORTS += QTREST_EXPORT
!qdep_build: DEFINES += "QTREST_EXPORT="
QDEP_DEPENDS += Skycoder42/qt-json@1.0.0
