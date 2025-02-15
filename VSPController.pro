QT += core
QT += gui
QT += widgets
QT += network
QT += concurrent
QT += serialbus
QT += serialport
QT += xml

###
CONFIG += c++17
CONFIG += sdk_no_version_check
CONFIG += nostrip
CONFIG += debug
#CONFIG += lrelease
CONFIG += embed_translations
CONFIG += create_prl
CONFIG += app_bundle

QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.2
#QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

VSPBASEDIR = $$PWD/vsplib
VSPLIB_DIR = $$PWD/vsplib
VSPLIB_LIB = VSPController

INCLUDEPATH += $$VSPBASEDIR

SOURCES += \
	main.cpp \
	pgchecks.cpp \
	pgconnect.cpp \
	pglinklist.cpp \
	pglkcreate.cpp \
	pglkremove.cpp \
	pgportlist.cpp \
	pgspcreate.cpp \
	pgspremove.cpp \
	pgtrace.cpp \
	vspcontrollerwindow.cpp \
	vspdatamodel.cpp \
	vspdriverclient.cpp

HEADERS += \
	pgchecks.h \
	pgconnect.h \
	pglinklist.h \
	pglkcreate.h \
	pglkremove.h \
	pgportlist.h \
	pgspcreate.h \
	pgspremove.h \
	pgtrace.h \
	vspcontrollerwindow.h \
	vspdatamodel.h \
	vspdriverclient.h \
	vsplib/VSPController.hpp

FORMS += \
	pgchecks.ui \
	pgconnect.ui \
	pglinklist.ui \
	pglkcreate.ui \
	pglkremove.ui \
	pgportlist.ui \
	pgspcreate.ui \
	pgspremove.ui \
	pgtrace.ui \
	vspcontrollerwindow.ui

TRANSLATIONS += \
	VSPController_en_US.ts

DISTFILES += \
	vsplib/libVSPController.dylib

CONFIG += lrelease
CONFIG += embed_translations
CONFIG += embed_libraries

#otool -L
LIBS += -dead_strip
LIBS += -L$$VSPLIB_DIR -l$$VSPLIB_LIB
LIBS += -liconv

QMAKE_CFLAGS += -mmacosx-version-min=12.0
QMAKE_CXXFLAGS += -mmacosx-version-min=12.0

# Default rules for deployment.
target.path = /usr/local/bin
INSTALLS += target

RESOURCES += \
	vspcontroller.qrc
