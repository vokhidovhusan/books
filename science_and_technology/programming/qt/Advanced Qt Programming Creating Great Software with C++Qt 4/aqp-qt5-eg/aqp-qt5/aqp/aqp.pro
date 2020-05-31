CONFIG	    += console debug
HEADERS	    += aqp.hpp
SOURCES     += aqp.cpp
SOURCES     += main.cpp
RESOURCES   += aqp.qrc
win32-msvc {
    DEFINES += _USE_MATH_DEFINES
}
