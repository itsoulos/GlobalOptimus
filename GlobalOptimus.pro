QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        armijosearch.cpp \
        fibonaccisearch.cpp \
        goldensearch.cpp \
        gradientdescent.cpp \
        linesearch.cpp \
        main.cpp \
        optimizer.cpp \
        parameter.cpp \
        problem.cpp \
        rastriginproblem.cpp \
        test2nproblem.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    armijosearch.h \
    fibonaccisearch.h \
    goldensearch.h \
    gradientdescent.h \
    linesearch.h \
    optimizer.h \
    parameter.h \
    problem.h \
    rastriginproblem.h \
    test2nproblem.h
