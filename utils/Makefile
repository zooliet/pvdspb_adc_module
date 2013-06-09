ROOTDIR = ../..
-include $(ROOTDIR)/Rules.make

#INCLUDE = LINUXKERNEL_INSTALL_DIR
COMMON_DEP = \
						 mdebug.h
INCLUDE_DIR = ${APP_KERNEL_INCLUDE_DIR}/usr/include
PROGS = md4 mm4 md2 mm2 md1 mm1 \
				ttyloop sloop fpga \
				kmd gmd i2cr i2cw \
				ldisp keypad lcd bmp \
				gdisp gps_tty gps
CFLAGS = -Wall -Werror

all : ${PROGS}

disp:
	@echo ${LINUXKERNEL_INSTALL_DIR}
	@echo ${CSTOOL_PREFIX}

md4: md4.c mem.c ${COMMON_DEP}
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o md4 md4.c
mm4: mm4.c mem.c ${COMMON_DEP}
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o mm4 mm4.c
md2: md2.c mem.c ${COMMON_DEP}
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o md2 md2.c
mm2: mm2.c mem.c ${COMMON_DEP}
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o mm2 mm2.c
md1: md1.c mem.c ${COMMON_DEP}
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o md1 md1.c
mm1: mm1.c mem.c ${COMMON_DEP}
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o mm1 mm1.c

ttyloop: ttyloop.c ${COMMON_DEP}
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o ttyloop ttyloop.c
sloop: sloop.c ${COMMON_DEP}
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o sloop sloop.c
fpga: fpga.c ${COMMON_DEP}
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o fpga fpga.c

kmd: kmd.c
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o kmd kmd.c
gmd: gmd.c ${COMMON_DEP}
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o gmd gmd.c

i2cr: i2cr.c
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o i2cr i2cr.c
i2cw: i2cw.c
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o i2cw i2cw.c

ldisp: ldisp.c
	${CSTOOL_PREFIX}gcc -I${INCLUDE_DIR} ${CFLAGS} -o ldisp ldisp.c
keypad: keypad.c
	${CSTOOL_PREFIX}gcc -I${INCLUDE_DIR} ${CFLAGS} -o keypad keypad.c
lcd: lcd.c
	${CSTOOL_PREFIX}gcc -I${INCLUDE_DIR} ${CFLAGS} -o lcd lcd.c
bmp: bmp.c
	${CSTOOL_PREFIX}gcc -I${INCLUDE_DIR} ${CFLAGS} -o bmp bmp.c

gdisp: gdisp.c
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o gdisp gdisp.c
gps: gps.c
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o gps gps.c
gps_tty: gps_tty.c
	${CSTOOL_PREFIX}gcc -I ${INCLUDE_DIR} ${CFLAGS} -o gps_tty gps_tty.c

install:
	install -d $(EXEC_DIR)/usr/local/bin
	install ${PROGS} ${EXEC_DIR}/usr/local/bin
	cp *.wav ${EXEC_DIR}/usr/local/bin
	cp *.bmp ${EXEC_DIR}/usr/local/bin

install2:
	cp ${PROGS} ~/

clean :
		rm -f *.o

