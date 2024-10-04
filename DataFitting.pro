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
        GE/fparser.cc \
        GE/fpoptimizer.cc \
        GE/population.cc \
        GE/program.cc \
        GE/rule.cc \
        GE/symbol.cc \
	GE/integeranneal.cpp \
	GE/nncprogram.cpp \
	GE/ruleprogram.cpp \
    METHODS/hybridmethod.cpp \
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
        OPTIMUS/parameterlist.cpp \
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
        PROBLEMS/fuch1.cpp \
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
        PROBLEMS/hess.cpp \
        PROBLEMS/levy.cpp \
        PROBLEMS/potential.cpp \
        PROBLEMS/rosenbrock.cpp \
        PROBLEMS/salkin.cpp \
        PROBLEMS/shekel10.cpp \
        PROBLEMS/shekel5.cpp \
	PROBLEMS/gross.cpp\
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
        datafitting.cpp \
        OPTIMUS/optimizer.cpp \
	PROBLEMS/discus.cpp \
	PROBLEMS/cigar.cpp \
        OPTIMUS/parameter.cpp \
        OPTIMUS/problem.cpp \
	MLMODELS/converter.cc \
	MLMODELS/neuralparser.cc \
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
    GE/cprogram.h \
    GE/doublestack.h \
    GE/fparser.hh \
    GE/fpconfig.hh \
    GE/fptypes.hh \
    GE/population.h \
    GE/program.h \
    GE/rule.h \
    GE/symbol.h \
    METHODS/hybridmethod.h \
    OPTIMUS/editlogger.h \
    OPTIMUS/filelogger.h \
    OPTIMUS/logger.h \
    OPTIMUS/mean.h \
    PROBLEMS/discus.h \
    PROBLEMS/cigar.h \
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
    PROBLEMS/fuch1.h \
    PROBLEMS/fuchss.h \
    PROBLEMS/gkls.h \
    PROBLEMS/gkls2100.h \
    PROBLEMS/gkls250.h \
    PROBLEMS/gkls350.h \
    PROBLEMS/gross.h \
    PROBLEMS/goldstein.h \
    PROBLEMS/griewank10.h \
    PROBLEMS/griewank2.h \
    PROBLEMS/hansen.h \
    PROBLEMS/hartman3.h \
    PROBLEMS/hartman6.h \
    PROBLEMS/hess.h \
    PROBLEMS/levy.h \
    PROBLEMS/potential.h \
    PROBLEMS/rastriginproblem.h \
    PROBLEMS/rosenbrock.h \
    PROBLEMS/salkin.h \
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
    MLMODELS/converter.h \
	MLMODELS/neuralparser.h \
    SAMPLER/uniformsampler.h

    contains(DEFINES,OPTIMUS_ARMADILLO){
        HEADERS+=MLMODELS/functionalrbf.h
    }
