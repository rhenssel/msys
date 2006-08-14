/* cksum -- calculate and print POSIX checksums and sizes of files
   Copyright (C) 92, 1995-2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Written by Q. Frank Xia, qx@math.columbia.edu.
   Cosmetic changes and reorganization by David MacKenzie, djm@gnu.ai.mit.edu.

  Usage: cksum [file...]

  The code segment between "#ifdef CRCTAB" and "#else" is the code
  which calculates the "crctab". It is included for those who want
  verify the correctness of the "crctab". To recreate the "crctab",
  do something like the following:

      cc -DCRCTAB -o crctab cksum.c
      crctab > crctab.h

  This software is compatible with neither the System V nor the BSD
  `sum' program.  It is supposed to conform to POSIX, except perhaps
  for foreign language support.  Any inconsistency with the standard
  (other than foreign language support) is a bug.  */

#include <config.h>

/* The official name of this program (e.g., no `g' prefix).  */
#define PROGRAM_NAME "cksum"

#define AUTHORS "Q. Frank Xia"

#include <stdio.h>
#include <sys/types.h>
#include "system.h"

#if !defined UINT_FAST32_MAX && !defined uint_fast32_t
# define uint_fast32_t unsigned int
#endif

#ifdef CRCTAB

# define BIT(x)	((uint_fast32_t) 1 << (x))
# define SBIT	BIT (31)

/* The generating polynomial is

          32   26   23   22   16   12   11   10   8   7   5   4   2   1
    G(X)=X  + X  + X  + X  + X  + X  + X  + X  + X + X + X + X + X + X + 1

  The i bit in GEN is set if X^i is a summand of G(X) except X^32.  */

# define GEN	(BIT (26) | BIT (23) | BIT (22) | BIT (16) | BIT (12) \
		 | BIT (11) | BIT (10) | BIT (8) | BIT (7) | BIT (5) \
		 | BIT (4) | BIT (2) | BIT (1) | BIT (0))

static uint_fast32_t r[8];

static void
fill_r (void)
{
  int i;

  r[0] = GEN;
  for (i = 1; i < 8; i++)
    r[i] = (r[i - 1] << 1) ^ ((r[i - 1] & SBIT) ? GEN : 0);
}

static uint_fast32_t
crc_remainder (int m)
{
  uint_fast32_t rem = 0;
  int i;

  for (i = 0; i < 8; i++)
    if (BIT (i) & m)
      rem ^= r[i];

  return rem & 0xFFFFFFFF;	/* Make it run on 64-bit machine.  */
}

int
main (void)
{
  int i;

  fill_r ();
  printf ("static uint_fast32_t const crctab[256] =\n{\n  0x00000000");
  for (i = 0; i < 51; i++)
    {
      printf (",\n  0x%08x, 0x%08x, 0x%08x, 0x%08x, 0x%08x",
	      crc_remainder (i * 5 + 1), crc_remainder (i * 5 + 2),
	      crc_remainder (i * 5 + 3), crc_remainder (i * 5 + 4),
	      crc_remainder (i * 5 + 5));
    }
  printf ("\n};\n");
  exit (EXIT_SUCCESS);
}

#else /* !CRCTAB */

# include <getopt.h>
# include "long-options.h"
# include "error.h"
# include "inttostr.h"

/* Number of bytes to read at once.  */
# define BUFLEN (1 << 16)

/* The name this program was run with.  */
char *program_name;

