make: rm compile run

compile: ma.c cv.c
	gcc -g ma.c lib/auxiliar.c -o ma
	gcc cv.c lib/auxiliar.c -o cv
	gcc sv.c lib/auxiliar.c -o sv
	gcc ag.c lib/auxiliar.c -o ag
	gcc cmp.c lib/auxiliar.c -o cmp
run:
	./ma < test_ma
	./cv < test_cv
	
rm:
	rm -f artigos strings stocks queue sv cv ma vendas 00* ag agregs.txt 2019*

git:
	git add .
	git commit -m "update"
	git push