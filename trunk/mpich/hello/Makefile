
MPICC=/usr/local/mpich/bin/mpicc
MPIRUN=/usr/local/mpich/bin/mpirun

nettest: nettest.c log.c
	$(MPICC) nettest.c log.c -o nettest

test: nettest push
	$(MPIRUN) -np 3 -machinefile hosts nettest

push: nettest
	scp nettest armyant:mpich/hello
	scp nettest ee1100pc08:mpich/hello

clean:
	rm nettest
