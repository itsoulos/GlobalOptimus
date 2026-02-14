#Available optimization methods: GradientDescent,Adam,Bfgs,Lbfgs,Gwo, NelderMead,Genetic,Multistart,iPso,NeuralMinimizer,DifferentialEvolution, ParallelDe, Simman, Ego, HybridMethod,Woa,MeWoa,JDE,UserMethod, BGWO
METHOD=Genetic
#Available local search methods: bfgs, lbfgs, gradient, nelderMead, adam
LOCALSEARCH=bfgs
#Available samplers: uniform, mlp, rbf, maxwell, triangular, kmeans
SAMPLER=uniform
#Available stopping rules: maxiters, doublebox, similarity, mean, all, sumfitness (only for DE)
TERMINATION=maxiters
### GLOBAL PARAMS
GLOBALPARAMS="--opt_localsearch=$LOCALSEARCH --opt_sampler=$SAMPLER --opt_termination=$TERMINATION "
if [ $METHOD = "bfgs" ]
then

#bfgs_iters: the maximum number of allowed iterations

	METHODPARAMS="--bfgs_iters=200"
elif [ $METHOD = "bho" ]
then
	  METHODPARAMS="--population=100 --maxiters=500 --localSearchRate=0.00"
elif [ $METHOD = "MFO" ]
then
	 METHODPARAMS="--mfo_count=200 --mfo_maxiters=200 --mfo_lrate=0.05 --mfo_strategy_mode=aggressive --stoppingRule=similarity"
elif [ $METHOD = "BGWO" ]
then
  METHODPARAMS="--bgwo_agents=100 --bgwo_maxiters=100 --lrate=0.05"
elif [ $METHOD = "DifferentialEvolution" ]
then
##de_np:          The population size. 
##de_f:           The differential weight
##de_cr:          The mutation parameter
##de_tsize:       The tournament size for the tournament selection
##de_maxiters:    The maximum number of parameters
##de_fselection:  The selection of differential weight. Values: number, ali, random, adaptive, migrant
##de_localsearch: Enable or disable the incorporation of local search at every iteration. Values: yes,no
##de_selection:   The selection method used in every iteration. Values: random, tournament
	METHODPARAMS="--de_np=200 --de_maxiters=500 --de_selection=tournament --de_fselection=random --de_lrate=0.005 --opt_termination=sumfitness"
elif [ $METHOD = "SAOP" ]
then
	METHODPARAMS="--population=500 --maxiters=200 --localsearchRate=0.00 --subPopulation=10 --subPopEnable=1 --prop=1 --propagationNumber=1 --propagationMethod=NtoN --propagationRate=1"
elif [ $METHOD = "ACOP" ]
then
	METHODPARAMS="--population=500 --maxiters=200 --localsearchRate=0.00 --subPopulation=10 --subPopEnable=1 --prop=1 --propagationNumber=1 --propagationMethod=NtoN --propagationRate=1"
elif [ $METHOD = "EO" ]
then
	METHODPARAMS="--population=200 --maxiters=200 --localsearchRate=0.02 --technique=0 --reject=0"
elif [ $METHOD = "NelderMead" ]
then
#nm_population: number of items in simplex
#nm_alpha: the alpha parameter
#nm_gamma: the gamma parameter
#nm_rho:   the rho parameter
#nm_sigma: the sigma parameter
#nm_maxiters: maximum number of iterations allowed
	METHODPARAMS="--nm_population=100 --nm_alpha=1.0 --nm_gamma=2.0 --nm_rho=0.5 --nm_sigma=0.5 --nm_maxiters=100"
elif [ $METHOD = "Ego" ]
then
METHODPARAMS="--mod1=1 --mod2=1 --mod3=2 --ego_count=200 --ego_maxiters=200 --ego_lrate=0.05 --ego_localiters=3 --ego_localmethod=none"
elif [ $METHOD = "Ofa" ]
then
	METHODPARAMS="--ofa_count=500  --ofa_maxiters=200 --ofa_lrate=0.02 --ofa_bfgsiters=3"
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

	METHODPARAMS="--gen_lrate=0.00 --gen_srate=0.1 --gen_mrate=0.05 --gen_tsize=8 --gen_selection=tournament --gen_crossover=double --gen_mutation=double --gen_count=500 --gen_maxiters=200"
elif [ $METHOD = "Multistart" ]
then
#ms_samples: number of multistart samples
#ms_maxiters: maximum number of iterations
#ms_termination: termination rule (maxiters|doublebox|similarity)
	METHODPARAMS="--ms_samples=50  --ms_maxiters=100"
elif [ $METHOD = "Lbfgs" ]
then
#lbfgs_iters: the maximum number of allowed iterations

	METHODPARAMS="--lbfgs_iters=200"
elif [ $METHOD = "iPso" ]
then
#ipso_particles: number of pso particles
#ipso_maxgenerations: maximum number of generations allowed
#ipso_localsearch_rate: the rate of local search applications
#ipso_stoppingrule: the stopping rule used (mean_fitness,best_fitness,doublebox,ali)
#ipso_gradientcheck: usage of gradient rejection rule (true|false)
#ipso_inertiatype: selection of inertia calcuation mechanism
METHODPARAMS="--ipso_particles=200 --ipso_maxgenerations=200 --ipso_localsearch_rate=0.05 --ipso_stoppingrule=best_fitness --ipso_gradientcheck=false --ipso_inertiatype=10"


elif [ $METHOD = "NeuralMinimizer" ]
then
#neural_model: the model used in the method (neural|rbf)
#neural_weights:  the weights used in the model 
#neural_samples: the samples taken from the model
#neural_iterations: the maximum number of allowed iterations
#neural_start_samples: the samples used to construct initially the model
#neural_trainmethod: the local search procedure used to train the method
	METHODPARAMS="--neural_model=neural --neural_weights=10 --neural_samples=100 --neural_iterations=200 --neural_start_samples=50   --neural_trainmethod=bfgs"
elif [ $METHOD = "armadillo1" ]
then
	METHODPARAMS="--gao_count=100 --gao_maxiters=100 --gao_lrate=0.05"
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

	METHODPARAMS="--parde_agents=200 --parde_generations=1000 --parde_cr=0.9 --parde_f=0.8 --parde_weight_method=random --parde_propagate_rate=5 --parde_selection_method=tournament --parde_propagate_method=1to1 --parde_islands=1 --pade_islands_enable=1"

elif [ $METHOD = "ParallelPso" ]
then
	METHODPARAMS="--parallelPso_particles=200 --parallelPso_generations=200 --parallelPso_c1=0.5 --parallelPso_c2=0.5 --parallelPso_propagateRate=15 --parallelPso_propagateMethod=1to1 --parallelPso_subCluster=1 --parallelPso_subClusterEnable=1 --parallelPso_pnumber=1"
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
       ./GlobalOptimus --opt_problem=$PROBLEM  --opt_method=$METHOD  $GLOBALPARAMS $METHODPARAMS --opt_iters=30 --opt_debug=yes
else
	./GlobalOptimus --opt_problem=$PROBLEM  --opt_method=$METHOD --opt_dimension=$DIMENSION $GLOBALPARAMS  $METHODPARAMS --opt_iters=30 --opt_debug=no
fi
