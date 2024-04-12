#Available optimization methods: GradientDescent,Adam,Bfgs,Lbfgs,NelderMead,Genetic,Multistart,iPso,NeuralMinimizer,DifferentialEvolution, ParallelDe, Simman
METHOD=Genetic
#Available local search methods: bfgs, lbfgs, gradient, nelderMead, adam
LOCALSEARCH=bfgs
#Available samplers: uniform, mlp, rbf, maxwell, triangular, kmeans
SAMPLER=uniform
#Available stopping rules: maxiters, doublebox, similarity
TERMINATION=maxiters

if [ $METHOD = "Bfgs" ]
then

#bfgs_iters: the maximum number of allowed iterations

	METHODPARAMS="--bfgs_iters=200"
elif [ $METHOD = "NelderMead" ]
then

#nm_population: number of items in simplex
#nm_alpha: the alpha parameter
#nm_gamma: the gamma parameter
#nm_rho:   the rho parameter
#nm_sigma: the sigma parameter
#nm_maxiters: maximum number of iterations allowed

	METHODPARAMS="--nm_population=100 --nm_alpha=1.0 --nm_gamma=2.0 --nm_rho=0.5 --nm_sigma=0.5 --nm_maxiters=100"
elif [ $METHOD = "Adam" ]
then

#adam_rate: adam learning rate
#adam_linesearch: linesearch method used. Possible values (none|golden|fibonacci|armijo)
#adam_maxiters: maximum number of iterations allowed
#adam_b1: the b1 parameter
#adam_b2: the b2 parameter
	METHODPARAMS="--adam_rate=0.001 --adam_linesearch=armijo --adam_maxiters=10000 --adam_b1=0.9 --adam_b2=0.999"
elif [ $METHOD = "GradientDescent" ]
then

#gd_maxiters: maximum number of allowed iterations
#gd_linesearch: the linesearch method used (none|golden|fibonacci|armijo)
#gd_epsilon: the epsilon value used in comparisons
#gd_rate: the line search rate

	METHODPARAMS="--gd_maxiters=200 --gd_linesearch=armijo --gd_epsilon=0.000001 --gd_rate=0.001"
elif [ $METHOD = "Genetic" ]
then

#gen_lrate:		local search rate
#gen_srate:		crossover rate
#gen_mrate:		mutation rate
#gen_tsize: 		tournament size
#gen_selection:		selection scheme (tournament|roulette)
#gen_crossover:		crossover scheme (double,uniform,onepoint)
#gen_mutation:		mutation  scheme (double|random) 
#gen_termination:	termination method (maxiters|doublebox|similarity) 
#gen_count:		number of chromosomes
#gen_maxiters:		maximum number of generations

	METHODPARAMS="--gen_lrate=0.01 --gen_srate=0.1 --gen_mrate=0.05 --gen_tsize=8 --gen_selection=tournament --gen_crossover=double --gen_mutation=double --gen_termination=$TERMINATION --gen_count=200 --opt_sampler=$SAMPLER --opt_localsearch=$LOCALSEARCH --gen_maxiters=80"
elif [ $METHOD = "Multistart" ]
then

#ms_samples: number of multistart samples
#ms_maxiters: maximum number of iterations
#ms_termination: termination rule (maxiters|doublebox|similarity)

	METHODPARAMS="--ms_samples=50 --opt_sampler=$SAMPLER --ms_maxiters=100 --opt_localsearch=$LOCALSEARCH --ms_termination=$TERMINATION"
elif [ $METHOD = "Lbfgs" ]
then

#lbfgs_iters: the maximum number of allowed iterations

	METHODPARAMS="--lbfgs_iters=200"
