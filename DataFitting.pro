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
        GE/cprogram.cc \
        GE/doublestack.cc \
        GE/fparser.cc \
        GE/fpoptimizer.cc \
        GE/nncprogram.cpp \
        GE/population.cc \
        GE/program.cc \
        GE/rule.cc \
        GE/symbol.cc \
        METHODS/adam.cpp \
        METHODS/ipso.cpp \
        METHODS/lbfgs.cpp \
        METHODS/neldermead.cpp \
        METHODS/neuralminimizer.cpp \
        METHODS/parallelde.cpp \
        METHODS/parallelpso.cpp \
        METHODS/simanmethod.cpp \
        METHODS/usermethod.cpp \
        MLMODELS/converter.cc \
        MLMODELS/gdfmodel.cpp \
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
        MLMODELS/neuralparser.cc \
        MLMODELS/nncmodel.cpp \
        MLMODELS/rbfproblem.cpp \
        OPTIMUS/parameterlist.cpp \
        OPTIMUS/statistics.cpp \
        OPTIMUS/collection.cpp \
        OPTIMUS/doublebox.cpp \
        OPTIMUS/similarity.cpp \
        MLMODELS/dataset.cpp \
        PROBLEMS/userproblem.cpp \
        SAMPLER/kmeanssampler.cpp \
        SAMPLER/maxwellsampler.cpp \
        SAMPLER/neuralsampler.cpp \
        SAMPLER/rbfsampler.cpp \
        SAMPLER/triangularsampler.cpp \
        SAMPLER/uniformsampler.cpp \
        datafitting.cpp \
        OPTIMUS/optimizer.cpp \
        OPTIMUS/parameter.cpp \
        OPTIMUS/problem.cpp \
        SAMPLER/problemsampler.cpp

contains(DEFINES,OPTIMUS_ARMADILLO){
    SOURCES+=MLMODELS/functionalrbf.cpp
}
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    GE/cprogram.h \
    GE/doublestack.h \
    GE/fparser.hh \
    GE/fpconfig.hh \
    GE/fptypes.hh \
    GE/nncprogram.h \
    GE/population.h \
    GE/program.h \
    GE/rule.h \
    GE/symbol.h \
    METHODS/adam.h \
    METHODS/ipso.h \
    METHODS/lbfgs.h \
    METHODS/neldermead.h \
    METHODS/neuralminimizer.h \
    METHODS/parallelde.h \
    METHODS/parallelpso.h \
    METHODS/simanmethod.h \
    METHODS/usermethod.h \
    MLMODELS/converter.h \
    MLMODELS/gdfmodel.h \
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
    MLMODELS/neuralparser.h \
    MLMODELS/nncmodel.h \
    MLMODELS/rbfproblem.h \
    OPTIMUS/parameterlist.h \
    OPTIMUS/statistics.h \
    OPTIMUS/collection.h \
    OPTIMUS/doublebox.h \
    OPTIMUS/optimizer.h \
    OPTIMUS/parameter.h \
    OPTIMUS/problem.h \
    OPTIMUS/similarity.h \
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
