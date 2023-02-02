#!/bin/bash

for FILE in *.bin
do
		valgrind --leak-check=full --show-leak-kinds=all $FILE 2> valgrind_err > valgrind_out
		if [[ $(grep 'no leaks are possible' valgrind_err | wc -l) -eq 1 && $(grep '0 errors' valgrind_err | wc -l) -eq 1 ]]
		then
				echo "$FILE passed valgrind check"
		else
				echo "$FILE FAILED valgrind check"
				cat valgrind_err
		fi
done

rm -r valgrind_err
rm -r valgrind_out
