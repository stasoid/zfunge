
SUBDIRS=test

bin_PROGRAMS=zfunge

zfunge_SOURCES=zfunge.c funge_ip.c funge_ip.h funge_stack.c funge_stack.h \
	funge_space.c funge_space.h funge_commands.c funge_commands.h \
	dimension.c befunged93.c minizfunge.c \
\
	print_BASE.c print_FILE.c print_FIXP.c print_NULL.c \
	print_PERL.c print_SOCK.c print_STRN.c print_SUBR.c \
	print_TERM.c print_TOYS.c

pkgdata_DATA=rom2.fl ROMA.fl
EXTRA_DIST=$(pkgdata_DATA) KNOWN-BUGS
