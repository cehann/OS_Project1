all:
	gcc main.c process.c -o main -lrt
clean:
	rm -f main
