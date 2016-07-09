#include <sys/time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "periodic.h"
#include "prdic_math.h"

#define SEC(x)      ((x)->tv_sec)
#define NSEC(x)     ((x)->tv_nsec)
#define NSEC_IN_SEC 1000000000L

#define timespec2dtime(s) ((double)SEC(s) + \
  (double)NSEC(s) / (double)NSEC_IN_SEC)
#define timespeciszero(t) (SEC(t) == 0 && NSEC(t) == 0)
#define timespecadd(vvp, uvp)                     \
        do {                                      \
                SEC(vvp) += SEC(uvp);             \
                NSEC(vvp) += NSEC(uvp);           \
                if (NSEC(vvp) >= NSEC_IN_SEC) {   \
                        SEC(vvp)--;               \
                        NSEC(vvp) -= NSEC_IN_SEC; \
                }                                 \
        } while (0)

struct mtime {
    double dtime;
    struct timespec ttime;
};

struct prdic_inst {
    double freq_hz;
    struct mtime epoch;
    struct recfilter loop_error;
    struct PFD phase_detector;
};

static double
getdtime(void)
{
    struct timespec tp;

    if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1)
        return (-1);

    return timespec2dtime(&tp);
}

static int
getmtime(struct mtime *mtp)
{

    if (clock_gettime(CLOCK_MONOTONIC, &mtp->ttime) == -1)
        return (-1);

    mtp->dtime = timespec2dtime(&mtp->ttime);
    return (0);
}

static void
dtime2mtimespec(double dtime, struct timespec *mtime)
{

    SEC(mtime) = trunc(dtime);
    dtime -= (double)SEC(mtime);
    NSEC(mtime) = round((double)NSEC_IN_SEC * dtime);
}

static void
mplusdtime(struct mtime *mtp, double offset)
{
    struct timespec tp;

    mtp->dtime += offset;
    dtime2mtimespec(offset, &tp);
    timespecadd(&mtp->ttime, &tp);
}

void *
prdic_init(double freq_hz, double off_from_now)
{
    struct prdic_inst *pip;

    pip = malloc(sizeof(struct prdic_inst));
    if (pip == NULL) {
        return (NULL);
    }
    memset(pip, '\0', sizeof(struct prdic_inst));
    pip->freq_hz = freq_hz;
    if (getmtime(&pip->epoch) != 0) {
        goto e0;
    }
    mplusdtime(&pip->epoch, off_from_now);
    recfilter_init(&pip->loop_error, 0.96, 0.0, 0);
    PFD_init(&pip->phase_detector, 0.0);
    return ((void *)pip);
e0:
    free(pip);
    return (NULL);
}

int
prdic_procrastinate(void *prdic_inst)
{
    struct prdic_inst *pip;
    struct timespec tsleep, tremain;
    int rval;
    double add_delay, eptime, clk, eval;

    pip = (struct prdic_inst *)prdic_inst;

    add_delay = freqoff_to_period(pip->freq_hz, 1.0, pip->loop_error.lastval);
    dtime2mtimespec(add_delay, &tremain);

    do {
        tsleep = tremain;
        memset(&tremain, '\0', sizeof(tremain));
        rval = nanosleep(&tsleep, &tremain);
    } while (rval < 0 && !timespeciszero(&tremain));

    eptime = getdtime();

    clk = (eptime - pip->epoch.dtime) * pip->freq_hz;

    eval = PFD_get_error(&pip->phase_detector, clk);

    if (eval != 0.0) {
        recfilter_apply(&pip->loop_error, sigmoid(eval));
    }
    return (0);
}

void
prdic_free(void *prdic_inst)
{

    free(prdic_inst);
}
