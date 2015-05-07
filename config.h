/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* Enable debugging codes. */
/* #undef DEBUG */

/* where is jbterm.conf in */
#define SYSCONFDIR "/etc"

/* Support UTF-8 */
#define JFB_UTF8 1

/* Support other coding system */
#define JFB_OTHER_CODING_SYSTEM 1

/* Support 1bpp */
/* #undef JFB_1BPP */

/* Support 2bpp */
#define JFB_2BPP 1

/* Support 4bpp */
/* #undef JFB_4BPP */

/* Support 8bpp */
#define JFB_8BPP 1

/* Support 15bpp */
#define JFB_15BPP 1

/* Support 16bpp */
#define JFB_16BPP 1

/* Support 24bpp */
#define JFB_24BPP 1

/* Support 32bpp */
#define JFB_32BPP 1

/* Support vga16 */
#define JFB_VGA16FB 1

/* Support packed pixel */
#define JFB_PACKED 1

/* Support pseudo color */
#define JFB_PSEUDOCOLOR 1

/* Support direct color */
/* #undef JFB_DIRECTCOLOR */

/* Support true color */
#define JFB_TRUECOLOR 1

/* Quit if mmap(mmio) is failed */
/* #undef JFB_MMIO_CHECK */

/* Force 8bit color */
/* #undef FORCE_8BIT */

/* gzip program path */
#define JFB_GZIP_PATH "/usr/bin/gunzip"

/* Color gamma effect */
/* #undef JFB_COLOR_GAMMA */

/* Supprt Dimmer (Screen blank) */
/* #undef JFB_ENABLE_DIMMER */

/* Support reverse video */
/* #undef JFB_REVERSEVIDEO */

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the `ioperm' function. */
#define HAVE_IOPERM 1

/* Define to 1 if you have the `m' library (-lm). */
#define HAVE_LIBM 1

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if `st_rdev' is member of `struct stat'. */
#define HAVE_STRUCT_STAT_ST_RDEV 1

/* Define to 1 if your `struct stat' has `st_rdev'. Deprecated, use
   `HAVE_STRUCT_STAT_ST_RDEV' instead. */
#define HAVE_ST_RDEV 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#define HAVE_SYS_WAIT_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define to 1 if `major', `minor', and `makedev' are declared in <mkdev.h>.
   */
/* #undef MAJOR_IN_MKDEV */

/* Define to 1 if `major', `minor', and `makedev' are declared in
   <sysmacros.h>. */
/* #undef MAJOR_IN_SYSMACROS */

/* Name of package */
#define PACKAGE "jfbterm"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Version number of package */
#define VERSION "0.4.7"

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */
