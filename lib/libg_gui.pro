TEMPLATE = lib
QT += widgets
CONFIG += G_BUILD
include(../g.pri)
TARGET = $${G_NAME}
LIBS *= -lpcap -lnetfilter_queue
android-g++: LIBS *= -lmnl -lnfnetlink
DESTDIR = $${PWD}/../bin
include(libg_files.pri)
