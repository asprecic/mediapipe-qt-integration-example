QT += quick qml multimedia

CONFIG += c++11
CONFIG += qmltypes
QML_IMPORT_NAME = io.qt.examples.backend
QML_IMPORT_MAJOR_VERSION = 1

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        graphs.cpp \
        imagerenderer.cpp \
        main.cpp \
        mpintegrationexample.cpp

RESOURCES += qml.qrc

exists(user.pri) {
include(user.pri)
}

## Define EXAMPLE_LIB_PATH in user.pri
## or uncomment this
#EXAMPLE_LIB_PATH = /example-lib

!defined(EXAMPLE_LIB_PATH,var) {
error(Missing EXAMPLE_LIB_PATH variable.)
}

INCLUDEPATH += $$EXAMPLE_LIB_PATH/include
LIBS += -L$$EXAMPLE_LIB_PATH/lib -lexample

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    graphs.h \
    imagerenderer.h \
    mpintegrationexample.h

DISTFILES +=
