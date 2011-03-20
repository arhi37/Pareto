#-------------------------------------------------
#
# Project created by QtCreator 2011-03-19T14:24:24
#
#-------------------------------------------------

QT       += core gui

TARGET = Pareto
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../LabVIEWgrad/clibs/pareto/pareto.cpp \
    ../LabVIEWgrad/clibs/BreakPoint/breakpoint.cpp

HEADERS  += mainwindow.h \
    ../LabVIEWgrad/clibs/pareto/pareto.h \
    ../LabVIEWgrad/clibs/BreakPoint/breakpoint.h

FORMS    += mainwindow.ui

win32 {
	INCLUDEPATH += C:/projects/qwt-5.2.1/src
	LIBS += -LC:/projects/qwt-build-desktop/lib \
		-lqwt5
}
