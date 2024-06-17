QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

## ARMADILLO DEFINITIONS (ONLY FOR FUNCTIONAL RBF)
##DEFINES +=OPTIMUS_ARMADILLO

contains(DEFINES,OPTIMUS_ARMADILLO){
    DEFINES += ADEPT_RECORDING_PAUSABLE ADEPTSTORAGETHREADSAFE
    LIBS+=-larmadillo
    LIBS+=-ladept
}

unix{
    macx{
        INCLUDEPATH+=/opt/homebrew/opt/libomp/include/
        QMAKE_CXXFLAGS_RELEASE += -O3  -Xpreprocessor  -fopenmp -unroll-loops -fomit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread
        QMAKE_CFLAGS_RELEASE += -O3 -march=native   -Xpreprocessor  -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native -pthread
        LIBS+=  -lm  -Xpreprocessor  -fopenmp -L/opt/homebrew/opt/libomp/lib/  -lomp
    }
    else
    {
        QMAKE_CXXFLAGS_RELEASE += -O3 -fopenmp -unroll-loops -fomit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread
        QMAKE_CFLAGS_RELEASE += -O3 -march=native -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native -pthread
        LIBS+=  -lm -fopenmp

    }
}
win32{
    QMAKE_CXXFLAGS_RELEASE += -O3 -fopenmp -unroll-loops -fomit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread
    QMAKE_CFLAGS_RELEASE += -O3 -march=native -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native -pthread
    LIBS+=  -lm -fopenmp
}
QMAKE_LFLAGS += -O3
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GE/cprogram.cc \
    GE/doublestack.cc \
    GE/fparser.cc \
    GE/fpoptimizer.cc \
    GE/integeranneal.cpp \
    GE/nncprogram.cpp \
    GE/population.cc \
    GE/program.cc \
    GE/rule.cc \
    GE/ruleprogram.cpp \
    GE/symbol.cc \
    LINESEARCH/armijosearch.cpp \
    LINESEARCH/fibonaccisearch.cpp \
    LINESEARCH/goldensearch.cpp \
    LINESEARCH/linesearch.cpp \
    METHODS/adam.cpp \
    METHODS/armadillo1.cpp \
    METHODS/bfgs.cpp \
    METHODS/differentialevolution.cpp \
    METHODS/genetic.cpp \
    METHODS/gradientdescent.cpp \
    METHODS/gwooptimizer.cpp \
    METHODS/ipso.cpp \
    METHODS/lbfgs.cpp \
    METHODS/multistart.cpp \
    METHODS/neldermead.cpp \
    METHODS/neuralminimizer.cpp \
    METHODS/ofalgorithm.cpp \
    METHODS/parallelde.cpp \
    METHODS/parallelpso.cpp \
    METHODS/simanmethod.cpp \
    METHODS/usermethod.cpp \
    MLMODELS/converter.cc \
    MLMODELS/dataset.cpp \
    MLMODELS/gdfmodel.cpp \
    MLMODELS/mlpproblem.cpp \
    MLMODELS/model.cpp \
    MLMODELS/neuralparser.cc \
    MLMODELS/nncmodel.cpp \
    MLMODELS/rbfproblem.cpp \
    MLMODELS/rulemodel.cpp \
    OPTIMUS/collection.cpp \
    OPTIMUS/doublebox.cpp \
    OPTIMUS/optimizer.cpp \
    OPTIMUS/parameter.cpp \
    OPTIMUS/parameterlist.cpp \
    OPTIMUS/problem.cpp \
    XOPTIMUS/methodloader.cpp \
    XOPTIMUS/parameterdialog.cpp \
    XOPTIMUS/problemloader.cpp \
    OPTIMUS/similarity.cpp \
    OPTIMUS/statistics.cpp \
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
    PROBLEMS/gross.cpp \
    PROBLEMS/hansen.cpp \
    PROBLEMS/hartman3.cpp \
    PROBLEMS/hartman6.cpp \
    PROBLEMS/hess.cpp \
    PROBLEMS/levy.cpp \
    PROBLEMS/potential.cpp \
    PROBLEMS/rastriginproblem.cpp \
    PROBLEMS/rosenbrock.cpp \
    PROBLEMS/salkin.cpp \
    PROBLEMS/shekel10.cpp \
    PROBLEMS/shekel5.cpp \
    PROBLEMS/shekel7.cpp \
    PROBLEMS/sinu.cpp \
    PROBLEMS/test2nproblem.cpp \
    PROBLEMS/test30n.cpp \
    PROBLEMS/userproblem.cpp \
    SAMPLER/kmeanssampler.cpp \
    SAMPLER/maxwellsampler.cpp \
    SAMPLER/neuralsampler.cpp \
    SAMPLER/problemsampler.cpp \
    SAMPLER/rbfsampler.cpp \
    SAMPLER/triangularsampler.cpp \
    SAMPLER/uniformsampler.cpp \
    XOPTIMUS/selectmethoddialog.cpp \
    XOPTIMUS/selectproblemdialog.cpp \
    XOPTIMUS/xoptimusmain.cpp \
    XOPTIMUS/mainwindow.cpp
    contains(DEFINES,OPTIMUS_ARMADILLO){
        SOURCES+=MLMODELS/functionalrbf.cpp
    }
