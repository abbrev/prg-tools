/*
 * prg2bas.c, convert a C64/128 PRG file to a BASIC text file.
 *
 * Copyright 2011, 2012 Christopher Williams
 * Copyright 2023 Fred N. van Kempen, <waltje@varcem.com>
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
#include <ctype.h>
#include <getopt.h>
#include "tokens.h"
#include "version.h"


static int debug;		// debug level
static int verbose;		// output noise level


/*
 * Note: this tokenizer does not handle non-sequential PRG files. Lines must be
 * in the same order as they are found in the input.
 */
static long
getword(FILE *fp)
{
    unsigned int x;
    int n;

    if (feof(fp) || ferror(fp) || ((n = getc(fp)) == EOF))
	return -1;

    x = n;

    if (feof(fp) || ferror(fp) || ((n = getc(fp)) == EOF))
	return -1;

    return x | (n << 8);
}


int
main(int argc, char **argv)
{
    long addr, line;
    char *out_name;
    FILE *fi, *fo;
    int c, quoted;

    /* Set defaults. */
    debug = verbose = 0;
    out_name = NULL;

    /* Process commandline arguments. */
    opterr = 0;
    while ((c = getopt(argc, argv, "do:v")) != EOF) switch (c) {
	case 'd':	// debug level
		debug++;
		break;

	case 'o':	// output-file
		out_name = optarg;
		break;

	case 'v':	// output level
		verbose++;
		break;

	default:
usage:
		fprintf(stderr, "Usage: prg2bas [-dv] [-o outfile] filename\n");
		exit(1);
    }

    /* If we have an output filename, open it. */
    if (out_name != NULL) {
	fo = fopen(out_name, "w");
	if (fo == NULL) {
		fprintf(stderr, "Unable to create output '%s'\n", out_name);
		return(2);
	}
    } else
	fo = stdout;

    /* If we have a filename, use it. */
    if (optind < argc) {
	fi = fopen(argv[optind], "rb");
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
    if (verbose)
	fprintf(stderr, "Load address: $%04lX\n", addr);

    for (;;) {
	/* Get next line address. */
	addr = getword(fi);
	if (addr <= 0)
		break;
	if (debug)
		fprintf(stderr, "$%04lX ", addr);

	/* Get line number. */
	line = getword(fi);
	if (debug)
		fprintf(stderr, "%li ", line);
	fprintf(fo, "%li", line);

	/*
	 * I have seen malformed PRG files with a next-address word,
	 * but then no more line number or line data. So, we check.
	 */
	if (line <= 0)
		break;

	quoted = 0;
	for (;;) {
		/* Get a byte from the input file. */
		if (feof(fi) || ferror(fi) || ((c = fgetc(fi)) == EOF)) {
			if (debug)
				fprintf(stderr, "*** END OF FILE\n");
			goto end;
		}

		/* End of line? */
		if (c == 0x00)
			break;

		/* Handle quotes. */
		if (c == '"')
			quoted = !quoted;

		if (quoted) {
			/* Inside a string, try to escape special characters. */
			if (isprint(c))
				fputc(c, fo);
			else
				fprintf(fo, "{%02X}", c);

			continue;
		}

		if (debug > 1)
			fprintf(stderr, "Token $%02X", c);

		if (c == PREFIX_1) {
			/* Special token: prefix to alternate table 1. */
			if (verbose)
				fprintf(stderr, "Prefix %02X found!\n", c);

			/* Get next byte. */
			if (feof(fi) || ferror(fi) || ((c = fgetc(fi)) == EOF))
				goto end;

			if (debug > 1)
				fprintf(stderr, "Extended token $%02X", c);
#ifdef USE_EXTEND
			if (c < tokens_ce_size)
				fprintf(fo, "%s", tokens_ce[c]);
			else {
#endif
				fprintf(stderr, "*** unknown extended token $%02X $%02X found!\n", PREFIX_1, c);
				fprintf(fo, "{%02X}{%02X}", PREFIX_1, c);
#ifdef USE_EXTEND
			}
#endif
		} else if (c == PREFIX_2) {
			/* Special token: prefix to alternate table 2. */
			if (verbose)
				fprintf(stderr, "Prefix %02X found!\n", c);

			/* Get next byte. */
			if (feof(fi) || ferror(fi) || ((c = fgetc(fi)) == EOF))
				goto end;

			if (debug > 1)
				fprintf(stderr, "Extended token $%02X", c);
#ifdef USE_EXTEND
			if (c < tokens_fe_size)
				fprintf(fo, "%s", tokens_fe[c]);
			else {
#endif
				fprintf(stderr, "*** unknown extended token $%02X $%02X found!\n", PREFIX_2, c);
				fprintf(fo, "{%02X}{%02X}", PREFIX_2, c);
#ifdef USE_EXTEND
			}
#endif
		} else {
			if (c >= TOKEN_BASE)
				fprintf(fo, "%s", tokens[c - TOKEN_BASE]);
			else if (c != '\r')
				fputc(c, fo);
		}
	}

	if (debug)
		fputc('\n', stderr);
	fputc('\n', fo);
    }

end:
    fflush(fo);

    if (fo != stdout)
	fclose(fo);

    return 0;
}
