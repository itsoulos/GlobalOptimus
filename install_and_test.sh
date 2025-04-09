problems=("bf1" "bf2" "bf3" "branin" "camel" "cigar" "Easom" "elp"  "gkls2100" "gkls250" "gkls350" "goldstein" "griewank10" "griewank2" "hansen" "hartman3" "hartman6" "shekel5" "shekel7" "shekel10" "shubert")
userchoice=$1
if [[ ${problems[@]} =~ $userchoice ]]
then
	(qmake GlobalOptimus.pro;make; ./single_test.sh $userchoice)
else
  echo "$userchoice not found in the list of problems"
fi
