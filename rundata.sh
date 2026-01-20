#Available optimization methods: GradientDescent,Adam,Bfgs,Lbfgs,NelderMead,Genetic,Multistart,iPso,NeuralMinimizer,DifferentialEvolution, ParallelDe, Simman, Trident
METHOD=iPso
#Available local search methods: bfgs, lbfgs, gradient, nelderMead, adam
LOCALSEARCH=bfgs
#Available samplers: uniform, mlp, rbf, maxwell, triangular, kmeans, dist
SAMPLER=uniform
#Available stopping rules: maxiters, doublebox, similarity
TERMINATION=doublebox
#Available values: mlp, rbf, frbf,gdf, nnc, rule
MODEL=mlp

BASEPATH=~/Desktop/ERGASIES/FeatureConstruction2/
DATAPATH=$BASEPATH/datasets/tenfolding/
DATAFILE=$1
GLOBALPARAMS="--opt_localsearch=$LOCALSEARCH --opt_sampler=$SAMPLER --opt_termination=$TERMINATION --opt_debug=yes"

if [ $MODEL = "mlp" ]; then
  MODELPARAMS="--opt_method=$METHOD --mlp_nodes=10 --mlp_leftmargin=-10 --mlp_rightmargin=10 --mlp_initmethod=uniform --model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --mlp_usebound=no --mlpboundlimit=10.0 --mlp_balanceclass=no --mlp_usesimanbound=no"
elif [ $MODEL = "fc" ]; then
  CREATEMODEL=rbf
  EVALUATEMODEL=mlp
  FEATURES=2
  CREATEPARAMS='\"--bfgs_iters=5\"\"--gen_maxiters=5\"\"--rbf_nodes=10\"\"--rbf_factor=2\"\"--opt_debug=no\"'
  EVALPARAMS='\"--opt_method=Bfgs\"\"--mlp_nodes=10\"\"--rbf_nodes=10\"\"--opt_debug=no\"\"--rbf_factor=5\"\"--opt_termination=maxiters\"'
  MODELPARAMS="--model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --fc_createmodel=$CREATEMODEL --fc_evaluatemodel=$EVALUATEMODEL --fc_popcount=500 --fc_popsize=200 --fc_popgens=200 --fc_features=$FEATURES --fc_createparams=$CREATEPARAMS --fc_evaluateparams=$EVALPARAMS"
elif [ $MODEL = "rbf" ]; then
  MODELPARAMS="--opt_method=$METHOD --rbf_nodes=10 --rbf_factor=2.0 --model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test"
elif [ $MODEL = "airbf" ]; then
##rbf_train.  Rbf trainig method. Possible values: optimus (default), rbf_gd, rbf_adam, rbf_bfgs
##rbf_nodes.  Number of rbf nodes (default 10).
##rbf_factor. Scale factor for rbr (default 3.0).
  MODELPARAMS="--rbf_localsearchrate=0.001 --rbf_usesigmaminmax=yes --rbf_train=optimus --opt_method=$METHOD --rbf_nodes=10 --rbf_factor=3.0 --model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test"
elif [ $MODEL = "frbf" ]; then
  MODELPARAMS="--opt_method=$METHOD --rbf_nodes=10 --rbf_factor=2.0 --model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test"
elif [ $MODEL = "gdf" ]; then
  MODELPARAMS="--model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --gdf_popcount=200 --gdf_popsize=100 --gdf_popgens=200 --gdf_popsrate=0.9 --gdf_popmrate=0.05"
elif [ $MODEL = "rule" ]; then
  MODELPARAMS="--model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --rule_popcount=500 --rule_popsize=200 --rule_popgens=1000 --rule_popsrate=0.1 --rule_popmrate=0.05 --rule_poplrate=0.01  --rule_lsearchmethod=mutate"
