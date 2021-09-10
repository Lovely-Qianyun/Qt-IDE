QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = notepad
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_NO_WARNING_OUTPUT
DEFINES += QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    codeeditor.cpp \
    main.cpp \
    mainwindow.cpp \
    finddialog.cpp \
    replacedialog.cpp \
    qmytextedit.cpp \
    color.cpp

HEADERS += \
    codeeditor.h \
    mainwindow.h \
    finddialog.h \
    replacedialog.h \
    qmytextedit.h \
    color.h

FORMS += \
    mainwindow.ui \
    finddialog.ui \
    replacedialog.ui \
    gotodialog.ui


RESOURCES += \
    images.qrc \
    notepad.qrc \
    qm.qrc
