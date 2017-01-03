/* Example was derived from 
   http://openmp.org/mp-documents/OpenMP_Examples_4.0.1.pdf
 */

#include <omp.h>

/* Sub section of iteration with start index and points */
void subdomain(float *x, int istart, int ipoints)
{
  int i;
  for (i = 0; i < ipoints; i++)
      x[istart+i] = 123.456;
}

/* The main iteration control with openmp */
void sub(float *x, int npoints)
{
    int iam, nt, ipoints, istart;
#pragma omp parallel default(shared) private(iam,nt,ipoints,istart)
    {
        iam = omp_get_thread_num();
        nt =  omp_get_num_threads();
        ipoints = npoints / nt;    /* size of partition */
        istart = iam * ipoints;  /* starting array index */
        if (iam == nt-1)     /* last thread may do more */
          ipoints = npoints - istart;
        subdomain(x, istart, ipoints);
    }
}

/* Obviously, main function */
int main()
{
    float array[10000];
    sub(array, 10000);
    return 0;
}