HEADERS += \
    GE/cprogram.h \
    GE/doublestack.h \
    GE/fparser.hh \
    GE/fpconfig.hh \
    GE/fptypes.hh \
    GE/integeranneal.h \
    GE/nncprogram.h \
    GE/population.h \
    GE/program.h \
    GE/rule.h \
    GE/ruleprogram.h \
    GE/symbol.h \
    LINESEARCH/armijosearch.h \
    LINESEARCH/fibonaccisearch.h \
    LINESEARCH/goldensearch.h \
    LINESEARCH/linesearch.h \
    METHODS/adam.h \
    METHODS/armadillo1.h \
    METHODS/bfgs.h \
    METHODS/differentialevolution.h \
    METHODS/genetic.h \
    METHODS/gradientdescent.h \
    METHODS/gwooptimizer.h \
    METHODS/ipso.h \
    METHODS/lbfgs.h \
    METHODS/multistart.h \
    METHODS/neldermead.h \
    METHODS/neuralminimizer.h \
    METHODS/ofalgorithm.h \
    METHODS/parallelde.h \
    METHODS/parallelpso.h \
    METHODS/simanmethod.h \
    METHODS/usermethod.h \
    MLMODELS/converter.h \
    MLMODELS/dataset.h \
    MLMODELS/functionalrbf.h \
    MLMODELS/gdfmodel.h \
    MLMODELS/mlpproblem.h \
    MLMODELS/model.h \
    MLMODELS/neuralparser.h \
    MLMODELS/nncmodel.h \
    MLMODELS/rbfproblem.h \
    MLMODELS/rulemodel.h \
    OPTIMUS/collection.h \
    OPTIMUS/doublebox.h \
    OPTIMUS/optimizer.h \
    OPTIMUS/parameter.h \
    OPTIMUS/parameterlist.h \
    OPTIMUS/problem.h \
    XOPTIMUS/methodloader.h \
    XOPTIMUS/parameterdialog.h \
    XOPTIMUS/problemloader.h \
    OPTIMUS/similarity.h \
    OPTIMUS/statistics.h \
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
    PROBLEMS/goldstein.h \
    PROBLEMS/griewank10.h \
    PROBLEMS/griewank2.h \
    PROBLEMS/gross.h \
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
    PROBLEMS/userproblem.h \
    SAMPLER/kmeanssampler.h \
    SAMPLER/maxwellsampler.h \
    SAMPLER/neuralsampler.h \
    SAMPLER/problemsampler.h \
    SAMPLER/rbfsampler.h \
    SAMPLER/triangularsampler.h \
    SAMPLER/uniformsampler.h \
    XOPTIMUS/mainwindow.h \
    XOPTIMUS/selectmethoddialog.h \
    XOPTIMUS/selectproblemdialog.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