elif [ $METHOD = "DifferentialEvolution" ]
then
#de_np: number of de agents
#de_f: factor value
#de_cr: crosover parameter
#de_selection: the method to select agents values (random|tournament)
#de_maxiters: maximum number of iterations
#de_tsize:  tournament size
#de_termination: termination rule (doublebox|similarity|maxiters)
	METHODPARAMS="--de_np=10n --de_f=0.8 --de_cr=0.9 --de_tsize=4 --de_maxiters=1000 --de_selection=random --de_termination=$TERMINATION --opt_sampler=$SAMPLER --opt_localsearch=$LOCALSEARCH"
elif [ $METHOD = "iPso" ]
then
#ipso_particles: number of pso particles
#ipso_maxgenerations: maximum number of generations allowed
#ipso_localsearch_rate: the rate of local search applications
#ipso_stoppingrule: the stopping rule used (mean_fitness,best_fitness,doublebox,ali)
#ipso_gradientcheck: usage of gradient rejection rule (true|false)
#ipso_inertiatype: selection of inertia calcuation mechanism
METHODPARAMS="--ipso_particles=100 --ipso_maxgenerations=200 --ipso_localsearch_rate=0.05 --ipso_stoppingrule=best_fitness -ipso_gradientcheck=false --ipso_inertiatype=5 --opt_sampler=$SAMPLER --opt_localsearch=$LOCALSEARCH"

elif [ $METHOD = "NeuralMinimizer" ]
then

#neural_model: the model used in the method (neural|rbf)
#neural_weights:  the weights used in the model 
#neural_samples: the samples taken from the model
#neural_iterations: the maximum number of allowed iterations
#neural_start_samples: the samples used to construct initially the model
#neural_trainmethod: the local search procedure used to train the method

	METHODPARAMS="--neural_model=neural --neural_weights=10 --neural_samples=100 --neural_iterations=200 --neural_start_samples=50  --neural_termination=$TERMINATION  --neural_trainmethod=bfgs --opt_localsearch=$LOCALSEARCH"
elif [ $METHOD = "ParallelDe" ]
then

#parde_termination: termination rule (maxiters|doublebox|similarity)
#parde_agents: the number of agents per island
#parde_generations: maximum number of allowed generations
#parde_cr: crossover rate
#parde_f:  f factor
#parde_weight_method: The differential weight method. (random|ali|constant)
#parde_propagate_rate: The number of generations before the propagation starts.
#parde_selection_method:  parde_selection_method: The selection method used to select atoms in crossover (random|tournament)
#parde_propagate_method: The used propagation method between islands (to1|1toN|Nto1|NtoN)
#parde_islands: The number of parallel islands for the method.

	METHODPARAMS="--parde_termination=$TERMINATION --parde_agents=200 --parde_generations=1000 --parde_cr=0.9 --parde_f=0.8 --parde_weight_method=random --parde_propagate_rate=5 --parde_selection_method=tournament --parde_propagate_method=1to1 --parde_islands=1 --pade_islands_enable=1 --opt_localsearch=$LOCALSEARCH"

elif [ $METHOD = "ParallelPso" ]
then
	METHODPARAMS="--parallelPso_particles=200 --parallelPso_generations=200 --parallelPso_c1=0.5 --parallelPso_c2=0.5 --parallelPso_propagateRate=15 --parallelPso_propagateMethod=1to1 --parallelPso_subCluster=1 --parallelPso_subClusterEnable=1 --parallelPso_pnumber=1 --opt_localsearch=$LOCALSEARCH"
fi


PROBLEM=$1
DIMENSION=$2
if [ -z "$PROBLEM" ]
then
	echo "PROBLEM PARAM IS EMPTY"
	exit
fi

if [ -z "$DIMENSION" ] 
then
	./GlobalOptimus --opt_problem=$PROBLEM  --opt_method=$METHOD  $METHODPARAMS --opt_iters=1 --opt_debug=yes
else
	./GlobalOptimus --opt_problem=$PROBLEM  --opt_method=$METHOD --opt_dimension=$DIMENSION  $METHODPARAMS --opt_iters=30
fi
