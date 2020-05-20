TEMPLATE = lib

QT = core network

TARGET = $$qtLibraryTarget(qt-rest)

include(../qt-rest.pri)

DEFINES += "QTREST_EXPORT=Q_DECL_EXPORT"

QDEP_EXPORTS += $$QDEP_DEPENDS
!load(qdep):error("Failed to load qdep feature! Run 'qdep prfgen --qmake $$QMAKE_QMAKE' to create it.")
