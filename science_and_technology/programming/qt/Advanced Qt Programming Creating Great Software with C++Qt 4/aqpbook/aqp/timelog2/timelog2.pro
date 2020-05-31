CONFIG	    += console debug
HEADERS	    += ../aqp/kuhn_munkres.hpp
SOURCES	    += ../aqp/kuhn_munkres.cpp
HEADERS	    += ../aqp/alt_key.hpp
SOURCES	    += ../aqp/alt_key.cpp
HEADERS	    += ../aqp/aqp.hpp
SOURCES	    += ../aqp/aqp.cpp
RESOURCES   += ../aqp/aqp.qrc
INCLUDEPATH += ../aqp
HEADERS	    += ../timelog1/richtextlineedit.hpp
SOURCES	    += ../timelog1/richtextlineedit.cpp
HEADERS	    += ../timelog1/richtextdelegate.hpp
SOURCES	    += ../timelog1/richtextdelegate.cpp
HEADERS	    += ../timelog1/global.hpp
HEADERS	    += ../timelog1/mainwindow.hpp
SOURCES     += ../timelog1/mainwindow.cpp
SOURCES     += ../timelog1/main.cpp
RESOURCES   += ../timelog1/timelog.qrc
INCLUDEPATH += ../timelog1
DEFINES	    += CUSTOM_MODEL
HEADERS	    += taskitem.hpp
SOURCES	    += taskitem.cpp
HEADERS	    += treemodel.hpp
SOURCES     += treemodel.cpp
win32 { INCLUDEPATH += . }

debug {
    exists(../modeltest-0.2/modeltest.pri) {
	DEFINES	    += MODEL_TEST
	include(../modeltest-0.2/modeltest.pri)
    }
}
