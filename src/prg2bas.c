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
#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
#endif
#include <getopt.h>
#include "tokens.h"
#include "version.h"


#ifdef _DEBUG
static int debug = 0;		// debug level
#endif


/*
 * Note: this tokenizer does not handle non-sequential PRG files. Lines must be
 * in the same order as they are found in the input.
 */
static long
getword(FILE *fp)
{
    unsigned int x;
    int n;

    n = getc(fp);
    if (n < 0)
	return -1;
    x = n;
    n = getc(fp);
    if (n < 0)
	return -1;

    return x | (n << 8);
}


int
main(int argc, char **argv)
{
    long addr, line;
    int quoted;
    int c;
    FILE *fi, *fo;
    char *out_name;

    /* Set defaults. */
#ifdef _DEBUG
    debug = 0;
#endif
    out_name = NULL;

    /* Process commandline arguments. */
    opterr = 0;
    while ((c = getopt(argc, argv, "do:")) != EOF) switch (c) {
	case 'd':	// debug-level
#ifdef _DEBUG
		debug++;
#else
		fprintf(stderr, "Debugging not compiled in.\n");
#endif
		break;

	case 'o':	// output-file
		out_name = optarg;
		break;

	default:
usage:
		fprintf(stderr, "Usage: prg2bas [-d] [-o outfile] filename\n");
		exit(1);
    }

    /* If we have an output filename, open it. */
    if (out_name != NULL) {
	fo = fopen(out_name, "wb");
	if (fo == NULL) {
		fprintf(stderr, "Unable to create output '%s'\n", out_name);
		return(2);
	}
    } else
	fo = stdout;

    /* If we have a filename, use it. */
    if (optind < argc) {
	fi = fopen(argv[optind], "r");
	if (fi == NULL) {
		fprintf(stderr, "Unable to open input '%s'\n", argv[optind]);
		if (fo != stdout) {
			fclose(fo);
			remove(out_name);
		}

		return(3);
	}
	optind++;
    } else {
#ifdef _WIN32
	/* Avoid CR/LF translations in our (binary) input file. */
	_setmode(_fileno(stdin), _O_BINARY);
#endif
	fi = stdin;
    }

    /* No more arguments. */
    if (optind != argc)
	goto usage;

    /* Get load address. */
    addr = getword(fi);

    fprintf(stderr, "Load address: 0x%04lx\n", addr);

    for (;;) {
	/* Get next line address. */
	addr = getword(fi);
	if (addr <= 0)
		break;

	/* Get line number. */
	line = getword(fi);
	fprintf(fo, "%li", line);

	quoted = 0;
	for (;;) {
		c = fgetc(fi);
		if (c == 0)
			break;

		if (c < 0)
			goto end;

		if (c == '"')
			quoted = !quoted;

		if (!quoted && c >= 0x80) {
			fprintf(fo, "%s", tokens[c - 0x80]);
#ifdef _DEBUG
			if (debug)
				fprintf(stderr, "TOKEN{0x%02x}", c);
#endif
		} else {
			fputc(c, fo);
		}
	}

	fputc('\n', fo);
    }

    if (fo != stdout)
	fclose(fo);

end:
    return 0;
}
