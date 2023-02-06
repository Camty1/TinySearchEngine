#!/bin/bash

# crawler file for tasks 1-5
valgrind --leak-check=full --show-leak-kinds=all crawler_1_5.bin 2> valgrind_err > valgrind_out
if [[ $(grep 'no leaks are possible' valgrind_err | wc -l) -eq 1 && $(grep '0 errors' valgrind_err | wc -l) -eq 1 ]]
then
		echo "crawler_1_5.bin passed valgrind check"
else
		echo "crawler_1_5.bin FAILED valgrind check"
		cat valgrind_err
fi

# crawler file for task 6
valgrind --leak-check=full --show-leak-kinds=all crawler.bin https://thayer.github.io/engs50/ ../pages 1 2> valgrind_err_2 > valgrind_out_2
if [[ $(grep 'no leaks are possible' valgrind_err_2 | wc -l) -eq 1 && $(grep '0 errors' valgrind_err_2 | wc -l) -eq 1 ]]
then
		echo "crawler_1_5.bin passed valgrind check"
else
		echo "crawler_1_5.bin FAILED valgrind check"
		cat valgrind_err_2
fi

rm -r valgrind_err
rm -r valgrind_out
rm -r valgrind_err_2
rm -r valgrind_out_2
