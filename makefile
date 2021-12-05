main: prueba.c
	@gcc prueba.c -o prueba
run: prueba
	@./prueba $(i)