QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

LIBS+=  -lm -fopenmp

## ARMADILLO DEFINITIONS (ONLY FOR FUNCTIONAL RBF)
##DEFINES +=OPTIMUS_ARMADILLO

contains(DEFINES,OPTIMUS_ARMADILLO){
    DEFINES += ADEPT_RECORDING_PAUSABLE ADEPTSTORAGETHREADSAFE
    LIBS+=-larmadillo
    LIBS+=-ladept
}
QMAKE_CXXFLAGS_RELEASE += -O3 -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread
QMAKE_CFLAGS_RELEASE += -O3 -march=native -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native -pthread

QMAKE_CXXFLAGS += -O3  -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread
QMAKE_CFLAGS += -O3 -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread

QMAKE_LFLAGS += -O3
SOURCES += \
        METHODS/adam.cpp \
        METHODS/ipso.cpp \
        METHODS/lbfgs.cpp \
        METHODS/neldermead.cpp \
        METHODS/neuralminimizer.cpp \
        METHODS/parallelde.cpp \
        METHODS/parallelpso.cpp \
        METHODS/simanmethod.cpp \
        MLMODELS/mlpproblem.cpp \
        MLMODELS/model.cpp \
        LINESEARCH/armijosearch.cpp \
        LINESEARCH/fibonaccisearch.cpp \
        LINESEARCH/goldensearch.cpp \
        METHODS/bfgs.cpp \
        METHODS/differentialevolution.cpp \
        METHODS/genetic.cpp \
        METHODS/gradientdescent.cpp \
        LINESEARCH/linesearch.cpp \
        METHODS/multistart.cpp \
        MLMODELS/rbfproblem.cpp \
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
        PROBLEMS/diracproblem.cpp \
        PROBLEMS/easom.cpp \
        PROBLEMS/elp.cpp \
        PROBLEMS/exp.cpp \
        PROBLEMS/fuchss.cpp \
        PROBLEMS/gkls.cpp \
        PROBLEMS/gkls2100.cpp \
        PROBLEMS/gkls250.cpp \
        PROBLEMS/gkls350.cpp \
        PROBLEMS/goldstein.cpp \
        PROBLEMS/griewank10.cpp \
        PROBLEMS/griewank2.cpp \
        PROBLEMS/hansen.cpp \
        PROBLEMS/hartman3.cpp \
        PROBLEMS/hartman6.cpp \
        PROBLEMS/potential.cpp \
        PROBLEMS/rosenbrock.cpp \
        PROBLEMS/shekel10.cpp \
        PROBLEMS/shekel5.cpp \
        PROBLEMS/shekel7.cpp \
        PROBLEMS/sinu.cpp \
        PROBLEMS/test30n.cpp \
        MLMODELS/dataset.cpp \
        PROBLEMS/userproblem.cpp \
        SAMPLER/kmeanssampler.cpp \
        SAMPLER/maxwellsampler.cpp \
        SAMPLER/neuralsampler.cpp \
        SAMPLER/rbfsampler.cpp \
        SAMPLER/triangularsampler.cpp \
        SAMPLER/uniformsampler.cpp \
        main.cpp \
        OPTIMUS/optimizer.cpp \
        OPTIMUS/parameter.cpp \
        OPTIMUS/problem.cpp \
        PROBLEMS/rastriginproblem.cpp \
        PROBLEMS/test2nproblem.cpp \
        SAMPLER/problemsampler.cpp

contains(DEFINES,OPTIMUS_ARMADILLO){
    SOURCES+=MLMODELS/functionalrbf.cpp
}
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    METHODS/adam.h \
    METHODS/ipso.h \
    METHODS/lbfgs.h \
    METHODS/neldermead.h \
    METHODS/neuralminimizer.h \
    METHODS/parallelde.h \
    METHODS/parallelpso.h \
    METHODS/simanmethod.h \
    MLMODELS/mlpproblem.h \
    MLMODELS/model.h \
    LINESEARCH/armijosearch.h \
    LINESEARCH/fibonaccisearch.h \
    LINESEARCH/goldensearch.h \
    LINESEARCH/linesearch.h \
    METHODS/bfgs.h \
    METHODS/differentialevolution.h \
    METHODS/genetic.h \
    METHODS/gradientdescent.h \
    METHODS/multistart.h \
    MLMODELS/rbfproblem.h \
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
    PROBLEMS/diracproblem.h \
    PROBLEMS/easom.h \
    PROBLEMS/elp.h \
    PROBLEMS/exp.h \
    PROBLEMS/fuchss.h \
    PROBLEMS/gkls.h \
    PROBLEMS/gkls2100.h \
    PROBLEMS/gkls250.h \
    PROBLEMS/gkls350.h \
    PROBLEMS/goldstein.h \
    PROBLEMS/griewank10.h \
    PROBLEMS/griewank2.h \
    PROBLEMS/hansen.h \
    PROBLEMS/hartman3.h \
    PROBLEMS/hartman6.h \
    PROBLEMS/potential.h \
    PROBLEMS/rastriginproblem.h \
    PROBLEMS/rosenbrock.h \
    PROBLEMS/shekel10.h \
    PROBLEMS/shekel5.h \
    PROBLEMS/shekel7.h \
    PROBLEMS/sinu.h \
    PROBLEMS/test2nproblem.h \
    PROBLEMS/test30n.h \
    MLMODELS/dataset.h \
    PROBLEMS/userproblem.h \
    SAMPLER/kmeanssampler.h \
    SAMPLER/maxwellsampler.h \
    SAMPLER/neuralsampler.h \
    SAMPLER/problemsampler.h \
    SAMPLER/rbfsampler.h \
    SAMPLER/triangularsampler.h \
    SAMPLER/uniformsampler.h

    contains(DEFINES,OPTIMUS_ARMADILLO){
        HEADERS+=MLMODELS/functionalrbf.h
    }
