#
#  Makefile for MIPS(EL) XINU.
#

include makeVars

BOOTIMAGE = xinu.boot

LIB       = ../lib

LDFLAGS   = -static --gc-sections -no-warn-mismatch -T ld.script

$(VERBOSE).SILENT:

#-----------------------------------------------------------------------
# source files
#-----------------------------------------------------------------------

# Bootloader
LDR = 

LDRS = start.S flushcache.S

# System
SYS = kprintf.c xtrap.c testcases.c main.c queue.c create.c ready.c \
	resched.c getstk.c kill.c \
	prioritize.c clockinit.c clockintr.c \
	semcreate.c semfree.c wait.c signal.c semcount.c mutex.c

SYSS = startup.S irq.S ctxsw.S \
	clockup.S interrupt.S \
	testAndSet.S

SRC =	${LDR:%.c=../loader/%.c} \
	${SYS:%.c=../system/%.c}

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
	@echo -e "\tLinking" $@
	${LD} -o $@ ${LDFLAGS} $^

%.o: %.c
	@echo -e "\tCompiling" $@
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.S
	@echo -e "\tAssembling" $@
	$(CC) $(CFLAGS) -o $@ $<

clean:
	@echo -e "\tCleaning all objects"
	rm -f *.o ${KRNOBJ} ${BOOTIMAGE}

depclean:
	@echo -e "\tCleaning dependencies"
	rm -f .deps
	touch .deps

realclean: clean depclean
	@echo -e "\tCleaning EVERYTHING"
	rm -f vn version
	rm -f Makefile.bak

depend:
	@echo -e "\tBuilding dependencies"
	${MAKEDEP} -I../include $(LDRASM) $(SYSASM) $(SRC) > .deps

#-----------------------------------------------------------------------
# targets for files outside of this directory
#-----------------------------------------------------------------------
initialize.o: ../system/initialize.c
	@echo -e "\tCompiling" $@
	sh mkvers.sh
	VERS=`cat version`
	${CC} -DVERSION=\""`cat version`"\" -c ${CFLAGS} ../system/initialize.c

library: ${LIB}/libxc.a

${LIB}/libxc.a:
	@echo -e "\tBuilding" $@
	(cd ${LIB}/libxc; make install; cp libxc.a ../)

include .deps
