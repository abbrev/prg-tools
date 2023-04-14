/*
 * tokens.h
 * Copyright 2013 Christopher Williams
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
#ifndef _TOKENS_H_
# define _TOKENS_H_


#define TOKEN_BASE	0x80
# define TOKEN_REM	0x8f

#define PREFIX_1	0xce		// prefix for V3.5 and V7.0
#define PREFIX_2	0xfe		// prefix for V7.0 and V7.1


extern const char	*tokens[];
extern int		tokens_size;

#ifdef USE_EXTEND
extern const char	*tokens_ce[];
extern int		tokens_ce_size;

extern const char	*tokens_fe[];
extern int		tokens_fe_size;
#endif


#endif	/*_TOKENS_H*/
