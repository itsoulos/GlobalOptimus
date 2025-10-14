QT       += core gui
LIBS+=  -lm -fopenmp
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
	SAMPLER/usersampler.cpp \
	PROBLEMS/zakharov.cpp \
	PROBLEMS/michalewicz.cpp \
	PROBLEMS/gallagher21.cpp \
	PROBLEMS/ellipsoidal.cpp \
	METHODS/MFO.cpp \
	METHODS/mbfgs.cpp  \
        GE/doublestack.cc \
        GE/fparser.cc \
        GE/fpoptimizer.cc \
        GE/population.cc \
        GE/program.cc \
        GE/rule.cc \
        GE/symbol.cc \
	GE/integeranneal.cpp \
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
    OPTIMUS/problemloader.cpp \
        OPTIMUS/statistics.cpp \
        OPTIMUS/collection.cpp \
        OPTIMUS/doublebox.cpp \
        OPTIMUS/similarity.cpp \
    PROBLEMS/Ackley.cpp \
    PROBLEMS/E_ev.cpp \
    PROBLEMS/Extended_f10.cpp \
        PROBLEMS/bf1.cpp \
        PROBLEMS/bf2.cpp \
    PROBLEMS/bf3.cpp \
        PROBLEMS/branin.cpp \
        PROBLEMS/camel.cpp \
        PROBLEMS/cm.cpp \
        PROBLEMS/diffpower.cpp \
        PROBLEMS/diracproblem.cpp \
        PROBLEMS/easom.cpp \
        PROBLEMS/elp.cpp \
    PROBLEMS/equal_maxima.cpp \
        PROBLEMS/exp.cpp \
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
    PROBLEMS/himmelblau.cpp \
        PROBLEMS/levy.cpp \
        PROBLEMS/potential.cpp \
        PROBLEMS/rosenbrock.cpp \
        PROBLEMS/salkin.cpp \
    PROBLEMS/schaffer.cpp \
    PROBLEMS/schwefel.cpp \
    PROBLEMS/schwefel221.cpp \
    PROBLEMS/schwefel222.cpp \
        PROBLEMS/shekel10.cpp \
        PROBLEMS/shekel5.cpp \
	PROBLEMS/gross.cpp\
        PROBLEMS/shekel7.cpp \
    PROBLEMS/shubert.cpp \
        PROBLEMS/sinu.cpp \
        PROBLEMS/test30n.cpp \
        MLMODELS/dataset.cpp \
        PROBLEMS/userproblem.cpp \
    PROBLEMS/vincent.cpp \
        SAMPLER/kmeanssampler.cpp \
        SAMPLER/maxwellsampler.cpp \
        SAMPLER/neuralsampler.cpp \
        SAMPLER/rbfsampler.cpp \
        SAMPLER/triangularsampler.cpp \
        SAMPLER/uniformsampler.cpp \
        main.cpp \
        OPTIMUS/optimizer.cpp \
        OPTIMUS/parameter.cpp \
	PROBLEMS/discus.cpp \
	PROBLEMS/cigar.cpp \
	PROBLEMS/griewank.cpp \
	PROBLEMS/katsuura.cpp \
	PROBLEMS/griewank_rosenbrock.cpp \
	PROBLEMS/lunacek_bi_rastrigin.cpp \
	PROBLEMS/weierstrass.cpp \
	PROBLEMS/different_powers.cpp \
	PROBLEMS/sharp_ridge.cpp \
	PROBLEMS/rotated_rosenbrock.cpp \
	PROBLEMS/step_ellipsoidal.cpp \
	PROBLEMS/attractive_sector.cpp \
	PROBLEMS/linear_slope.cpp \
	PROBLEMS/buche_rastrigin.cpp \
	PROBLEMS/gallagher101.cpp \
	PROBLEMS/rastrigin.cpp \
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
    GE/cprogram.h \
    SAMPLER/usersampler.h\
    PROBLEMS/zakharov.h \
    PROBLEMS/michalewicz.h \
    PROBLEMS/gallagher21.h \
    PROBLEMS/ellipsoidal.h \
    PROBLEMS/lunacek_bi_rastrigin.h \
    PROBLEMS/griewank_rosenbrock.h \
    PROBLEMS/different_powers.h \
    PROBLEMS/rotated_rosenbrock.h \
    PROBLEMS/step_ellipsoidal.h \
    PROBLEMS/attractive_sector.h \
    PROBLEMS/rastrigin.h \
    PROBLEMS/linear_slope.h \
    PROBLEMS/buche_rastrigin.h \
    PROBLEMS/gallagher101.h \
    PROBLEMS/sharp_ridge.h \
    PROBLEMS/weierstrass.h \
    PROBLEMS/katsuura.h \
    PROBLEMS/griewank.h \
    METHODS/mbfgs.h \
    METHODS/MFO.h \
    GE/doublestack.h \
    GE/fparser.hh \
    GE/fpconfig.hh \
    GE/fptypes.hh \
    GE/population.h \
    GE/program.h \
    GE/rule.h \
    GE/symbol.h \
    GE/integeranneal.h\
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
    PROBLEMS/E_ev.h \
    PROBLEMS/Extended_f10.h \
    PROBLEMS/bf3.h \
    PROBLEMS/discus.h \
    PROBLEMS/cigar.h \
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
    PROBLEMS/equal_maxima.h \
    PROBLEMS/exp.h \
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
    PROBLEMS/himmelblau.h \
    PROBLEMS/levy.h \
    PROBLEMS/potential.h \
    PROBLEMS/rastriginproblem.h \
    PROBLEMS/rosenbrock.h \
    PROBLEMS/salkin.h \
    PROBLEMS/schaffer.h \
    PROBLEMS/schwefel.h \
    PROBLEMS/schwefel221.h \
    PROBLEMS/schwefel222.h \
    PROBLEMS/shekel10.h \
    PROBLEMS/shekel5.h \
    PROBLEMS/shekel7.h \
    PROBLEMS/shubert.h \
    PROBLEMS/sinu.h \
    PROBLEMS/test2nproblem.h \
    PROBLEMS/test30n.h \
    MLMODELS/dataset.h \
    PROBLEMS/userproblem.h \
    PROBLEMS/vincent.h \
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
