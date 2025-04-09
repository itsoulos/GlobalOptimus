problems=("bf1" "bf2" "bf3" "branin" "camel" "cigar" "Easom" "elp"  "gkls2100" "gkls250" "gkls350" "goldstein" "griewank10" "griewank2" "hansen" "hartman3" "hartman6" "shekel5" "shekel7" "shekel10" "shubert")
for str in ${problems[@]}; do
	./GlobalOptimus --opt_problem=$str --opt_enabletesting=yes
done
