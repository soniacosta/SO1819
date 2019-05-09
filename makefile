make: rm compile run

compile: ma.c cv.c
	gcc ma.c lib/auxiliar.c -o ma
	gcc cv.c lib/auxiliar.c -o cv
	gcc sv.c lib/auxiliar.c -o sv
	gcc ag.c lib/auxiliar.c -o ag

run:
	./ma < test_ma
	./cv < test_cv
	
rm:
	rm -f artigos.txt strings.txt stocks.txt queue sv cv ma vendas.txt 00* 2019* ag agregs.txt

git:
	git add .
	git commit -m "update"
	git push