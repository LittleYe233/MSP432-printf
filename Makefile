VERBOSE=1
DEBUG=1

#
# Defines current folder.
#
PROJECT=$(shell basename $(CURDIR))

#
# Defines the part type that this project uses.
#
PART=__MSP432P401R__

#
# The base directory for MSPWare.
#
ROOT=/opt/ti/MSP432Ware_3_50_00_02/driverlib

#
# Include the common make definitions.
#
include ${ROOT}/examples/MSP432P4xx/Makefile.defs

#
# Where to find header files that do not live in the source directory.
#
IPATH=${ROOT}
IPATH+=${ROOT}/inc/
IPATH+=${ROOT}/inc/CMSIS/
IPATH+=${ROOT}/driverlib/MSP432P4xx

CFILES= \
	startup_msp432p401r_gcc.c \
	system_msp432p401r.c \
	${PROJECT}.c \
	printf.c
OFILES=$(patsubst %.c,gcc/%.o,${CFILES})

LIBOTHER=/opt/ti/tirslk_upg_1_00_01/inc/gcc/libtirslk.a

#
# The default rule, which causes the project to be built.
#
all: ${COMPILER}
all: ${COMPILER}/${PROJECT}.axf

#
# The rule to clean out all the build products.
#
clean:
	rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	mkdir -p ${COMPILER}

flash:
	openocd -f interface/xds110.cfg -f board/ti_msp432_launchpad.cfg -c init -c "reset halt; wait_halt; flash write_image erase gcc/${PROJECT}.axf" -c reset -c shutdown

#
# Rules for building the project.
#
#${COMPILER}/${PROJECT}.axf: ${COMPILER}/startup_msp432p401r_${COMPILER}.o
#${COMPILER}/${PROJECT}.axf: ${COMPILER}/system_msp432p401r.o
#${COMPILER}/${PROJECT}.axf: ${COMPILER}/${PROJECT}.o
${COMPILER}/${PROJECT}.axf: ${OFILES}
${COMPILER}/${PROJECT}.axf: ${ROOT}/driverlib/MSP432P4xx/${COMPILER}/msp432p4xx_driverlib.a
${COMPILER}/${PROJECT}.axf: msp432p401r_gcc.ld
SCATTERgcc_${PROJECT}=msp432p401r_gcc.ld
ENTRY_${PROJECT}=resetISR
CFLAGSgcc=-DTARGET_IS_MSP432P4XX

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
