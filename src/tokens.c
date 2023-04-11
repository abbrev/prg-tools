/*
 * tokens.c, array of C64 tokens.
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
#include "tokens.h"


const char *tokens[128] = {
    "END",		// 80
    "FOR",		// 81
    "NEXT",		// 82
    "DATA",		// 83
    "INPUT#",		// 84
    "INPUT",		// 85
    "DIM",		// 86
    "READ",		// 87

    "LET",		// 88
    "GOTO",		// 89
    "RUN",		// 8a
    "IF",		// 8b
    "RESTORE",		// 8c
    "GOSUB",		// 8d
    "RETURN",		// 8e
    "REM",		// 8f

    "STOP",		// 90
    "ON",		// 91
    "WAIT",		// 92
    "LOAD",		// 93
    "SAVE",		// 94
    "VERIFY",		// 95
    "DEF",		// 96
    "POKE",		// 97

    "PRINT#",		// 98
    "PRINT",		// 99
    "CONT",		// 9a
    "LIST",		// 9b
    "CLR",		// 9c
    "CMD",		// 9d
    "SYS",		// 9e
    "OPEN",		// 9f

    "CLOSE",		// a0
    "GET",		// a1
    "NEW",		// a2
    "TAB(",		// a3
    "TO",		// a4
    "FN",		// a5
    "SPC(",		// a6
    "THEN",		// a7

    "NOT",		// a8
    "STEP",		// a9
    "+",		// aa
    "-",		// ab
    "*",		// ac
    "/",		// ad
    "^",		// ae
    "AND",		// af

    "OR",		// b0
    ">",		// b1
    "=",		// b2
    "<",		// b3
    "SGN",		// b4
    "INT",		// b5
    "ABS",		// b6
    "USR",		// b7

    "FRE",		// b8
    "POS",		// b9
    "SQR",		// ba
    "RND",		// bb
    "LOG",		// bc
    "EXP",		// bd
    "COS",		// be
    "SIN",		// bf

    "TAN",		// c0
    "ATN",		// c1
    "PEEK",		// c2
    "LEN",		// c3
    "STR$",		// c4
    "VAL",		// c5
    "ASC",		// c6
    "CHR$",		// c7

    "LEFT$",		// c8
    "RIGHT$",		// c9
    "MID$",		// ca
    "GO",		// cb
    "{cc}",
    "{cd}",
    "{ce}",
    "{cf}",
	
    "{d0}",
    "{d1}",
    "{d2}",
    "{d3}",
    "{d4}",
    "{d5}",
    "{d6}",
    "{d7}",
	
    "{d8}",
    "{d9}",
    "{da}",
    "{db}",
    "{dc}",
    "{dd}",
    "{de}",
    "{df}",
	
    "{e0}",
    "{e1}",
    "{e2}",
    "{e3}",
    "{e4}",
    "{e5}",
    "{e6}",
    "{e7}",
	
    "{e8}",
    "{e9}",
    "{ea}",
    "{eb}",
    "{ec}",
    "{ed}",
    "{ee}",
    "{ef}",
	
    "{f0}",
    "{f1}",
    "{f2}",
    "{f3}",
    "{f4}",
    "{f5}",
    "{f6}",
    "{f7}",
	
    "{f8}",
    "{f9}",
    "{fa}",
    "{fb}",
    "{fc}",
    "{fd}",
    "{fe}",
    "{pi}"		// ff
};
