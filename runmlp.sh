#!/bin/bash
DATAPATH=~/Desktop/ERGASIES/FeatureConstruction2/datasets/tenfolding/
NODES=10

#Available local search methods: bfgs, lbfgs, gradient, nelderMead, adam
LOCALSEARCH=bfgs
#Available samplers: uniform, mlp, rbf, maxwell, triangular, kmeans
SAMPLER=uniform
#Available stopping rules: maxiters, doublebox, similarity
TERMINATION=doublebox

METHOD=Genetic
METHODPARAMS=""
if [ $METHOD = "Genetic" ]
then
	METHODPARAMS="--gen_count=200 --gen_maxiters=200 --gen_termination=similarity --gen_lrate=0.01 --opt_debug=yes"
elif [ $METHOD = "iPso" ]
then
#ipso_particles: number of pso particles
#ipso_maxgenerations: maximum number of generations allowed
#ipso_localsearch_rate: the rate of local search applications
#ipso_stoppingrule: the stopping rule used (mean_fitness,best_fitness,doublebox,ali)
#ipso_gradientcheck: usage of gradient rejection rule (true|false)
#ipso_inertiatype: selection of inertia calcuation mechanism

	METHODPARAMS="--ipso_particles=200 --ipso_maxgenerations=200 --ipso_localsearch_rate=0.05 --ipso_stoppingrule=best_fitness -ipso_gradientcheck=true --ipso_inertiatype=1 --opt_sampler=$SAMPLER --opt_localsearch=$LOCALSEARCH"

elif [ $METHOD = "Pso" ]
then
	METHODPARAMS="--pso_localsearch_rate=0.01"
elif [ $METHOD = "Multistart" ]
then
	METHODPARAMS=="--multistart_samples=25"
elif [ $METHOD = "Search" ]
then
	METHODPARAMS=="--Search_method=3"
elif [ $METHOD = "ParallelDe" ]
then
	METHODPARAMS="--parde_agents=100 --parde_islands=2"
elif [ $METHOD = "Bfgs" ]
then
	METHODPARAMS="--localsearch_method=bfgs"
fi

PROBLEM=mlp
NODES=$2

MLPARAMS="--mlp_trainfile=$DATAPATH/$1.train --mlp_testfile=$DATAPATH/$1.test --mlp_nodes=$2"
./GlobalOptimus --opt_problem=$PROBLEM  --opt_method=$METHOD   $METHODPARAMS  $MLPARAMS --opt_iters=1

