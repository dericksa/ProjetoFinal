all:
	@echo "Compilando..."
	gcc -o imgexec main.c pdi.c -lm
	@echo "Executando..."
	./imgexec
	@echo "Sucesso"