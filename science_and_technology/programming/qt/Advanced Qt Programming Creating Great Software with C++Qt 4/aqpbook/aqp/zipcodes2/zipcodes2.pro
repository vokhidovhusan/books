CONFIG	     += console debug
HEADERS	     += ../aqp/kuhn_munkres.hpp
SOURCES	     += ../aqp/kuhn_munkres.cpp
HEADERS	     += ../aqp/alt_key.hpp
SOURCES	     += ../aqp/alt_key.cpp
HEADERS	     += ../aqp/aqp.hpp
SOURCES	     += ../aqp/aqp.cpp
RESOURCES    += ../aqp/aqp.qrc
INCLUDEPATH  += ../aqp
HEADERS	     += ../zipcodes1/global.hpp
HEADERS	     += ../zipcodes1/zipcodespinbox.hpp
HEADERS	     += ../zipcodes1/itemdelegate.hpp
SOURCES	     += ../zipcodes1/itemdelegate.cpp
HEADERS	     += ../zipcodes1/proxymodel.hpp
SOURCES	     += ../zipcodes1/proxymodel.cpp
HEADERS	     += ../zipcodes1/uniqueproxymodel.hpp
SOURCES	     += ../zipcodes1/uniqueproxymodel.cpp
HEADERS	     += ../zipcodes1/mainwindow.hpp
SOURCES      += ../zipcodes1/mainwindow.cpp
SOURCES      += ../zipcodes1/main.cpp
TRANSLATIONS += ../zipcodes1/zipcodes_en.ts
INCLUDEPATH  += ../zipcodes1
DEFINES	     += CUSTOM_MODEL
HEADERS	     += zipcodeitem.hpp
HEADERS	     += tablemodel.hpp
SOURCES      += tablemodel.cpp
win32 { INCLUDEPATH += . }

debug {
exists(../modeltest-0.2/modeltest.pri) {
    DEFINES += MODEL_TEST
    include(../modeltest-0.2/modeltest.pri)
}
}
