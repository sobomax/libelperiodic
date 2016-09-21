/*
 * Copyright (c) 2016, sobomax
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "elperiodic.h"
#include "prdic_math.h"
#include "prdic_timespecops.h"

struct prdic_inst {
    double freq_hz;
    struct timespec tfreq_hz;
    struct timespec epoch;
    struct recfilter loop_error;
    struct PFD phase_detector;
    struct timespec last_tclk;
};

static inline int
getttime(struct timespec *ttp, int abort_on_fail)
{

    if (clock_gettime(CLOCK_MONOTONIC, ttp) == -1) {
        if (abort_on_fail)
            abort();
        return (-1);
    }
    return (0);
}

static void
dtime2timespec(double dtime, struct timespec *ttp)
{

    SEC(ttp) = trunc(dtime);
    dtime -= (double)SEC(ttp);
    NSEC(ttp) = round((double)NSEC_IN_SEC * dtime);
}

static void
tplusdtime(struct timespec *ttp, double offset)
{
    struct timespec tp;

    dtime2timespec(offset, &tp);
    timespecadd(ttp, &tp);
}

void *
prdic_init(double freq_hz, double off_from_now)
{
    struct prdic_inst *pip;

    pip = malloc(sizeof(struct prdic_inst));
    if (pip == NULL) {
        goto e0;
    }
    memset(pip, '\0', sizeof(struct prdic_inst));
    pip->freq_hz = freq_hz;
    dtime2timespec(freq_hz, &pip->tfreq_hz);
    if (getttime(&pip->epoch, 0) != 0) {
        goto e1;
    }
    tplusdtime(&pip->epoch, off_from_now);
    recfilter_init(&pip->loop_error, 0.96, 0.0, 0);
    PFD_init(&pip->phase_detector);
    return ((void *)pip);
e1:
    free(pip);
e0:
    return (NULL);
}

int
prdic_procrastinate(void *prdic_inst)
{
    struct prdic_inst *pip;
    struct timespec tsleep, tremain;
    int rval;
    double add_delay, eval;
    struct timespec eptime;

    pip = (struct prdic_inst *)prdic_inst;

    add_delay = freqoff_to_period(pip->freq_hz, 1.0, pip->loop_error.lastval);
    dtime2timespec(add_delay, &tremain);

    do {
        tsleep = tremain;
        memset(&tremain, '\0', sizeof(tremain));
        rval = nanosleep(&tsleep, &tremain);
    } while (rval < 0 && !timespeciszero(&tremain));

    getttime(&eptime, 1);

    timespecsub(&eptime, &pip->epoch);
    timespecmul(&pip->last_tclk, &eptime, &pip->tfreq_hz);

    eval = PFD_get_error(&pip->phase_detector, &pip->last_tclk);

    if (eval != 0.0) {
        recfilter_apply(&pip->loop_error, sigmoid(eval));
    }
    return (0);
}

time_t
prdic_getncycles_ref(void *prdic_inst)
{
    struct prdic_inst *pip;

    pip = (struct prdic_inst *)prdic_inst;
    return (SEC(&pip->last_tclk));
}

void
prdic_free(void *prdic_inst)
{

    free(prdic_inst);
}
