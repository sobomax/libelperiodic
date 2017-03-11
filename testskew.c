#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//extern int optind;

#include "elperiodic.h"

static void
usage(void)
{

    fprintf(stderr, "usage: testskew [-v] freq ival\n");
    exit(1);
}

int
main(int argc, char * const argv[])
{
    void *prd;
    int i, ch, vflag;
    double freq, duration, skew;
    time_t ncycles;

    vflag = 0;
    while ((ch = getopt(argc, argv, "v")) != -1) {
         switch (ch) {
         case 'v':
             vflag = 1;
             break;

         case '?':
         default:
             usage();
         }
    }
    argc -= optind;
    argv += optind;
    if (argc != 2) {
        usage();
    }
    freq = atof(argv[0]);
    duration = atof(argv[1]);
    prd = prdic_init(freq, 0.0);
    for (i = 0; i < (freq * duration); i++) {
        prdic_procrastinate(prd);
        ncycles = prdic_getncycles_ref(prd);
        if (vflag != 0) {
            printf("%lld\r", (long long)ncycles);
            fflush(stdout);
        }
    }
    if (vflag != 0) {
         printf("\n");
    }
    skew = 1.0 - ((double)ncycles / (freq * duration));
    printf("skew %f%%\n", skew * 100.0);

    return (0);
}
