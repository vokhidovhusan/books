CONFIG	    += console debug
HEADERS	    += ../aqp/kuhn_munkres.hpp
SOURCES	    += ../aqp/kuhn_munkres.cpp
HEADERS	    += ../aqp/alt_key.hpp
SOURCES	    += ../aqp/alt_key.cpp
HEADERS	    += ../aqp/aqp.hpp
SOURCES	    += ../aqp/aqp.cpp
RESOURCES   += ../aqp/aqp.qrc
INCLUDEPATH += ../aqp
HEADERS	    += global.hpp
HEADERS	    += richtextlineedit.hpp
SOURCES	    += richtextlineedit.cpp
HEADERS	    += richtextdelegate.hpp
SOURCES	    += richtextdelegate.cpp
HEADERS	    += standarditem.hpp
SOURCES	    += standarditem.cpp
HEADERS	    += standardtreemodel.hpp
SOURCES     += standardtreemodel.cpp
HEADERS	    += mainwindow.hpp
SOURCES     += mainwindow.cpp
SOURCES     += main.cpp
RESOURCES   += timelog.qrc

debug {
    exists(../modeltest-0.2/modeltest.pri) {
	DEFINES	    += MODEL_TEST
	include(../modeltest-0.2/modeltest.pri)
    }
}
