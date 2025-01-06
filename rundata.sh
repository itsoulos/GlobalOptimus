#Available optimization methods: GradientDescent,Adam,Bfgs,Lbfgs,NelderMead,Genetic,Multistart,iPso,NeuralMinimizer,DifferentialEvolution, ParallelDe, Simman
METHOD=Bfgs
#Available local search methods: bfgs, lbfgs, gradient, nelderMead, adam
LOCALSEARCH=bfgs
#Available samplers: uniform, mlp, rbf, maxwell, triangular, kmeans
SAMPLER=uniform
#Available stopping rules: maxiters, doublebox, similarity
TERMINATION=similarity
#Available values: mlp, rbf, frbf,gdf, nnc, rule
MODEL=rule

BASEPATH=~/Desktop/ERGASIES/FeatureConstruction2/
DATAPATH=$BASEPATH/datasets/tenfolding/
DATAFILE=$1
GLOBALPARAMS="--opt_localsearch=$LOCALSEARCH --opt_sampler=$SAMPLER --opt_termination=$TERMINATION --opt_debug=yes"

if [ $MODEL = "mlp" ]
then
	MODELPARAMS="--opt_method=$METHOD --mlp_nodes=10 --mlp_leftmargin=-10 --mlp_rightmargin=10 --mlp_initmethod=smallvalues --model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --mlp_usebound=false --mlpboundlimit=10.0"
elif [ $MODEL = "fc" ]
then
	CREATEMODEL=rbf
	EVALUATEMODEL=mlp
	FEATURES=2
	CREATEPARAMS='\"--bfgs_iters=5\"\"--gen_maxiters=5\"\"--rbf_nodes=10\"\"--rbf_factor=2\"\"--opt_debug=no\"'
	EVALPARAMS='\"--opt_method=Bfgs\"\"--mlp_nodes=10\"\"--rbf_nodes=10\"\"--opt_debug=no\"\"--rbf_factor=5\"\"--opt_termination=maxiters\"'
	MODELPARAMS="--model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --fc_createmodel=$CREATEMODEL --fc_evaluatemodel=$EVALUATEMODEL --fc_popcount=500 --fc_popsize=200 --fc_popgens=200 --fc_features=$FEATURES --fc_createparams=$CREATEPARAMS --fc_evaluateparams=$EVALPARAMS"
elif [ $MODEL = "rbf" ]
then
	MODELPARAMS="--opt_method=$METHOD --rbf_nodes=10 --rbf_factor=8.0 --model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test"
elif [ $MODEL = "gdf" ]
then
	MODELPARAMS="--model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --gdf_popcount=200 --gdf_popsize=100 --gdf_popgens=200 --gdf_popsrate=0.9 --gdf_popmrate=0.05"
elif [ $MODEL = "rule" ]
then
	MODELPARAMS="--model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --rule_popcount=500 --rule_popsize=200 --rule_popgens=1000 --rule_popsrate=0.1 --rule_popmrate=0.05 --rule_poplrate=0.000  --rule_lsearchmethod=crossover"
elif [ $MODEL = "nnc" ]
then
	MODELPARAMS="--model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --nnc_popcount=500 --nnc_popsize=500 --nnc_popgens=500 --nnc_popsrate=0.1 --nnc_popmrate=0.05 --nnc_lsearchrate=0.000 --nnc_lsearchmethod=mutate --nnc_crossitems=10 --nnc_lsearchiters=5 --nnc_lsearchitems=5"
fi

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
elif [ $METHOD = "DifferentialEvolution" ]
then
##de_np:          The population size. 
##de_f:           The differential weight
##de_cr:          The mutation parameter
##de_tsize:       The tournament size for the tournament selection
##de_maxiters:    The maximum number of parameters
##de_fselection:  The selection of differential weight. Values: number, ali, random, adaptive, migrant
##de_selection:   The selection method used in every iteration. Values: random, tournament
	METHODPARAMS="--de_np=200 --de_maxiters=200 --de_selection=tournament --de_fselection=migrant --de_isneural=yes --de_neuralw=2 --de_lrate=0.005 --opt_termination=$TERMINATION --de_neuralsampling=no"
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

	METHODPARAMS="--gen_lrate=0.000 --gen_srate=0.1 --gen_mrate=0.05 --gen_tsize=8 --gen_selection=tournament --gen_crossover=double --gen_mutation=double --gen_count=500 --gen_maxiters=200 --gen_lsearchmethod=none"
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
elif [ $METHOD = "iPso" ]
then
#ipso_particles: number of pso particles
#ipso_maxgenerations: maximum number of generations allowed
#ipso_localsearch_rate: the rate of local search applications
#ipso_stoppingrule: the stopping rule used (mean_fitness,best_fitness,doublebox,ali)
#ipso_gradientcheck: usage of gradient rejection rule (true|false)
#ipso_inertiatype: selection of inertia calcuation mechanism
METHODPARAMS="--ipso_particles=500 --ipso_maxgenerations=200 --ipso_localsearch_rate=0.001 --ipso_stoppingrule=best_fitness -ipso_gradientcheck=false --ipso_inertiatype=5 "

elif [ $METHOD = "NeuralMinimizer" ]
then

#neural_model: the model used in the method (neural|rbf)
#neural_weights:  the weights used in the model 
#neural_samples: the samples taken from the model
#neural_iterations: the maximum number of allowed iterations
#neural_start_samples: the samples used to construct initially the model
#neural_trainmethod: the local search procedure used to train the method

	METHODPARAMS="--neural_model=neural --neural_weights=10 --neural_samples=100 --neural_iterations=200 --neural_start_samples=50  --neural_termination=$TERMINATION  --neural_trainmethod=lbfgs"
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

./DataFitting --opt_model=$MODEL  $GLOBALPARAMS $METHODPARAMS $MODELPARAMS --opt_iters=30 --opt_debug=yes
