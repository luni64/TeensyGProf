/*	$OpenBSD: gmon.h,v 1.3 1996/04/21 22:31:46 deraadt Exp $	*/
/*	$NetBSD: gmon.h,v 1.5 1996/04/09 20:55:30 cgd Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)gmon.h	8.2 (Berkeley) 1/4/94
 */

/*
 * This file is taken from Cygwin distribution. Please keep it in sync.
 * The differences should be within __MINGW32__ guard.
 */

#ifndef _SYS_GMON_H_
#define _SYS_GMON_H_

// #define size_t unsigned int
// #define u_short unsigned short

#ifndef __P
#define __P(x) x
#endif

/* On POSIX systems, profile.h is a KRB5 header.  To avoid collisions, just
   pull in profile.h's content here.  The profile.h header won't be provided
   by Mingw-w64 anymore at one point. */
#if 0
#include <profile.h>
#else
#ifndef _WIN64
#define _MCOUNT_CALL __attribute__ ((regparm (2)))
extern void _mcount(void);
#else
#define _MCOUNT_CALL
extern void mcount(void);
#endif
#define _MCOUNT_DECL __attribute__((gnu_inline)) __inline__ \
   void _MCOUNT_CALL _mcount_private
#define MCOUNT
#endif

/*
 * Structure prepended to gmon.out profiling data file.
 */
struct gmonhdr {
	size_t	lpc;		/* base pc address of sample buffer */
	size_t	hpc;		/* max pc address of sampled buffer */
	int	ncnt;		    /* size of sample buffer (plus this header) */
	int	version;	  /* version number */
	int	profrate;	  /* profiling clock rate */
	int	spare[3];	  /* reserved */
};
#define GMONVERSION	0x00051879

/*
 * histogram counters are unsigned shorts (according to the kernel).
 */
#define	HISTCOUNTER	unsigned short

/*
 * fraction of text space to allocate for histogram counters here, 1/2
 */
#define	HISTFRACTION	4

/*
 * Fraction of text space to allocate for from hash buckets.
 * The value of HASHFRACTION is based on the minimum number of bytes
 * of separation between two subroutine call points in the object code.
 * Given MIN_SUBR_SEPARATION bytes of separation the value of
 * HASHFRACTION is calculated as:
 *
 *	HASHFRACTION = MIN_SUBR_SEPARATION / (2 * sizeof(short) - 1);
 *
 * For example, on the VAX, the shortest two call sequence is:
 *
 *	calls	$0,(r0)
 *	calls	$0,(r0)
 *
 * which is separated by only three bytes, thus HASHFRACTION is
 * calculated as:
 *
 *	HASHFRACTION = 3 / (2 * 2 - 1) = 1
 *
 * Note that the division above rounds down, thus if MIN_SUBR_FRACTION
 * is less than three, this algorithm will not work!
 *
 * In practice, however, call instructions are rarely at a minimal
 * distance.  Hence, we will define HASHFRACTION to be 2 across all
 * architectures.  This saves a reasonable amount of space for
 * profiling data structures without (in practice) sacrificing
 * any granularity.
 */
#define	HASHFRACTION	4

/*
 * percent of text space to allocate for tostructs with a minimum.
 */
#define ARCDENSITY	 3   /* this is in percentage, relative to text size! */
#define MINARCS		 50
#define MAXARCS		 ((1 << (8 * sizeof(HISTCOUNTER))) - 2)

struct tostruct {
	size_t	selfpc; /* callee address/program counter. The caller address is in froms[] array which points to tos[] array */
	long	count;    /* how many times it has been called */
	u_short	link;   /* link to next entry in hash table. For tos[0] this points to the last used entry */
	// u_short pad;    /* additional padding bytes, to have entries 4byte aligned */
};

/*
 * a raw arc, with pointers to the calling site and
 * the called site and a count.
 */
struct rawarc {
	size_t	raw_frompc;
	size_t	raw_selfpc;
	long	raw_count;
};

/*
 * general rounding functions.
 */
#define ROUNDDOWN(x,y)	(((x)/(y))*(y))
#define ROUNDUP(x,y)	  ((((x)+(y)-1)/(y))*(y))

/*
 * The profiling data structures are housed in this structure.
 */
struct gmonparam {
	int		state;
	u_short		*kcount;    /* histogram PC sample array */
	size_t		kcountsize; /* size of kcount[] array in bytes */
	u_short		*froms;     /* array of hashed 'from' addresses. The 16bit value is an index into the tos[] array */
	size_t		fromssize;  /* size of froms[] array in bytes */
	struct tostruct	*tos; /* to struct, contains histogram counter */
	size_t		tossize;    /* size of tos[] array in bytes */
	long		  tolimit;
	size_t		lowpc;      /* low program counter of area */
	size_t		highpc;     /* high program counter */
	size_t		textsize;   /* code size */
};
extern struct gmonparam _gmonparam;

/*
 * Possible states of profiling.
 */
#define	GMON_PROF_ON	  0
#define	GMON_PROF_BUSY	  1
#define	GMON_PROF_ERROR	  2
#define	GMON_PROF_OFF	  3
#define	GMON_PROF_ABORT	  4

// #define GMON_DISABLE

void _mcleanup(void); /* routine to be called to write gmon.out file */
void _monInit(void); /* initialization routine */

#endif /* !_SYS_GMONH_ */
