PHONON = 
exists($$QMAKE_LIBDIR_QT/libphonon.prl) {
    PHONON = 1
}
exists($$QMAKE_LIBDIR_QT/qt4/plugins/phonon_backend/*) {
    PHONON = 1
}
!isEmpty($$PHONON) {
    error(Cannot build this example since it needs Phonon support)
}
CONFIG	    += console debug
QT	    += phonon
HEADERS	    += ../aqp/aqp.hpp
SOURCES	    += ../aqp/aqp.cpp
RESOURCES   += ../aqp/aqp.qrc
INCLUDEPATH += ../aqp
HEADERS	    += treewidgetitem.hpp
HEADERS	    += mainwindow.hpp
SOURCES     += mainwindow.cpp
SOURCES     += main.cpp
RESOURCES   += playmusic.qrc

