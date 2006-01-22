

pam:
	gcc -c -Wall -fPIC -DHAVE_SHADOW -O2 pam.c
	ld -o pam_captcha.so -s -lpam -lcrypt --shared pam.o


