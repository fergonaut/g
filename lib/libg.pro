TEMPLATE = lib
QT -= gui
CONFIG += G_BUILD
DEFINES += G_BUILD
include(../g.pri)
TARGET = $${G_NAME}
linux-g++: LIBS += -lpcap -lnetfilter_queue
android-g++: LIBS += -lmnl -lnfnetlink
DESTDIR = $${PWD}/../bin
include(libg-files.pri)
