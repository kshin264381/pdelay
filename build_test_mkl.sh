#!/bin/sh

gcc -o test_mkl ./test_mkl.c \
    -DMKL_ILP64 -fopenmp -m64 -I${MKLROOT}/include \
    -L$MKLROOT/intel64/lib \
    -Wl,--start-group \
    ${MKLROOT}/lib/intel64/libmkl_intel_ilp64.a \
    ${MKLROOT}/lib/intel64/libmkl_core.a \
    ${MKLROOT}/lib/intel64/libmkl_gnu_thread.a \
    -Wl,--end-group -lpthread -lm -ldl

