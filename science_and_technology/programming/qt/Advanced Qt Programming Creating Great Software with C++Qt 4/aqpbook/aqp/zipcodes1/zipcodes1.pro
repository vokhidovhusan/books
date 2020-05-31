CONFIG	     += console debug
HEADERS	     += ../aqp/kuhn_munkres.hpp
SOURCES	     += ../aqp/kuhn_munkres.cpp
HEADERS	     += ../aqp/alt_key.hpp
SOURCES	     += ../aqp/alt_key.cpp
HEADERS	     += ../aqp/aqp.hpp
SOURCES	     += ../aqp/aqp.cpp
RESOURCES    += ../aqp/aqp.qrc
INCLUDEPATH  += ../aqp
HEADERS	     += global.hpp
HEADERS	     += zipcodespinbox.hpp
HEADERS	     += itemdelegate.hpp
SOURCES	     += itemdelegate.cpp
HEADERS	     += standardtablemodel.hpp
SOURCES      += standardtablemodel.cpp
HEADERS	     += proxymodel.hpp
SOURCES	     += proxymodel.cpp
HEADERS	     += uniqueproxymodel.hpp
SOURCES	     += uniqueproxymodel.cpp
HEADERS	     += mainwindow.hpp
SOURCES      += mainwindow.cpp
SOURCES      += main.cpp
TRANSLATIONS += zipcodes_en.ts

debug {
    exists(../modeltest-0.2/modeltest.pri) {
	DEFINES += MODEL_TEST
	include(../modeltest-0.2/modeltest.pri)
    }
}
