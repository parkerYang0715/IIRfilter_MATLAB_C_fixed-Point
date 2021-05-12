output: main.o 
	gcc main.o -o output
main.o: main.c
	gcc -c main.c
	
clean:
	rm *.o output