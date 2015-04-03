#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# Autoconf version required
AC_PREREQ([2.69])
# Init autoconf and give project information: name, version, bug reporting 
# address, package
AC_INIT(Android Beyond the Stratosphere, 0.0,
                        software-abs@googlegroups.com, abs)

# Autoconf auxiliary files, to not clutter the tree
AC_CONFIG_AUX_DIR([bin])

# Libtool version required
LT_PREREQ([2.4.2])
# Init libtools
LT_INIT([disable-static])

# Init automake with warnings enabled and treated as errors (for Makefile.am
# files) and does not check file tree for GNU compliance
AM_INIT_AUTOMAKE([-Wall -Werror foreign no-define])

# Checks a certain file exists to be sure that the source tree is correct
AC_CONFIG_SRCDIR([src/Architecture/mcs.json])

# Checks for compiler
AC_PROG_CC

# Checks for libraries.

# At this moment, multi-systems is not supported, so checking for std libraries
# and functions is unnecessary

# Checks for header files.
#AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h stdio.h pthread.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_CHECK_HEADER_STDBOOL
#AC_C_INLINE
#AC_TYPE_SIZE_T

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_CHECK_FUNCS([memset socket strchr])

# Global variables
WARNINGCFLAGS="-Wall -Wextra -Werror"
AC_SUBST([WARNINGCFLAGS])

# Generate Makefiles
#AC_CONFIG_FILES([Makefile
#                doc/Makefile
#                doc/sdb_mcs/Makefile
#                src/Makefile
#                src/Architecture/Makefile
#                src/Architecture/Libraries/Makefile
#                src/Architecture/SDB/Makefile
#                ])

AC_CONFIG_FILES([Makefile
                src/Makefile
                src/Architecture/Makefile
                src/Architecture/Libraries/Makefile
                src/Architecture/Libraries/mcs/Makefile
                src/Architecture/SDB/Makefile
                ])

AC_OUTPUT
