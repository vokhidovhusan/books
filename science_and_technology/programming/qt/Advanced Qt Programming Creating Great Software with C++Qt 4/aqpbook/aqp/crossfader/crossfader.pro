CONFIG	    += console debug
HEADERS	    += ../aqp/kuhn_munkres.hpp
SOURCES	    += ../aqp/kuhn_munkres.cpp
HEADERS	    += ../aqp/alt_key.hpp
SOURCES	    += ../aqp/alt_key.cpp
HEADERS	    += ../aqp/aqp.hpp
SOURCES	    += ../aqp/aqp.cpp
RESOURCES   += ../aqp/aqp.qrc
INCLUDEPATH += ../aqp
HEADERS	    += crossfader.hpp
SOURCES     += crossfader.cpp
HEADERS	    += statusbuttonbar.hpp
SOURCES     += statusbuttonbar.cpp
HEADERS	    += mainwindow.hpp
SOURCES     += mainwindow.cpp
SOURCES     += main.cpp
DEFINES	    += PIXEL=1 SCANLINE=2 BITS=3
DEFINES     += CROSSFADE=BITS
#DEFINES	    += SLOW_STOP
