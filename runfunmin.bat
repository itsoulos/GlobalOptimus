@echo off
REM echo run
REM Available optimization methods: Bfgs,Lbfgs,Genetic,Multistart,iPso,NeuralMinimizer,DifferentialEvolution
SET METHOD=iPso
REM Available local search methods: bfgs, lbfgs, gradient
SET LOCALSEARCH=bfgs
REM Available samplers: uniform, mlp, rbf, maxwell, triangular, kmeans
SET SAMPLER=uniform
REM Available stopping rules: maxiters, doublebox, similarity
SET TERMINATION=doublebox

IF %METHOD%==Bfgs (
    SET "METHODPARAMS=--bfgs_iters=200" 
)
IF %METHOD%==Genetic (
    REM gen_lrate:		local search rate
    REM gen_srate:		crossover rate
    REM gen_mrate:		mutation rate
    REM gen_tsize: 		tournament size
    REM gen_selection:		selection scheme (tournament|roulette)
    REM gen_crossover:		crossover scheme (double,uniform,onepoint)
    REM gen_mutation:		mutation  scheme (double|random) 
    REM gen_termination:	termination method (maxiters|doublebox|similarity) 
    REM gen_count:		number of chromosomes
    REM gen_maxiters:		maximum number of generations
    SET "METHODPARAMS=--gen_lrate=0.01 --gen_srate=0.1 --gen_mrate=0.05 --gen_tsize=8 --gen_selection=tournament --gen_crossover=double --gen_mutation=double --gen_termination=%TERMINATION% --gen_count=200 --opt_sampler=%SAMPLER%" --opt_localsearch=%LOCALSEARCH% --gen_maxiters=200
)
IF %METHOD%==Multistart (
REM ms_samples: number of multistart samples
REM ms_maxiters: maximum number of iterations
REM ms_termination: termination rule (maxiters|doublebox|similarity)

	METHODPARAMS="--ms_samples=50 --opt_sampler=$SAMPLER --ms_maxiters=100 --opt_localsearch=%LOCALSEARCH% -ms_termination=%TERMINATION%" 

)
IF %METHOD%==Lbfgs (  
    SET METHODPARAMS="--lbfgs_iters=200"
)
IF %METHOD%==DifferentialEvolution (
    REM de_np: number of de agents
    REM de_f: factor value
    REM de_cr: crosover parameter
    REM de_selection: the method to select agents values (random|tournament)
    REM de_maxiters: maximum number of iterations
    REM de_tsize:  tournament size
    REM de_termination: termination rule (doublebox|similarity|maxiters)
	METHODPARAMS="--de_np=10n --de_f=0.8 --de_cr=0.9 --de_tsize=4 --de_maxiters=1000 --de_selection=random --de_termination=%TERMINATION% --opt_sampler=%SAMPLER% --opt_localsearch=%LOCALSEARCH%"  

)
IF %METHOD%==iPso (
    REM ipso_particles: number of pso particles
    SET METHODPARAMS="--ipso_particles=200 --ipso_maxgenerations=200 --ipso_localsearch_rate=0.05 --ipso_stoppingrule=best_fitness -ipso_gradientcheck=true --ipso_inertiatype=2 --opt_sampler=%SAMPLER% --opt_localsearch=%LOCALSEARCH%"

)
IF %METHOD%==NeuralMinimizer (  
    SET METHODPARAMS="--neural_model=neural --neural_trainmethod=bfgs"
)


SET PROBLEM=%~1
SET DIMENSION=%~2
IF "%1"=="" (
    echo PROBLEM PARAM IS EMPTY
    exit
)
IF "%2"==""  (
    GlobalOptimus.exe --opt_problem=%PROBLEM% --opt_method=%METHOD% %METHODPARAMS% --iterations=30
) ELSE (
    GlobalOptimus "--opt_problem=%PROBLEM%" "--opt_method=%METHOD%" "--opt_dimension=%DIMENSION%" "%METHODPARAMS%" "--iterations=30"
)

             
