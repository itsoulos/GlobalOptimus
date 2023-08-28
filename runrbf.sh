#!/bin/bash
DATAPATH=~/Desktop/ERGASIES/FeatureConstruction2/datasets/tenfolding/
NODES=10

METHOD=Genetic
METHODPARAMS=""
if [ $METHOD = "Minfinder" ]
then
	METHODPARAMS="--minfinder_samples=25 --minfinder_sampling=repulsion"
elif [ $METHOD = "Genetic" ]
then
	METHODPARAMS="--gen_count=200 --gen_maxiters=200 --gen_termination=similarity --gen_lrate=0.01 --gen_selection=tournament "
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

PROBLEM=rbf
NODES=$2

MLPARAMS="--rbf_trainfile=$DATAPATH/$1.train --rbf_testfile=$DATAPATH/$1.test --rbf_nodes=$2"
./GlobalOptimus --opt_problem=$PROBLEM  --opt_method=$METHOD   $METHODPARAMS  $MLPARAMS --opt_iters=30

