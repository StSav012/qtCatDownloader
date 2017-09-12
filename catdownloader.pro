#-------------------------------------------------
#
# Project created by QtCreator 2017-04-20T09:19:10
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = catdownloader
TEMPLATE = app


SOURCES += main.cpp\
		mainwindow.cpp \
	downloader.cpp \
	catalogentry.cpp

HEADERS  += mainwindow.h \
	downloader.h \
	catalogentry.h

FORMS    += mainwindow.ui

RESOURCES += \
	resource.qrc

win32 {
	QMAKE_TARGET_COMPANY = SavSoft
	RC_FILE += resource.rc
}

VERSION = 1.0.0

# convert icon1.png -define icon:auto-resize=16,32,48,64,128,256 icon1.ico
