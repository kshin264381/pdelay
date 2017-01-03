#!/bin/sh
gcc -v -o sql_test ./sql_test.c -lsqlite3 -L/usr/lib
