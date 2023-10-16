#Available optimization methods: Bfgs,Lbfgs,Genetic,Multistart,iPso,NeuralMinimizer,DifferentialEvolution
METHOD=Genetic
#Available local search methods: bfgs, lbfgs, gradient
LOCALSEARCH=bfgs
#Available samplers: uniform, mlp, rbf, maxwell, triangular, kmeans
SAMPLER=uniform
#Available stopping rules: maxiters, doublebox, similarity
TERMINATION=doublebox

if [ $METHOD = "Bfgs" ]
then
	METHODPARAMS="--bfgs_iters=200"
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

	METHODPARAMS="--gen_lrate=0.01 --gen_srate=0.1 --gen_mrate=0.05 --gen_tsize=8 --gen_selection=tournament --gen_crossover=double --gen_mutation=double --gen_termination=$TERMINATION --gen_count=200 --opt_sampler=$SAMPLER --opt_localsearch=$LOCALSEARCH --gen_maxiters=200"
elif [ $METHOD = "Multistart" ]
then

#ms_samples: number of multistart samples
#ms_maxiters: maximum number of iterations
#ms_termination: termination rule (maxiters|doublebox|similarity)

	METHODPARAMS="--ms_samples=50 --opt_sampler=$SAMPLER --ms_maxiters=100 --opt_localsearch=$LOCALSEARCH -ms_termination=$TERMINATION"
elif [ $METHOD = "Lbfgs" ]
then
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
	METHODPARAMS="--ipso_particles=200 --ipso_maxgenerations=200 --ipso_localsearch_rate=0.05 --ipso_stoppingrule=best_fitness -ipso_gradientcheck=true --ipso_inertiatype=2 --opt_sampler=$SAMPLER --opt_localsearch=$LOCALSEARCH"

elif [ $METHOD = "NeuralMinimizer" ]
then
	METHODPARAMS="--neural_model=neural --neural_trainmethod=bfgs"
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
