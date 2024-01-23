include ../Make.defines

PROGS =	prodcons1 prodcons2 prodcons3 prodcons4 prodcons5 prodcons6 \
		prodcons7 test1 test2 test3 test4 os_prodcons8 os_cv_if os_cv_while os_fish

.PHONY : all
all:	${PROGS}

prodcons1:	prodcons1.o
		${CC} ${CFLAGS} -o $@ prodcons1.o ${LIBS}

prodcons2:	prodcons2.o
		${CC} ${CFLAGS} -o $@ prodcons2.o ${LIBS}

prodcons3:	prodcons3.o
		${CC} ${CFLAGS} -o $@ prodcons3.o ${LIBS}

prodcons4:	prodcons4.o
		${CC} ${CFLAGS} -o $@ prodcons4.o ${LIBS}

prodcons5:	prodcons5.o
		${CC} ${CFLAGS} -o $@ prodcons5.o ${LIBS}

prodcons6:	prodcons6.o
		${CC} ${CFLAGS} -o $@ prodcons6.o ${LIBS}

prodcons7:	prodcons7.o
		${CC} ${CFLAGS} -o $@ prodcons7.o ${LIBS}

os_prodcons8: os_prodcons8.o
		${CC} ${CFLAGS} -o $@  os_prodcons8.o ${LIBS}

os_cv_if: os_cv_if.o
		${CC} ${CFLAGS} -o $@  os_cv_if.o ${LIBS}

os_cv_while: os_cv_while.o
		${CC} ${CFLAGS} -o $@  os_cv_while.o ${LIBS}

os_fish: os_fish.o
		${CC} ${CFLAGS} -o $@  os_fish.o ${LIBS}

test1:	test1.o
		${CC} ${CFLAGS} -o $@ test1.o ${LIBS}

test2:	test2.o
		${CC} ${CFLAGS} -o $@ test2.o ${LIBS}

test3:	test3.o
		${CC} ${CFLAGS} -o $@ test3.o ${LIBS}

test4:	test4.o
		${CC} ${CFLAGS} -o $@ test4.o ${LIBS}

clean:
		rm -f ${PROGS} ${CLEANFILES}
