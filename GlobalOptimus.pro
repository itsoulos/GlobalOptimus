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
        METHODS/differentialevolution.cpp \
        METHODS/genetic.cpp \
        METHODS/gradientdescent.cpp \
        LINESEARCH/linesearch.cpp \
        METHODS/multistart.cpp \
        OPTIMUS/statistics.cpp \
        OPTIMUS/collection.cpp \
        OPTIMUS/doublebox.cpp \
        OPTIMUS/similarity.cpp \
        PROBLEMS/bf1.cpp \
        PROBLEMS/bf2.cpp \
        PROBLEMS/branin.cpp \
        PROBLEMS/camel.cpp \
        PROBLEMS/cm.cpp \
        PROBLEMS/diffpower.cpp \
        PROBLEMS/easom.cpp \
        PROBLEMS/elp.cpp \
        PROBLEMS/exp.cpp \
        PROBLEMS/gkls.cpp \
        PROBLEMS/gkls2100.cpp \
        PROBLEMS/gkls250.cpp \
        PROBLEMS/gkls350.cpp \
        PROBLEMS/goldstein.cpp \
        PROBLEMS/griewank10.cpp \
        PROBLEMS/griewank2.cpp \
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
    METHODS/differentialevolution.h \
    METHODS/genetic.h \
    METHODS/gradientdescent.h \
    METHODS/multistart.h \
    OPTIMUS/statistics.h \
    OPTIMUS/collection.h \
    OPTIMUS/doublebox.h \
    OPTIMUS/optimizer.h \
    OPTIMUS/parameter.h \
    OPTIMUS/problem.h \
    OPTIMUS/similarity.h \
    PROBLEMS/bf1.h \
    PROBLEMS/bf2.h \
    PROBLEMS/branin.h \
    PROBLEMS/camel.h \
    PROBLEMS/cm.h \
    PROBLEMS/diffpower.h \
    PROBLEMS/easom.h \
    PROBLEMS/elp.h \
    PROBLEMS/exp.h \
    PROBLEMS/gkls.h \
    PROBLEMS/gkls2100.h \
    PROBLEMS/gkls250.h \
    PROBLEMS/gkls350.h \
    PROBLEMS/goldstein.h \
    PROBLEMS/griewank10.h \
    PROBLEMS/griewank2.h \
    PROBLEMS/rastriginproblem.h \
    PROBLEMS/test2nproblem.h
