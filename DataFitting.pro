QT -= gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
    GE/fcprogram.cpp \
        GE/fparser.cc \
        GE/fpoptimizer.cc \
    GE/mapper.cc \
        GE/population.cc \
        GE/program.cc \
        GE/rule.cc \
        GE/symbol.cc \
	GE/integeranneal.cpp \
	GE/nncprogram.cpp \
	GE/ruleprogram.cpp \
    METHODS/ego.cpp \
    METHODS/hybridmethod.cpp \
    METHODS/jDElsgo.cpp \
    METHODS/mewoa.cpp \
    METHODS/woa.cpp \
    MLMODELS/fcmodel.cpp \
    OPTIMUS/editlogger.cpp \
    OPTIMUS/filelogger.cpp \
    OPTIMUS/logger.cpp \
	OPTIMUS/mean.cpp \
    	METHODS/gwooptimizer.cpp \
	METHODS/ofalgorithm.cpp \
        METHODS/adam.cpp \
	METHODS/armadillo1.cpp \
        METHODS/ipso.cpp \
        METHODS/lbfgs.cpp \
        METHODS/neldermead.cpp \
        METHODS/neuralminimizer.cpp \
        METHODS/parallelde.cpp \
        METHODS/parallelpso.cpp \
        METHODS/simanmethod.cpp \
        METHODS/usermethod.cpp \
        MLMODELS/gdfmodel.cpp \
        MLMODELS/nncmodel.cpp \
        MLMODELS/rulemodel.cpp \
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
    OPTIMUS/methodloader.cpp \
        OPTIMUS/parameterlist.cpp \
        OPTIMUS/statistics.cpp \
        OPTIMUS/collection.cpp \
        OPTIMUS/doublebox.cpp \
        OPTIMUS/similarity.cpp \
        MLMODELS/dataset.cpp \
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
	MLMODELS/converter.cc \
	MLMODELS/neuralparser.cc \
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
    GE/fcprogram.h \
    GE/fparser.hh \
    GE/fpconfig.hh \
    GE/fptypes.hh \
    GE/mapper.h \
    GE/population.h \
    GE/program.h \
    GE/rule.h \
    GE/symbol.h \
    METHODS/ego.h \
    METHODS/hybridmethod.h \
    METHODS/jDElsgo.h \
    METHODS/mewoa.h \
    METHODS/woa.h \
    MLMODELS/fcmodel.h \
    OPTIMUS/editlogger.h \
    OPTIMUS/filelogger.h \
    OPTIMUS/logger.h \
    OPTIMUS/mean.h \
    OPTIMUS/methodloader.h \
    GE/integeranneal.h\
    GE/nncprogram.h \
    GE/ruleprogram.h \
    METHODS/adam.h \
    METHODS/gwooptimizer.h \
    METHODS/ipso.h \
    METHODS/lbfgs.h \
    METHODS/neldermead.h \
    METHODS/neuralminimizer.h \
    METHODS/parallelde.h \
    METHODS/parallelpso.h \
    METHODS/simanmethod.h \
    METHODS/ofalgorithm.h \
    METHODS/usermethod.h \
    METHODS/armadillo1.h\
    MLMODELS/gdfmodel.h \
    MLMODELS/rulemodel.h \
    MLMODELS/nncmodel.h \
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
    OPTIMUS/parameterlist.h \
    OPTIMUS/statistics.h \
    OPTIMUS/collection.h \
    OPTIMUS/doublebox.h \
    OPTIMUS/optimizer.h \
    OPTIMUS/parameter.h \
    OPTIMUS/problem.h \
    OPTIMUS/similarity.h \
    MLMODELS/dataset.h \
    SAMPLER/kmeanssampler.h \
    SAMPLER/maxwellsampler.h \
    SAMPLER/neuralsampler.h \
    SAMPLER/problemsampler.h \
    SAMPLER/rbfsampler.h \
    SAMPLER/triangularsampler.h \
    MLMODELS/converter.h \
	MLMODELS/neuralparser.h \
    SAMPLER/uniformsampler.h

    contains(DEFINES,OPTIMUS_ARMADILLO){
        HEADERS+=MLMODELS/functionalrbf.h
    }
