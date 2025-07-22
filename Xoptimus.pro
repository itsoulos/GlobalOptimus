QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

## ARMADILLO DEFINITIONS (ONLY FOR FUNCTIONAL RBF)
##DEFINES +=OPTIMUS_ARMADILLO

contains(DEFINES,OPTIMUS_ARMADILLO){
    DEFINES += ADEPT_RECORDING_PAUSABLE ADEPTSTORAGETHREADSAFE
    LIBS+=-larmadillo
    LIBS+=-ladept
}

win32:{
    QMAKE_CXXFLAGS_RELEASE += -O3 -fopenmp -unroll-loops -fomit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread
    QMAKE_CFLAGS_RELEASE += -O3 -march=native -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native -pthread
    LIBS+=  -lm -fopenmp
}
unix:!macx{
    QMAKE_CXXFLAGS_RELEASE += -O3 -fopenmp -unroll-loops -fomit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread
    QMAKE_CFLAGS_RELEASE += -O3 -march=native -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native -pthread
    LIBS+=  -lm -fopenmp
}
macx{
        INCLUDEPATH+=/opt/homebrew/opt/libomp/include/
        INCLUDEPATH+=/usr/local/opt/libomp/include
        QMAKE_CXXFLAGS_RELEASE += -O3  -Xpreprocessor  -fopenmp -unroll-loops -fomit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native  -pthread
        QMAKE_CFLAGS_RELEASE += -O3 -march=native   -Xpreprocessor  -fopenmp -unroll-loops -omit-frame-pointer -Winline -unsafe-math-optimizations -mtune=native -pthread
        #LIBS+=  -lm  -Xpreprocessor  -fopenmp -L/opt/homebrew/opt/libomp/lib/  -lomp
        LIBS+=  -lm  -Xpreprocessor  -fopenmp -L/usr/local/opt/libomp/lib  -lomp
}
QMAKE_LFLAGS += -O3
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GE/cprogram.cc \
    METHODS/mbfgs.cpp\
    METHODS/MFO.cpp \
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
    METHODS/eo.cpp \
    METHODS/acop.cpp \
    METHODS/ego.cpp \
    METHODS/hybridmethod.cpp \
    METHODS/jDElsgo.cpp \
    METHODS/mewoa.cpp \
    METHODS/saop.cpp \
    METHODS/woa.cpp \
    OPTIMUS/editlogger.cpp \
    OPTIMUS/filelogger.cpp \
    OPTIMUS/logger.cpp \
    OPTIMUS/mean.cpp \
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
    OPTIMUS/methodloader.cpp \
    OPTIMUS/optimizer.cpp \
    OPTIMUS/parameter.cpp \
    OPTIMUS/parameterlist.cpp \
    OPTIMUS/problem.cpp \
    OPTIMUS/problemloader.cpp \
    PROBLEMS/Ackley.cpp \
    PROBLEMS/Extended_f10.cpp \
    PROBLEMS/Sphere.cpp \
    PROBLEMS/bf3.cpp \
    PROBLEMS/equal_maxima.cpp \
    PROBLEMS/f1.cpp \
    PROBLEMS/f12.cpp \
    PROBLEMS/f13.cpp \
    PROBLEMS/f14.cpp \
    PROBLEMS/f15.cpp \
    PROBLEMS/f16.cpp \
    PROBLEMS/f17.cpp \
    PROBLEMS/f18.cpp \
    PROBLEMS/f19.cpp \
    PROBLEMS/f3.cpp \
    PROBLEMS/f5.cpp \
    PROBLEMS/f9.cpp \
    PROBLEMS/himmelblau.cpp \
    PROBLEMS/schaffer.cpp \
    PROBLEMS/schwefel.cpp \
    PROBLEMS/schwefel221.cpp \
    PROBLEMS/schwefel222.cpp \
    PROBLEMS/shubert.cpp \
    PROBLEMS/vincent.cpp \
    XOPTIMUS/parameterdialog.cpp \
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
    PROBLEMS/E_ev.cpp \
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
    PROBLEMS/discus.cpp \
    PROBLEMS/cigar.cpp \
    PROBLEMS/test30n.cpp \
    PROBLEMS/userproblem.cpp \
    SAMPLER/kmeanssampler.cpp \
    SAMPLER/maxwellsampler.cpp \
    SAMPLER/neuralsampler.cpp \
    SAMPLER/problemsampler.cpp \
    SAMPLER/rbfsampler.cpp \
    SAMPLER/triangularsampler.cpp \
    SAMPLER/uniformsampler.cpp \
    XOPTIMUS/paramwidget.cpp \
    XOPTIMUS/selectmethoddialog.cpp \
    XOPTIMUS/selectproblemdialog.cpp \
    XOPTIMUS/xoptimusmain.cpp \
    XOPTIMUS/mainwindow.cpp
    contains(DEFINES,OPTIMUS_ARMADILLO){
        SOURCES+=MLMODELS/functionalrbf.cpp
    }
HEADERS += \
    GE/cprogram.h \
    METHODS/mbfgs.h \
    METHODS/MFO.h \
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
    METHODS/eo.h \
    METHODS/acop.h \
    METHODS/ego.h \
    METHODS/hybridmethod.h \
    METHODS/jDElsgo.h \
    METHODS/mewoa.h \
    METHODS/saop.h \
    METHODS/woa.h \
    OPTIMUS/editlogger.h \
    OPTIMUS/filelogger.h \
    OPTIMUS/logger.h \
    OPTIMUS/mean.h \
    OPTIMUS/methodloader.h \
    OPTIMUS/problemloader.h \
    PROBLEMS/Ackley.h \
    PROBLEMS/Extended_f10.h \
    PROBLEMS/Sphere.h \
    PROBLEMS/bf3.h \
    PROBLEMS/discus.h \
    PROBLEMS/cigar.h \
    LINESEARCH/armijosearch.h \
    LINESEARCH/fibonaccisearch.h \
    LINESEARCH/goldensearch.h \
    LINESEARCH/linesearch.h \
    METHODS/adam.h \
    METHODS/armadillo1.h \
    METHODS/bfgs.h \
    METHODS/armadillo1.h \
    METHODS/differentialevolution.h \
    METHODS/genetic.h \
    METHODS/gradientdescent.h \
    METHODS/gwooptimizer.h \
    PROBLEMS/E_ev.h \
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
    PROBLEMS/equal_maxima.h \
    PROBLEMS/f1.h \
    PROBLEMS/f12.h \
    PROBLEMS/f13.h \
    PROBLEMS/f14.h \
    PROBLEMS/f15.h \
    PROBLEMS/f16.h \
    PROBLEMS/f17.h \
    PROBLEMS/f18.h \
    PROBLEMS/f19.h \
    PROBLEMS/f3.h \
    PROBLEMS/f5.h \
    PROBLEMS/f9.h \
    PROBLEMS/himmelblau.h \
    PROBLEMS/schaffer.h \
    PROBLEMS/schwefel.h \
    PROBLEMS/schwefel221.h \
    PROBLEMS/schwefel222.h \
    PROBLEMS/shubert.h \
    PROBLEMS/vincent.h \
    XOPTIMUS/parameterdialog.h \
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
    XOPTIMUS/paramwidget.h \
    XOPTIMUS/selectmethoddialog.h \
    XOPTIMUS/selectproblemdialog.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
