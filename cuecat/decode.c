/*
 *cuecat.c  22 Sept 2000  Stephen Satchell -- add interpretation of cue 
 *cuecat.c  21 Sept 2000  by Stephen Satchell
 *
 * This program takes keystrokes from stdin, interprets them according
 * to the research described in the programmer's essay on decoding
 * the CueCat's output, and emits information on stdout.
 *
 * The intent of this source module is to illustrate the techniques
 * described in the associated essay regarding the decoding and
 * decryption of Digital Convergence Inc. CueCat barcode wands.
 *
 * USE AT YOUR OWN RISK.  NO WARRANTY IS OFFERED.
 *
 * Copyright 2000 Stephen Satchell -- all rights reserved
 *
 * Anyone is free to link to this source code, and to download and
 * use the source code for non-commercial purposes.
 *  
 * Reproduction, but not for profit, of this source code is hereby granted.  
 * Any commercial use of the software requires that appropriate rights be 
 * secured from the author at satch (at) fluent-access (dot) com.
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define XOR ('C')

static int c;
static char string[80];

extern char *encstring;
extern int offset;

static int
get_cat (void)
{
	//c = getchar ();
	c = *(encstring + offset++);
	switch (c)
	{
		default:
			{
				return;
			}
		case '+':
			{
				c = ',';
				return;
			}
		case '/':
			{
				c = '-';
				return;
			}
	}
}

static int
get_64 (void)
{
	static
		const
		char map[66] = "abcdefghijklmnopqrstuvwxyz"

		"ABCDEFGHIJKLMNOPQRSTUVWXYZ" "0123456789" ",-\0";
	int w = 0;
	const char *p;

	for (p = map; *p && ((unsigned) (*p) != (unsigned) (c)); p++)
	{
	}
	if (*p)
	{
		w = p - map;
		get_cat ();
	}
	return w;
}

int
get_byte (int digit_number)
{
	int w = 0;
	static int hold;

	if (!hold)
	{
		if ((c == '.') || (c == '\n') || (c == EOF))
		{
			return 0;
		}
	}
	switch (digit_number)
	{
		case 0:
			{
				w = get_64 () << 2;
				hold = get_64 ();
				w = w | ((hold >> 4) & 0x03);
				hold = hold & 0x0F;
				return w ^ XOR;
			}
		case 1:
			{
				w = ((hold << 4) & 0xF0);
				hold = get_64 ();
				w = w | ((hold >> 2) & 0x0F);
				hold = hold & 0x03;
				return w ^ XOR;
			}
		case 2:
			{
				w = ((hold << 6) & 0xC0);
				hold = get_64 ();
				w = w | ((hold >> 0) & 0x3F);
				hold = 0;
				return w ^ XOR;
			}
		default:
			{
				return 0;
			}
	}
}

void
swallow_dot (void)
{
	if (c == '.')
	{
		get_cat ();
	}
	return;
}

void
process_code (void (*f) (int))
{
	int digit;
	int limit = 60;

	digit = 0;
	memset(string, 0, sizeof(string));
	while ((c != '.') && (c != '\n') && (c != EOF) && limit)
	{
		(*f) (get_byte (digit));
		digit = (digit + 1) % 3;
		limit--;
	}

	if (digit)
	{
		(*f) (get_byte (digit));
#if 0
		printf (" [%d]", digit);
#endif
	}
	swallow_dot ();
	return;
}

void
print_code (int code)
{
	char *p;
	if (isprint (code))
	{
		printf ("%c", code);
	}
	else
	{
		printf ("\\x%02.2X", code);
	}
	p = strchr(string, 0);
	if (p - string < sizeof(string) - 2)
	{
		*p++ = code;
		*p   = '\0';
	}

	return;
}

int
decodecat (void)
{
	int digit;
	int cue;
	int cuetype;

	printf("Stringy: %s\n", encstring);
	get_cat ();

	/* swallow the header dot */
	printf ("\nheader=");
	process_code (print_code);

	/*get the device ID */
	printf ("\ndevid=");
	process_code (print_code);

	/*get the code type */
	printf ("\ncodetype=");
	process_code (print_code);
	cue = ((string[0] == 'C') && (string[1] == 'C'));
	cuetype = string[2];
#if 0
	printf(" %1.64s", string);
#endif

	/*get the barcode */
	printf ("\nbarcode=");
	process_code (print_code);
	if (cue)
	{
		static const
			char xlate[108] =
			" !\"#$%&'()"  /* 00-09 */
			"*+,-./0123"   /* 10-19 */
			"456789:;<="   /* 20-29 */
			">?@ABCDEFG"   /* 30-39 */
			"HIJKLMNOPQ"   /* 40-49 */
			"RSTUVWXYZ["   /* 50-59 */
			"\\]^_`abcde"  /* 60-69 */
			"fghijklmno"   /* 70-79 */
			"pqrstuvwxy"   /* 80-89 */
			"z{|}~"        /* 90-99 */
			;
		char *p;
		int i;

		printf(", cue=C");
		p = strchr(xlate, cuetype);
		if (p)
		{
			printf(" %02.2d", p - xlate);
		}
		for (i=0; (i < sizeof(string)) && (string[i]); i++)
		{
			p = strchr(xlate, string[i]);
			if (p)
			{
				printf(" %02.2d", p - xlate);
			}
		}
	}

	/*get anything that follows */
	printf ("\ntrailer=");
	process_code (print_code);
	putchar ('\n');
}
