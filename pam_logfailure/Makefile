

pam:
	gcc -Wunused -c -fPIC -DHAVE_SHADOW -O2 pam_logfailure.c
	ld -o pam_logfailure.so -s -lpam -lcrypt --shared pam_logfailure.o

%.o: %.c
	gcc -Wunused -c -fPIC -DHAVE_SHADOW -O2 $< -o $@

%.so: %.o
	ld -o $@ -s -lpam -lcrypt --shared $<


