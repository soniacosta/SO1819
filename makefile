make: compile run

compile: ma.c cv.c
	gcc -Wall ma.c lib/auxiliar.c -o ma
	gcc -Wall cv.c lib/auxiliar.c -o cv

run:
	rm artigos.txt strings.txt
	./ma < test_ma
	./cv < test_cv
