QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        LINESEARCH/armijosearch.cpp \
        LINESEARCH/fibonaccisearch.cpp \
        LINESEARCH/goldensearch.cpp \
        METHODS/bfgs.cpp \
        METHODS/gradientdescent.cpp \
        LINESEARCH/linesearch.cpp \
        METHODS/multistart.cpp \
        OPTIMUS/collection.cpp \
        OPTIMUS/doublebox.cpp \
        main.cpp \
        OPTIMUS/optimizer.cpp \
        OPTIMUS/parameter.cpp \
        OPTIMUS/problem.cpp \
        PROBLEMS/rastriginproblem.cpp \
        PROBLEMS/test2nproblem.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    LINESEARCH/armijosearch.h \
    LINESEARCH/fibonaccisearch.h \
    LINESEARCH/goldensearch.h \
    LINESEARCH/linesearch.h \
    METHODS/bfgs.h \
    METHODS/gradientdescent.h \
    METHODS/multistart.h \
    OPTIMUS/collection.h \
    OPTIMUS/doublebox.h \
    OPTIMUS/optimizer.h \
    OPTIMUS/parameter.h \
    OPTIMUS/problem.h \
    PROBLEMS/rastriginproblem.h \
    PROBLEMS/test2nproblem.h