static uint_fast32_t const crctab[256] =
{
  0x00000000,
  0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
  0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6,
  0x2b4bcb61, 0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
  0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9, 0x5f15adac,
  0x5bd4b01b, 0x569796c2, 0x52568b75, 0x6a1936c8, 0x6ed82b7f,
  0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3, 0x709f7b7a,
  0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
  0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58,
  0xbaea46ef, 0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033,
  0xa4ad16ea, 0xa06c0b5d, 0xd4326d90, 0xd0f37027, 0xddb056fe,
  0xd9714b49, 0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
  0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1, 0xe13ef6f4,
  0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
  0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5,
  0x2ac12072, 0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
  0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca, 0x7897ab07,
  0x7c56b6b0, 0x71159069, 0x75d48dde, 0x6b93dddb, 0x6f52c06c,
  0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1,
  0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
  0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b,
  0xbb60adfc, 0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698,
  0x832f1041, 0x87ee0df6, 0x99a95df3, 0x9d684044, 0x902b669d,
  0x94ea7b2a, 0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
  0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2, 0xc6bcf05f,
  0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
  0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80,
  0x644fc637, 0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
  0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f, 0x5c007b8a,
  0x58c1663d, 0x558240e4, 0x51435d53, 0x251d3b9e, 0x21dc2629,
  0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5, 0x3f9b762c,
  0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
  0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e,
  0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65,
  0xeba91bbc, 0xef68060b, 0xd727bbb6, 0xd3e6a601, 0xdea580d8,
  0xda649d6f, 0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
  0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7, 0xae3afba2,
  0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
  0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74,
  0x857130c3, 0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
  0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c, 0x7b827d21,
  0x7f436096, 0x7200464f, 0x76c15bf8, 0x68860bfd, 0x6c47164a,
  0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e, 0x18197087,
  0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
  0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d,
  0x2056cd3a, 0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce,
  0xcc2b1d17, 0xc8ea00a0, 0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb,
  0xdbee767c, 0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
  0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4, 0x89b8fd09,
  0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
  0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf,
  0xa2f33668, 0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

/* Nonzero if any of the files read were the standard input. */
static bool have_read_stdin;

/* Calculate and print the checksum and length in bytes
   of file FILE, or of the standard input if FILE is "-".
   If PRINT_NAME is true, print FILE next to the checksum and size.
   Return true if successful.  */

static bool
cksum (const char *file, bool print_name)
{
  unsigned char buf[BUFLEN];
  uint_fast32_t crc = 0;
  uintmax_t length = 0;
  size_t bytes_read;
  FILE *fp;
  char length_buf[INT_BUFSIZE_BOUND (uintmax_t)];
  char const *hp;

  if (STREQ (file, "-"))
    {
      fp = stdin;
      have_read_stdin = true;
      if (!__MSYS__ && O_BINARY && ! isatty (STDIN_FILENO))
	freopen (NULL, "rb", stdin);
    }
  else
    {
      fp = fopen (file, (O_BINARY ? "rb" : "r"));
      if (fp == NULL)
	{
	  error (0, errno, "%s", file);
	  return false;
	}
    }

  while ((bytes_read = fread (buf, 1, BUFLEN, fp)) > 0)
    {
      unsigned char *cp = buf;

      if (length + bytes_read < length)
	error (EXIT_FAILURE, 0, _("%s: file too long"), file);
      length += bytes_read;
      while (bytes_read--)
	crc = (crc << 8) ^ crctab[((crc >> 24) ^ *cp++) & 0xFF];
      if (feof (fp))
	break;
    }

  if (ferror (fp))
    {
      error (0, errno, "%s", file);
      if (!STREQ (file, "-"))
	fclose (fp);
      return false;
    }

  if (!STREQ (file, "-") && fclose (fp) == EOF)
    {
      error (0, errno, "%s", file);
      return false;
    }

  hp = umaxtostr (length, length_buf);

  for (; length; length >>= 8)
    crc = (crc << 8) ^ crctab[((crc >> 24) ^ length) & 0xFF];

  crc = ~crc & 0xFFFFFFFF;

  if (print_name)
    printf ("%u %s %s\n", (unsigned int) crc, hp, file);
  else
    printf ("%u %s\n", (unsigned int) crc, hp);

  if (ferror (stdout))
    error (EXIT_FAILURE, errno, "-: %s", _("write error"));

  return true;
}

void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    fprintf (stderr, _("Try `%s --help' for more information.\n"),
	     program_name);
  else
    {
      printf (_("\
Usage: %s [FILE]...\n\
  or:  %s [OPTION]\n\
"),
	      program_name, program_name);
      fputs (_("\
Print CRC checksum and byte counts of each FILE.\n\
\n\
"), stdout);
      fputs (HELP_OPTION_DESCRIPTION, stdout);
      fputs (VERSION_OPTION_DESCRIPTION, stdout);
      printf (_("\nReport bugs to <%s>.\n"), PACKAGE_BUGREPORT);
    }
  exit (status);
}

int
main (int argc, char **argv)
{
  int i;
  bool ok;

  initialize_main (&argc, &argv);
  program_name = argv[0];
  setlocale (LC_ALL, "");
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  atexit (close_stdout);

  parse_long_options (argc, argv, PROGRAM_NAME, PACKAGE, VERSION,
		      usage, AUTHORS, (char const *) NULL);
  if (getopt_long (argc, argv, "", NULL, NULL) != -1)
    usage (EXIT_FAILURE);

  have_read_stdin = false;

  if (!__MSYS__ && O_BINARY && ! isatty (STDOUT_FILENO))
    freopen (NULL, "wb", stdout);

  if (optind == argc)
    ok = cksum ("-", false);
  else
    {
      ok = true;
      for (i = optind; i < argc; i++)
	ok &= cksum (argv[i], true);
    }

  if (have_read_stdin && fclose (stdin) == EOF)
    error (EXIT_FAILURE, errno, "-");
  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}

#endif /* !CRCTAB */
