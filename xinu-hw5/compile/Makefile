#
#  Makefile for MIPS(EL) XINU.
#

include makeVars

BOOTIMAGE = xinu.boot

LIB       = ../lib

LDFLAGS   = -static --gc-sections -no-warn-mismatch -T ld.script

#-----------------------------------------------------------------------
# source files
#-----------------------------------------------------------------------

# Bootloader
LDR = 

LDRS = start.S flushcache.S

# System
SYS = kprintf.c xtrap.c testcases.c main.c queue.c create.c ready.c \
      resched.c getstk.c kill.c \
      clockinit.c clockintr.c prioritize.c

SYSS = startup.S irq.S ctxsw.S \
       clockup.S interrupt.S

SRC =	${LDR:%.c=../loader/%.c} \
	${SYS:%.c=../system/%.c}

#-----------------------------------------------------------------------
# documentation variables
#-----------------------------------------------------------------------

DOCGEN_CONFIG = Doxyfile.in
DOCGEN_ARGS = -
DOCS := $(shell grep ^OUTPUT_DIRECTORY ${DOCGEN_CONFIG} | awk '{ print $$3 }')

#-----------------------------------------------------------------------
# object files
#-----------------------------------------------------------------------
LDRASM = ${LDRS:%.S=../loader/%.S}
SYSASM = ${SYSS:%.S=../system/%.S}

# Order matters here!  LDRASM (loader/start.S) *must* come first!
KRNOBJ = ${LDRASM:%.S=%.o} ${SYSASM:%.S=%.o} ${SRC:%.c=%.o}

#-----------------------------------------------------------------------
# make targets
#-----------------------------------------------------------------------
all: ${BOOTIMAGE} 

${BOOTIMAGE}: ${KRNOBJ} initialize.o ld.script ${LIB}/libxc.a 
	${LD} -o $@ ${LDFLAGS} $^

clean:
	rm -f *.o ${KRNOBJ} ${BOOTIMAGE}
	rm -rf ${DOCS}

depend:
	$(MAKEDEP) ${INCLUDE} ${SRC} > .deps

depclean:
	rm -f .deps
	touch .deps

realclean: clean depclean
	rm -f vn version
	rm -f Makefile.bak

docs:
	cat ${DOCGEN_CONFIG} | \
		sed -e 's/\[\[XINU_SVN_REV\]\]/"Revision ${XINU_SVN_REV}"/' | \
		${DOCGEN} ${DOCGEN_ARGS}

#-----------------------------------------------------------------------
# targets for files outside of this directory
#-----------------------------------------------------------------------
initialize.o: ../system/initialize.c
	sh mkvers.sh
	VERS=`cat version`
	${CC} -DVERSION=\""`cat version`"\" -c ${CFLAGS} ../system/initialize.c

library: ${LIB}/libxc.a

${LIB}/libxc.a:
	(cd ${LIB}/libxc; make install; cp libxc.a ../)

include .deps