elif [ $MODEL = "nnc" ]; then
### PARAMS FOR model  nnc
### nnc_popcount: Number of chromosomes in the genetic population.
### nnc_popsize: The length of chromosomes in the genetic population.
### nnc_popgens: The maximum number of allowed generations.
### nnc_popsrate: The selection rate.
### nnc_popmrate: The mutation rate.
### nnc_lsearchrate: The used local search rate.
### nnc_lsearchmethod: The local search method used periodically.
### nnc_lsearchiters: The number of iterations, after which the local search method will be applied.
### nnc_lsearchitems: The number of items that participate in the local search procedure.
### nnc_steadystate: Enable or disable the steady state feature.
### nnc_weightfactor: The weight factor used in the local search procedure.
### nnc_enablebound: Enable or disable the usage of bounding techniques during the local search procedure.
### nnc_balanceclass: Enable or disable the imbalanced classes feature
  MODELPARAMS="--model_trainfile=$DATAPATH/$1.train --model_testfile=$DATAPATH/$1.test --nnc_popcount=500 --nnc_popsize=500 --nnc_popgens=500 --nnc_popsrate=0.1 --nnc_popmrate=0.05 --nnc_lsearchrate=0.00 --nnc_lsearchmethod=none --nnc_crossitems=20 --nnc_lsearchiters=20 --nnc_lsearchitems=20 --nnc_steadystate=no --nnc_weightfactor=2.0  --nnc_enablebound=yes --nnc_pretrain=no --nnc_balanceclass=yes"
fi

if [ $METHOD = "Bfgs" ]; then

  #bfgs_iters: the maximum number of allowed iterations

  METHODPARAMS="--bfgs_iters=200"
elif [ $METHOD = "EO" ]; then
METHODPARAMS="--population=100 --maxiters=200 --localsearchRate=0.00 --technique=0"
elif [ $METHOD = "NelderMead" ]; then

  #nm_population: number of items in simplex
  #nm_alpha: the alpha parameter
  #nm_gamma: the gamma parameter
  #nm_rho:   the rho parameter
  #nm_sigma: the sigma parameter
  #nm_maxiters: maximum number of iterations allowed

  METHODPARAMS="--nm_population=100 --nm_alpha=1.0 --nm_gamma=2.0 --nm_rho=0.5 --nm_sigma=0.5 --nm_maxiters=100"
elif [ $METHOD = "DifferentialEvolution" ]; then
  ##de_np:          The population size.
  ##de_f:           The differential weight
  ##de_cr:          The mutation parameter
  ##de_tsize:       The tournament size for the tournament selection
  ##de_maxiters:    The maximum number of parameters
  ##de_fselection:  The selection of differential weight. Values: number, ali, random, adaptive, migrant
  ##de_selection:   The selection method used in every iteration. Values: random, tournament
  METHODPARAMS="--de_np=200 --de_maxiters=200 --de_selection=tournament --de_fselection=migrant --de_lrate=0.005 --opt_termination=$TERMINATION --de_neuralsampling=no --de_isneural=yes --de_neuralw=2.0"
elif [ $METHOD = "bho" ]
then
	  METHODPARAMS="--population=100 --maxiters=500 --localSearchRate=0.00"
elif [ $METHOD = "Adam" ]; then

  #adam_rate: adam learning rate
  #adam_linesearch: linesearch method used. Possible values (none|golden|fibonacci|armijo)
  #adam_maxiters: maximum number of iterations allowed
  #adam_b1: the b1 parameter
  #adam_b2: the b2 parameter
  METHODPARAMS="--adam_rate=0.001 --adam_linesearch=armijo --adam_maxiters=2000 --adam_b1=0.9 --adam_b2=0.999"
elif [ $METHOD = "GradientDescent" ]; then

  #gd_maxiters: maximum number of allowed iterations
  #gd_linesearch: the linesearch method used (none|golden|fibonacci|armijo)
  #gd_epsilon: the epsilon value used in comparisons
  #gd_rate: the line search rate

  METHODPARAMS="--gd_maxiters=200 --gd_linesearch=armijo --gd_epsilon=0.000001 --gd_rate=0.001"
elif [ $METHOD = "PdoubleGenetic" ]; then
#pgen_chromosomes: 	Number of chromosomes
#pgen_generations: 	Maximum number of allowed generations
#pgen_selectionrate: 	Selection rate of the genetic algorithm
#pgen_mutationrate:  	Mutation rate of the genetic algorithm
#pgen_subClusterEnable: The number of islands that participate in the termination rule [1,islands]
#pgen_subCluster:	Number of islands
#pgen_centers:		Number of centers for k-means
#pgen_pNumber:		Numbers of samples in propagate
#pgen_parallelPropagateMethod: Propagation method (1to1,1toN,Nto1,NtoN)
#pgen_propagateRate:    Number of generations before propagation.
#pgen_sampler:		Sampling method (uniform, kmeans, teams)
THREADS=5
TOTAL_AGENTS=500
PAGENTS=`expr $TOTAL_AGENTS/$THREADS | bc `
 METHODPARAMS="--pgen_chromosomes=$PAGENTS --pgen_generations=500 --pgen_selectionrate=0.1 --pgen_mutationrate=0.05 --pgen_localsearchrate=0.00 --pgen_subCluster=$THREADS --pgen_subClusterEnable=1 --pgen_centers=20 --pgen_pNumber=5 --pgen_parallelPropagateMethod=NtoN --opt_distpartitions=$THREADS"
