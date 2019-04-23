make: compile run

compile: ma.c
	gcc -Wall ma.c lib/auxiliar.c -o ma

run:
	rm artigos.txt strings.txt
	./ma < test_ma
