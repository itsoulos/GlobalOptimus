@echo off
REM echo run
REM Available optimization methods: Bfgs,Lbfgs,Genetic,Multistart,iPso,NeuralMinimizer,DifferentialEvolution
SET METHOD=ParallelPso
REM Available local search methods: bfgs, lbfgs, gradient
SET LOCALSEARCH=bfgs
REM Available samplers: uniform, mlp, rbf, maxwell, triangular, kmeans
SET SAMPLER=uniform
REM Available stopping rules: maxiters, doublebox, similarity
SET TERMINATION=doublebox
IF %METHOD%==Bfgs (
    REM bfgs_iters: the maximum number of allowed iterations

    SET METHODPARAMS="--bfgs_iters=200" 
)
IF %METHOD%==GradientDescent (
    REM gd_maxiters: maximum number of allowed iterations
    REM gd_linesearch: the linesearch method used (none|golden|fibonacci|armijo)
    REM gd_epsilon: the epsilon value used in comparisons
    REM gd_rate: the line search rate

    SET METHODPARAMS="--gd_maxiters=200 --gd_linesearch=armijo --gd_epsilon=0.000001 --gd_rate=0.001"
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

    SET METHODPARAMS="--gen_lrate=0.01 --gen_srate=0.1 --gen_mrate=0.05 --gen_tsize=8 --gen_selection=tournament --gen_crossover=double --gen_mutation=double --gen_termination=%TERMINATION% --gen_count=200 --opt_sampler=%SAMPLER%" --opt_localsearch=%LOCALSEARCH% --gen_maxiters=200"
)
IF %METHOD%==Multistart (   
    REM ms_samples: number of multistart samples
    REM ms_maxiters: maximum number of iterations
    REM ms_termination: termination rule (maxiters|doublebox|similarity)

    SET METHODPARAMS="--ms_samples=50 --opt_sampler=$SAMPLER --ms_maxiters=100 --opt_localsearch=%LOCALSEARCH% -ms_termination=%TERMINATION%" 
)
IF %METHOD%==Lbfgs (
    REM lbfgs_iters: the maximum number of allowed iterations

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

	SET METHODPARAMS="--de_np=10n --de_f=0.8 --de_cr=0.9 --de_tsize=4 --de_maxiters=1000 --de_selection=random --de_termination=%TERMINATION% --opt_sampler=%SAMPLER% --opt_localsearch=%LOCALSEARCH%"  
)
IF %METHOD%==iPso (
    REM ipso_particles: number of pso particles
    REM ipso_maxgenerations: maximum number of generations allowed
    REM #ipso_localsearch_rate: the rate of local search applications
    REM ipso_stoppingrule: the stopping rule used (mean_fitness,best_fitness,doublebox,ali)
    REM ipso_gradientcheck: usage of gradient rejection rule (true|false)
    REM ipso_inertiatype: selection of inertia calcuation mechanism

    SET METHODPARAMS="--ipso_particles=200 --ipso_maxgenerations=200 --ipso_localsearch_rate=0.05 --ipso_stoppingrule=best_fitness -ipso_gradientcheck=true --ipso_inertiatype=2 --opt_sampler=%SAMPLER% --opt_localsearch=%LOCALSEARCH%"
)
IF %METHOD%==NeuralMinimizer (  
    REM neural_model: the model used in the method (neural|rbf)
    REM neural_weights:  the weights used in the model 
    REM neural_samples: the samples taken from the model
    REM neural_iterations: the maximum number of allowed iterations
    REM neural_start_samples: the samples used to construct initially the model
    REM neural_trainmethod: the local search procedure used to train the method

	SET METHODPARAMS="--neural_model=rbf --neural_weights=10 --neural_samples=500 --neural_iterations=200 --neural_start_samples=500  --neural_termination=%TERMINATION%  --neural_trainmethod=bfgs --opt_localsearch=%LOCALSEARCH%"
)
IF %METHOD%==ParallelDe (  
    REM parde_termination: termination rule (maxiters|doublebox|similarity)
    REM parde_agents: the number of agents per island
    REM parde_generations: maximum number of allowed generations
    REM parde_cr: crossover rate
    REM parde_f:  f factor
    REM parde_weight_method: The differential weight method. (random|ali|constant)
    REM parde_propagate_rate: The number of generations before the propagation starts.
    REM parde_selection_method:  parde_selection_method: The selection method used to select atoms in crossover (random|tournament)
    REM parde_propagate_method: The used propagation method between islands (to1|1toN|Nto1|NtoN)
    REM parde_islands: The number of parallel islands for the method.

    SET	METHODPARAMS="--parde_termination=%TERMINATION% --parde_agents=200 --parde_generations=1000 --parde_cr=0.9 --parde_f=0.8 --parde_weight_method=random --parde_propagate_rate=5 --parde_selection_method=tournament --parde_propagate_method=1to1 --parde_islands=1 --pade_islands_enable=1 --opt_localsearch=%LOCALSEARCH%"
)
IF %METHOD%==ParallelPso (  
    REM parde_termination: termination rule (maxiters|doublebox|similarity)
    REM parde_agents: the number of agents per island
    REM parde_generations: maximum number of allowed generations
    REM parde_cr: crossover rate
    REM parde_f:  f factor
    REM parde_weight_method: The differential weight method. (random|ali|constant)
    REM parde_propagate_rate: The number of generations before the propagation starts.
    REM parde_selection_method:  parde_selection_method: The selection method used to select atoms in crossover (random|tournament)
    REM parde_propagate_method: The used propagation method between islands (to1|1toN|Nto1|NtoN)
    REM parde_islands: The number of parallel islands for the method.

    SET METHODPARAMS="--parallelPso_particles=200 --parallelPso_generations=200 --parallelPso_c1=0.5 --parallelPso_c2=0.5 --parallelPso_propagateRate=15 --parallelPso_propagateMethod=1to1 --parallelPso_subCluster=1 --parallelPso_subClusterEnable=1 --parallelPso_pnumber=1 --opt_localsearch=%LOCALSEARCH%"
)
SET PROBLEM=%~1
SET DIMENSION=%~2
IF "%1"=="" (
    echo PROBLEM PARAM IS EMPTY
    exit
)
IF "%2"=="" (
    GlobalOptimus "--opt_problem=%PROBLEM%" "--opt_method=%METHOD%" "%METHODPARAMS%" "--iterations=30"
) ELSE (
    GlobalOptimus "--opt_problem=%PROBLEM%" "--opt_method=%METHOD%" "--opt_dimension=%DIMENSION%" "%METHODPARAMS%" "--iterations=30"
)

             