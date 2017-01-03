/**
 *
 * sqlite3_mt.h
 *
 * Override functions for sqlite3 multithreading implementation.
 * --> MUST BE USED WITH OpenMP library!!!!
 *
**/

#ifndef __sqlite3_mt_h__
#define __sqlite3_mt_h__

#include <sqlite3.h>
#include <stdio.h>

/* MSVC doesn't have unistd.h */
#ifndef _MSC_VER
#include <unistd.h>
#endif

/* MSVC doesn't have sleep() so we need to override it somehow */
#ifdef _MSC_VER
#include <windows.h>
#define sleep(milisecond) Sleep(milisecond) 
#endif

#include <omp.h>

static const int SQLTM_COUNT = 60000;
static const int SQLTM_TIME  = 5;

// Overriding sqlite3_open
int sqlite3_open_mt(const char* dbfname, sqlite3** db)
{
    int n, rc; n = 0;

    do {
        rc = sqlite3_open(dbfname, db);

        if ( (rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED) ) {
            n++;
            sleep(SQLTM_TIME);
        }
    } while ( (n < SQLTM_COUNT) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)) );

    if ( rc != SQLITE_OK ) {
    #pragma omp critical
    {
        fprintf(stderr, "sqlite3_open_mt error: %s\n", sqlite3_errmsg(*db));
    }

    }

    return rc;
}

// Overriding sqlite3_open16
int sqlite3_open16_mt(const char* dbfname, sqlite3** db)
{
    int n, rc; n = 0;

    do {
        rc = sqlite3_open16(dbfname, db);

        if ( (rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED) ) {
            n++;
            sleep(SQLTM_TIME);
        }
    } while ( (n < SQLTM_COUNT) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)) );

    if ( rc != SQLITE_OK ) {
    #pragma omp critical
    {
        fprintf(stderr, "sqlite3_open_mt error: %s\n", sqlite3_errmsg(*db));
    }

    }

    return rc;
}

// Overriding sqlite3_open_v2
int sqlite3_open_v2_mt(
    const char* dbfname, sqlite3** db, int flags, const char* zVfs)
{
    int n, rc; n = 0;

    do {
        rc = sqlite3_open_v2(dbfname, db, flags, zVfs);

        if ( (rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED) ) {
            n++;
            sleep(SQLTM_TIME);
        }
    } while ( (n < SQLTM_COUNT) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)) );

    if ( rc != SQLITE_OK ) {
    #pragma omp critical
    {
        fprintf(stderr, "sqlite3_open_v2_mt error: %s\n", sqlite3_errmsg(*db));
    }

    }

    return rc;
}

// Overriding sqlite3_prepare
int sqlite3_prepare_mt(
    sqlite3* db, const char* zSql, int nByte,
    sqlite3_stmt **stmt, const char** pztail)
{
    int n, rc; n = 0;

    do {
        rc = sqlite3_prepare(db, zSql, nByte, stmt, pztail);

        if ( (rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED) ) {
            n++;
            sleep(SQLTM_TIME);
        }
    } while ( (n < SQLTM_COUNT) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)) );

    if ( rc != SQLITE_OK ) {
    #pragma omp critical
    {
        fprintf(stderr, "sqlite3_prepare_mt error [%d]: %s\n",
            rc,
            sqlite3_errmsg(db));
    }

    }

    return rc;
}

// Overriding sqlite3_prepare_v2
int sqlite3_prepare_v2_mt(
    sqlite3* db, const char* zSql, int nByte,
    sqlite3_stmt **stmt, const char** pztail)
{
    int n, rc; n = 0;

    do {
        rc = sqlite3_prepare_v2(db, zSql, nByte, stmt, pztail);

        if ( (rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED) ) {
            n++;
            sleep(SQLTM_TIME);
        }
    } while ( (n < SQLTM_COUNT) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)) );

    if ( rc != SQLITE_OK ) {
    #pragma omp critical
    {
        fprintf(stderr, "sqlite3_prepare_v2_mt error [%d]: %s\n",
            rc,
            sqlite3_errmsg(db));
    }

    }

    return rc;
}

