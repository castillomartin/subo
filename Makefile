program: main.c
	mpicc -o program main.c -lm

run: ./program clean
	mpirun -np 8 ./program 100 200

clean:
	@rm -f % plot
	@touch plot
	
graph:
	./max.sh
	./total.sh