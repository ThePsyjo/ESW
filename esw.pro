######################################################################
# Automatically generated by qmake (2.01a) Mo Aug 4 21:17:41 2008
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . src
INCLUDEPATH += . src
QT += xml network svg 
#QTPLUGIN += qsvg
OBJECTS_DIR += obj
MOC_DIR += moc
RCC_DIR += moc

# Input
HEADERS += src/window.h \
	   src/configuration.h \
	   src/apiInput.h \
	   src/webDoc.h \
	   src/training.h
SOURCES += src/main.cpp \
           src/window.cpp \
	   src/configuration.cpp \
	   src/apiInput.cpp \
	   src/webDoc.cpp \
	   src/training.cpp

TRANSLATIONS += src/German.ts src/en.ts
RESOURCES += res/res.qrc
