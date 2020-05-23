TEMPLATE = lib

QT = core network

TARGET = $$qtLibraryTarget(qt-rest)

CONFIG += qdep_build
include(../qt-rest.pri)
CONFIG -= qdep_build

QDEP_EXPORTS += $$QDEP_DEPENDS
!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")

DEFINES += "QTREST_EXPORT=Q_DECL_EXPORT"
