METHOD=iPso
#Available local search methods: bfgs, lbfgs, gradient
LOCALSEARCH=bfgs
#Available samplers: uniform, mlp, rbf, nnc, maxwell, triangular
SAMPLER=uniform

if [ $METHOD = "Bfgs" ]
then
	METHODPARAMS="--bfgs_iters=200"
elif [ $METHOD = "Genetic" ]
then
	METHODPARAMS="--gen_rate=0.01  --gen_count=500 --opt_sampler=$SAMPLER --opt_localsearch=$LOCALSEARCH --gen_maxiters=2000"
elif [ $METHOD = "Multistart" ]
then
	METHODPARAMS="--ms_samples=50 --opt_sampler=$SAMPLER --ms_maxiters=100"
elif [ $METHOD = "Lbfgs" ]
then
	METHODPARAMS="--lbfgs_iters=200"
elif [ $METHOD = "iPso" ]
then
	METHODPARAMS="--ipso_particles=200 --ipso_maxgenerations=200 --ipso_localsearch_rate=0.05 --ipso_stoppingrule=best_fitness -ipso_gradientcheck=true --ipso_inertiatype=2"

elif [ $METHOD = "MinCenter" ]
then
	METHODPARAMS="--mincenter_samples=600 --mincenter_centers=100 --mincenter_iterations=100"
elif [ $METHOD = "pDoubleGenetic" ]
then
	METHODPARAMS="--double_chromosomes=200 --double_generations=200  --localsearch_method=bfgs --double_localsearchrate=0.01 --subCluster=1 --subClusterEnable=1 --centers=200 --parallelPropagateMethod=NtoN --pNumber=5  --rbf_samples=100 --rbf_sampler_weights=10 --mlp_samples=1000 --sample_method=triangular double_stoprule=doublebox"
elif [ $METHOD = "NeuralMinimizer" ]
then
	METHODPARAMS="--neural_model=neural --neural_trainmethod=lbfgs"
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
	./GlobalOptimus --opt_problem=$PROBLEM  --opt_method=$METHOD  $METHODPARAMS --iterations=30
else
	./GlobalOptimus --opt_problem=$PROBLEM  --opt_method=$METHOD --opt_dimension=$DIMENSION  $METHODPARAMS --iterations=30
fi
