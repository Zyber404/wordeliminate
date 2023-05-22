QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 \
          exceptions

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += $$PWD/libs/libcomponents.a \
    -L$$PWD/libs -llibcrypto-3-x64

INCLUDEPATH += $$PWD/libs/include

SOURCES += \
    challengeWindow.cpp \
    challenger.cpp \
    loginwindow.cpp \
    main.cpp \
    questionWindow.cpp \
    questioner.cpp \
    searchSort.cpp \
    stage.cpp \
    user.cpp

HEADERS += \
    challengeWindow.h \
    challenger.h \
    loginwindow.h \
    questionWindow.h \
    questioner.h \
    searchSort.h \
    stage.h \
    user.h

FORMS += \
    challengeWindow.ui \
    loginwindow.ui \
    questionWindow.ui \
    searchSort.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    E:/Download/renato-ramos-puma-37WxvlfW3to-unsplash.jpg \
    luminadeer.jpg

RESOURCES += \
    rsc.qrc
