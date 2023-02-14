#!/bin/bash

# valgrind for indexer file
valgrind --leak-check=full --show-leak-kinds=all indexer.bin 2> valgrind_err > valgrind_out
if [[ $(grep 'no leaks are possible' valgrind_err | wc -l) -eq 1 && $(grep '0 errors' valgrind_err | wc -l) -eq 1 ]]
then
		echo "indexer.bin passed valgrind check"
else
		echo "indexer.bin FAILED valgrind check"
		cat valgrind_err
fi

rm -r valgrind_err
rm -r valgrind_out
