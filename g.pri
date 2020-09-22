#------------------------------------------------------------------------------
# c++11
#------------------------------------------------------------------------------
CONFIG *= c++11
linux: QMAKE_LFLAGS *= -pthread

#------------------------------------------------------------------------------
# debug and release
#------------------------------------------------------------------------------
CONFIG(debug, debug|release) DEFINES *= _DEBUG
CONFIG(release, debug|release) DEFINES *= _RELEASE
DEFINES *= QT_MESSAGELOGCONTEXT

#------------------------------------------------------------------------------
# G_NAME
#------------------------------------------------------------------------------
G_NAME = g
android-g++:                  G_NAME = $${G_NAME}-android
CONFIG(qt): contains(QT, gui) G_NAME = $${G_NAME}-gui
CONFIG(debug, debug|release)  G_NAME = $${G_NAME}-d

#------------------------------------------------------------------------------
# G_DIR
#------------------------------------------------------------------------------
G_DIR = $${PWD}
INCLUDEPATH *= $${G_DIR}/src
!CONFIG(G_BUILD) {
	win32: PRE_TARGETDEPS *= $${G_DIR}/bin/$${G_NAME}.dll
	linux: PRE_TARGETDEPS *= $${G_DIR}/bin/lib$${G_NAME}.so
	LIBS *= -L$${G_DIR}/bin -l$${G_NAME}
	ANDROID_EXTRA_LIBS *= $${G_DIR}/bin/lib$${G_NAME}.so
}

#------------------------------------------------------------------------------
# mingw
#------------------------------------------------------------------------------
mingw: DEFINES *= __USE_MINGW_ANSI_STDIO=1

#------------------------------------------------------------------------------
# rpath
#------------------------------------------------------------------------------
QMAKE_RPATHDIR *= . $${PWD}/bin

#------------------------------------------------------------------------------
# gstacktrace
#------------------------------------------------------------------------------
CONFIG(gstacktrace) {
	QMAKE_LFLAGS *= -rdynamic
}

#------------------------------------------------------------------------------
# pcap
#------------------------------------------------------------------------------
win32 {
	INCLUDEPATH *= $${PWD}/../npcap/Include
	contains(QMAKE_TARGET.arch, x86_64) { # 64 bit
		LIBS *= -L$${PWD}/../npcap/Lib/x64
	} else { # 32 bit
		LIBS *= -L$${PWD}/../npcap/Lib
	}
	LIBS *= -lpacket -lwpcap -lws2_32
}
