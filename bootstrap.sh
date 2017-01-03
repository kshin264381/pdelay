#!/bin/sh
set -e

rm -rf ./config.status ./aclocal.m4 ./configure ./config.status ./config.log ./Makefile ./Makefile.in
rm -rf ./src/*.o ./src/*.a ./src/Makefile ./src/Makefile.in
rm -rf ./src/octree/*.o ./src/octree/*.a ./src/octree/Makefile ./src/octree/Makefile.in

# cpu number?
CPUNUM=$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | tail -1)

# receiving arguments.
ARG=$1

ARG_CC=$(command -v gcc)
ARG_CXX=$(command -v g++)
ARG_LDFLAGS=""
ARG_CFLAGS=""
ARG_CXXFLAGS=""

M4FOLDER=m4.2.69

# Default library dir
BOOST_LIBDIR="/usr/local/lib"

# sushi home brewing directory: change this if they change home directory again...
BREWHOME="/data/KSHIN/local"

DEBUG_COMPILER_FLAGS="-ggdb3"
RELEASE_COMPILER_FLAGS="-ggdb3 -O3"

# Dealing with MKL
MKLROOT="${BREWHOME}/intel/mkl"
MKLLIB="-Wl,--start-group ${MKLROOT}/lib/intel64/libmkl_intel_ilp64.a ${MKLROOT}/lib/intel64/libmkl_core.a ${MKLROOT}/lib/intel64/libmkl_gnu_thread.a -Wl,--end-group -lpthread -lm -ldl"
MKLCFLAGS="-DMKL_ILP64 -fopenmp -m64 -I${MKLROOT}/include"

if [[ $ARG == "sushi.debug" ]] ; then
    DEBUG_COMPILER_FLAGS="-g3 -gdwarf-2"
    ARG_LDFLAGS="-L$BREWHOME/lib64 -Wl,-rpath-link,$BREWHOME/lib64 ${MKLLIB}"
    ARG_CFLAGS="$DEBUG_COMPILER_FLAGS -I$BREWHOME/include ${MKLCFLAGS}"
    ARG_CXXFLAGS="$DEBUG_COMPILER_FLAGS -I$BREWHOME/include ${MKLCFLAGS}"
    ARG_LD_LIBRARY_PATH=$BREWHOME/lib:$BREWHOME/lib64:$LD_LIBRARY_PATH
    M4FOLDER=m4.2.69
    BOOST_LIBDIR=$BREWHOME/lib64
elif [[ $ARG == "sushi.release" ]] ; then
    ARG_LDFLAGS="-L$BREWHOME/lib64 -Wl,-rpath-link,$BREWHOME/lib64 ${MKLLIB}"
    ARG_CFLAGS="$DEBUG_COMPILER_FLAGS -I$BREWHOME/include ${MKLCFLAGS}"
    ARG_CXXFLAGS="$DEBUG_COMPILER_FLAGS -I$BREWHOME/include ${MKLCFLAGS}"
    ARG_LD_LIBRARY_PATH=$BREWHOME/lib:$BREWHOME/lib64:$LD_LIBRARY_PATH
    M4FOLDER=m4.2.69
    BOOST_LIBDIR=$BREWHOME/lib64
elif [[ $ARG == "ubuntu.debug" ]] ; then
    ARG_CC=$HOME/local/bin/gcc
    ARG_CXX=$HOME/local/bin/g++
    ARG_LDFLAGS="${MKLLIB}"
    ARG_CFLAGS="$DEBUG_COMPILER_FLAGS -I$HOME/local/include ${MKLCFLAGS}"
    ARG_CXXFLAGS="$DEBUG_COMPILER_FLAGS -I$HOME/local/include ${MKLCFLAGS}"
    M4FOLDER=m4.2.69
    BOOST_LIBDIR=$HOME/local/lib
elif [[ $ARG == "ubuntu.release" ]] ; then
    ARG_CC=$HOME/local/bin/gcc
    ARG_CXX=$HOME/local/bin/g++
    ARG_LDFLAGS="${MKLLIB}"
    ARG_CFLAGS="$RELEASE_COMPILER_FLAGS -I$HOME/local/include ${MKLCFLAGS}"
    ARG_CXXFLAGS="$RELEASE_COMPILER_FLAGS -I$HOME/local/include ${MKLCFLAGS}"
    M4FOLDER=m4.2.69
    BOOST_LIBDIR=$HOME/local/lib
elif [[ $ARG == "mingw64.release" ]] ; then
    ARG_CFLAGS="$RELEASE_COMPILER_FLAGS -I/mingw64/include -I/local64/dst/boost/include"
    ARG_CXXFLAGS="$RELEASE_COMPILER_FLAGS -I/mingw64/include -I/local64/dst/boost/include"
    M4FOLDER=m4.2.69
    BOOST_ROOT="/local64/dst/boost"
    BOOST_LIBDIR="/local64/dst/boost/lib"
elif [[ $ARG == "mingw64.debug" ]] ; then
    ARG_CFLAGS="$DEBUG_COMPILER_FLAGS -I/mingw64/include -I/local64/dst/boost/include"
    ARG_CXXFLAGS="$DEBUG_COMPILER_FLAGS -I/mingw64/include -I/local64/dst/boost/include"
    M4FOLDER=m4.2.69
    BOOST_ROOT="/local64/dst/boost"
    BOOST_LIBDIR="/local64/dst/boost/lib"
elif [[ $ARG == "cygwin.debug" ]] ; then
    ARG_CFLAGS="$DEBUG_COMPILER_FLAGS -I/usr/local/include"
    ARG_CXXFLAGS="$DEBUG_COMPILER_FLAGS -I/usr/local/include"
    M4FOLDER=m4.2.69
    BOOST_LIBDIR="/usr/local/lib"
elif [[ $ARG == "cygwin.release" ]] ; then
    ARG_CFLAGS="$RELEASE_COMPILER_FLAGS -I/usr/local/include"
    ARG_CXXFLAGS="$RELEASE_COMPILER_FLAGS -I/usr/local/include"
    M4FOLDER=m4.2.69
    BOOST_LIBDIR="/usr/local/lib"
elif [[ $ARG == "release" ]] ; then
    ARG_LDFLAGS="${MKLLIB}"
    ARG_CFLAGS="$RELEASE_COMPILER_FLAGS ${MKLCFLAGS}"
    ARG_CXXFLAGS="$RELEASE_COMPILER_FLAGS ${MKLCFLAGS}"
    M4FOLDER=m4.2.69
elif [[ $ARG == "debug" ]] ; then
    ARG_LDFLAGS="${MKLLIB}"
    ARG_CFLAGS="$DEBUG_COMPILER_FLAGS ${MKLCFLAGS}"
    ARG_CXXFLAGS="$DEBUG_COMPILER_FLAGS ${MKLCFLAGS}"
    M4FOLDER=m4.2.69
else
    echo ""
    echo "Usage: bootstrap.sh <argument>"
    echo ""
    echo "===>>>>>> bootstrap.sh Options list... <<<<<<==="
    echo "  debug:           typical debug bootstrap."
    echo "  release:         typical release bootstrap."
    echo "  ubuntu.debug:    My Ubuntu debug bootstrap."
    echo "  ubuntu.release:  My Ubuntu release bootstrap."
    echo "  mingw64.debug:   Mingw64 debug bootstrap."
    echo "  mingw64.release  Mingw64 release bootstrap."
    echo "  cygwin.debug:    Cygwin debug bootstrap."
    echo "  cygwin.release   Cygwin release bootstrap."
    echo "  sushi.debug:     Sushi debug bootstrap."
    echo "  sushi.release:   Sushi release bootstrap."
    echo "================================================"
    echo ""
    exit -1
fi

ACONF=$(command -v autoreconf)
$ACONF --force --install -I config -I $M4FOLDER

if [[ $ARG == "mingw64.debug" || $ARG == "mingw64.release" ]] ; then
  CC=$ARG_CC \
  CXX=$ARG_CXX \
  CFLAGS=$ARG_CFLAGS \
  CXXFLAGS=$ARG_CFLAGS \
  LD_LIBRARY_PATH=$ARG_LD_LIBRARY_PATH \
  $PWD/configure --with-boost-libdir=$BOOST_LIBDIR
else
  CC=$ARG_CC \
  CXX=$ARG_CXX \
  CFLAGS=$ARG_CFLAGS \
  CXXFLAGS=$ARG_CFLAGS \
  LD_LIBRARY_PATH=$ARG_LD_LIBRARY_PATH \
  $PWD/configure --with-boost-libdir=$BOOST_LIBDIR
fi

if [ -f ./Makefile ] ; then
    make -j $CPUNUM
fi
