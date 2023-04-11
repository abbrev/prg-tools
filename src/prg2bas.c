/*
 * prg2bas.c, convert a C64 PRG file to a BASIC text file.
 * Copyright 2011, 2012 Christopher Williams
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <stdio.h>
#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
#endif
#include "tokens.h"


/*
 * Note: this tokenizer does not handle non-sequential PRG files. Lines must be
 * in the same order as they are found in the input.
 */
static long
getword(FILE *f)
{
    unsigned int x;
    int n;

    n = getc(f);
    if (n < 0)
	return -1;
    x = n;
    n = getc(f);
    if (n < 0)
	return -1;

    return x | (n << 8);
}


int
main()
{
    long addr, line;
    int quoted;
    int c;

#ifdef _WIN32
    /* Avoid CR/LF translations in our (binary) input file. */
    _setmode(_fileno(stdin), _O_BINARY);
#endif

    /* Get load address. */
    addr = getword(stdin);
    fprintf(stderr, "Load address: 0x%04lx\n", addr);

    for (;;) {
	/* Get next line address. */
	addr = getword(stdin);
	if (addr <= 0)
		break;

	/* Get line number. */
	line = getword(stdin);
	printf("%li ", line);

	quoted = 0;
	for (;;) {
		c = getchar();
		if (c == 0)
			break;

		if (c < 0)
			goto end;

		if (c == '"')
			quoted = !quoted;

		if (!quoted && c >= 0x80) {
			printf("%s", tokens[c - 0x80]);
//			printf("TOKEN{0x%02x}", c);
		} else {
			putchar(c);
		}
	}
	putchar('\n');
    }

end:
    return 0;
}
