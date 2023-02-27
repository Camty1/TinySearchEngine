#!/bin/bash

if [[ $# == 5 ]]
then
		# valgrind for querier file
		valgrind --leak-check=full --show-leak-kinds=all ./query.bin $1 $2 $3 $4 $5 2> valgrind_err > valgrind_out

		if [[ $(grep 'no leaks are possible' valgrind_err | wc -l) -eq 1 && $(grep '0 errors' valgrind_err | wc -l) -eq 1 ]]
		then
				echo "query.bin passed valgrind check"
		else
				echo "query.bin FAILED valgrind check"
				cat valgrind_err
		fi
		rm -r valgrind_err
		rm -r valgrind_out
else
		echo "usage error: valgrind.sh <pagedir> <indexnm> -q <myqueries.txt> <outputFile>"
fi