elif [ $METHOD = "Genetic" ]; then

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

  METHODPARAMS="--gen_lrate=0.000 --gen_srate=0.1 --gen_mrate=0.05 --gen_tsize=8 --gen_selection=tournament --gen_crossover=double --gen_mutation=double --gen_count=200 --gen_maxiters=200 --gen_lsearchmethod=none --gen_lsearchgens=20 --gen_lsearchitems=20"
elif [ $METHOD = "Multistart" ]; then

  #ms_samples: number of multistart samples
  #ms_maxiters: maximum number of iterations
  #ms_termination: termination rule (maxiters|doublebox|similarity)

  METHODPARAMS="--ms_samples=50 --opt_sampler=$SAMPLER --ms_maxiters=100 --opt_localsearch=$LOCALSEARCH --ms_termination=$TERMINATION"
elif [ $METHOD = "Lbfgs" ]; then

  #lbfgs_iters: the maximum number of allowed iterations

  METHODPARAMS="--lbfgs_iters=200"
elif [ $METHOD = "iPso" ]; then
  #ipso_particles: number of pso particles
  #ipso_maxgenerations: maximum number of generations allowed
  #ipso_localsearch_rate: the rate of local search applications
  #ipso_stoppingrule: the stopping rule used (mean_fitness,best_fitness,doublebox,ali)
  #ipso_gradientcheck: usage of gradient rejection rule (true|false)
  #ipso_inertiatype: selection of inertia calcuation mechanism
METHODPARAMS="--ipso_particles=200 --ipso_maxgenerations=200 --ipso_localsearch_rate=0.05 --ipso_stoppingrule=similarity --ipso_gradientcheck=false --ipso_inertiatype=20 --ipso_movemechanism=turbulence --ipso_restart=yes --ipso_restart_stall=3 --ipso_restart_max=10" 

elif [ $METHOD = "NeuralMinimizer" ]; then

  #neural_model: the model used in the method (neural|rbf)
  #neural_weights:  the weights used in the model
  #neural_samples: the samples taken from the model
  #neural_iterations: the maximum number of allowed iterations
  #neural_start_samples: the samples used to construct initially the model
  #neural_trainmethod: the local search procedure used to train the method

  METHODPARAMS="--neural_model=neural --neural_weights=10 --neural_samples=100 --neural_iterations=200 --neural_start_samples=50  --neural_termination=$TERMINATION  --neural_trainmethod=lbfgs"
elif [ $METHOD = "ParallelDe" ]; then

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

  THREADS=20
  TOTAL_AGENTS=200
  PAGENTS=`expr $TOTAL_AGENTS/$THREADS | bc `
  METHODPARAMS="--parde_termination=$TERMINATION --parde_agents=$PAGENTS --parde_generations=1000 --parde_cr=0.9 --parde_f=0.8 --parde_weight_method=random --parde_propagate_rate=10 --parde_selection_method=random --parde_propagate_method=1to1 --parde_islands=$THREADS --parde_islands_enable=$THREADS --opt_distpartitions=$THREADS --opt_localsearch=$LOCALSEARCH"

elif [ $METHOD = "ParallelPso" ]; then
  THREADS=10
  TOTAL_AGENTS=500
  PAGENTS=`expr $TOTAL_AGENTS/$THREADS | bc `
  METHODPARAMS="--parallelPso_particles=$PAGENTS --parallelPso_generations=500 --parallelPso_c1=0.5 --parallelPso_c2=0.5 --parallelPso_propagateRate=5 --parallelPso_propagateMethod=NtoN --parallelPso_subCluster=$THREADS --parallelPso_subClusterEnable=$THREADS --parallelPso_pnumber=3 --opt_localsearch=$LOCALSEARCH --opt_distpartitions=$THREADS" 
fi

PROBLEM=$1
DIMENSION=$2
if [ -z "$PROBLEM" ]; then
  echo "PROBLEM PARAM IS EMPTY"
  exit
fi

./DataFitting --opt_model=$MODEL $GLOBALPARAMS $METHODPARAMS $MODELPARAMS --opt_iters=30 --opt_debug=yes