// Overriding sqlite3_prepare16
int sqlite3_prepare16_mt(
    sqlite3* db, const char* zSql, int nByte,
    sqlite3_stmt **stmt, const void** pztail)
{
    int n, rc; n = 0;

    do {
        rc = sqlite3_prepare16(db, zSql, nByte, stmt, pztail);

        if ( (rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED) ) {
            n++;
            sleep(SQLTM_TIME);
        }
    } while ( (n < SQLTM_COUNT) && ((rc == SQLITE_BUSY) || (rc == SQLITE_LOCKED)) );

    if ( rc != SQLITE_OK ) {
    #pragma omp critical
    {
        fprintf(stderr, "sqlite3_prepare16_mt error: %s\n", sqlite3_errmsg(db));
    }

    }

    return rc;
}

// Overriding sqlite3_prepare16_v2
int sqlite3_prepare16_v2_mt(
    sqlite3* db, const char* zSql, int nByte,
    sqlite3_stmt **stmt, const void** pztail)
{
    int n, rc; n = 0;

    do {
        rc = sqlite3_prepare16_v2(db, zSql, nByte, stmt, pztail);

        if ( rc == SQLITE_BUSY || rc == SQLITE_LOCKED ) {
            n++;
            sleep(SQLTM_TIME);
        }
    } while ( n < SQLTM_COUNT && (rc == SQLITE_BUSY || rc == SQLITE_LOCKED) );

    if ( rc != SQLITE_OK ) {
    #pragma omp critical
    {
        fprintf(stderr, "sqlite3_prepare16_v2_mt error: %s\n", sqlite3_errmsg(db));
    }

    }

    return rc;
}

// sqlite3_step override
int sqlite3_step_mt(sqlite3_stmt* stmt)
{
    int rc, n; n = 0;

    do {
        rc = sqlite3_step(stmt);

        if( rc == SQLITE_LOCKED ) {
            rc = sqlite3_reset(stmt);
            n++;
            sleep(SQLTM_TIME);
        }
        else if (rc == SQLITE_BUSY) {
            sleep(SQLTM_TIME);
            n++;
        }
    } while(
        (n < SQLTM_COUNT) && (rc != SQLITE_DONE) && \
        (rc == SQLITE_BUSY || rc == SQLITE_ROW ));

    if ( rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW) {
    #pragma omp critical
    {
        fprintf(stderr, "sqlite3_step_mt error [%d]\n", rc);
    }

    }

    return rc;
}

// sqlite3_finalize override
int sqlite3_finalize_mt(sqlite3_stmt* stmt)
{
    int rc, n; n = 0;

    do {
        rc = sqlite3_finalize(stmt);

        if( rc == SQLITE_LOCKED ) {
            rc = sqlite3_reset(stmt);
            n++;
            sleep(SQLTM_TIME);
        }
        else {
            if( (rc == SQLITE_BUSY) ) {
                sleep(SQLTM_TIME);
                n++;
            }
        }
    } while( (n < SQLTM_COUNT) &&
        (rc == SQLITE_BUSY || rc == SQLITE_LOCKED));

    if ( rc != SQLITE_OK ) {
    #pragma omp critical
    {
        fprintf(stderr, "sqlite3_finalize_mt error [%d]\n", rc);
    }

    }

    return rc;
}

// sqlite3_exec override
int sqlite3_exec_mt(
    sqlite3* db, const char* sqlcmd,
    int (*callback)(void*, int, char**, char**),
    void* cbarg,
    char** errmsg)
{
    int rc, n; n = 0;

    do {
        rc = sqlite3_exec(db, sqlcmd, callback, cbarg, errmsg);

        if( rc == SQLITE_LOCKED ) {
            n++;
            sleep(SQLTM_TIME);
        }
        else {
            if( (rc == SQLITE_BUSY) ) {
                sleep(SQLTM_TIME);
                n++;
            }
        }
    } while( (n < SQLTM_COUNT) &&
        (rc == SQLITE_BUSY || rc == SQLITE_LOCKED));

    if ( rc != SQLITE_OK ) {
    #pragma omp critical
    {
        fprintf(
            stderr, "sqlite3_exec_mt error [%d]: %s\n",
            rc,
            sqlite3_errmsg(db));
    }

    }

    return rc;
}



#else

#endif /* Include guard */
