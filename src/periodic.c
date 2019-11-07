/*
 * Copyright (c) 2016-2018, sobomax
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
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "elperiodic.h"
#include "prdic_math.h"
#include "prdic_timespecops.h"
#include "prdic_fd.h"
#include "prdic_pfd.h"
#include "prdic_main_fd.h"
#include "prdic_band.h"
#include "prdic_inst.h"
#include "prdic_time.h"

static void
band_init(struct prdic_band *bp, enum prdic_band_det_type dt,
  double freq_hz)
{

    bp->det_type = dt;
    bp->freq_hz = freq_hz;
    bp->period = 1.0 / freq_hz;
    dtime2timespec(bp->period, &bp->tperiod);
    dtime2timespec(freq_hz, &bp->tfreq_hz);
    _prdic_recfilter_init(&bp->loop_error, 0.96, 0.0, 0);
    _prdic_recfilter_init(&bp->add_delay_fltrd, 0.96, bp->period, 0);
    _prdic_recfilter_init(&bp->sysload_fltrd, 0.997, 0.0, 0);
    switch (dt) {
    case PRD_BDET_FREQ:
        _prdic_FD_init(&bp->detector.freq);
        break;
    case PRD_BDET_PHASE:
        _prdic_PFD_init(&bp->detector.phase);
        break;
    default:
        abort();
    }

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
    pip->ab = &pip->bands[0];
    if (getttime(&pip->ab->epoch, 0) != 0) {
        goto e1;
    }
    tplusdtime(&pip->ab->epoch, off_from_now);
    band_init(pip->ab, PRD_BDET_FREQ, freq_hz);
    return ((void *)pip);
e1:
    free(pip);
e0:
    return (NULL);
}

int
prdic_addband(void *prdic_inst, double freq_hz)
{
    struct prdic_inst *pip;
    struct prdic_band *bp, *tbp;
    int i;

    pip = (struct prdic_inst *)prdic_inst;

    bp = malloc(sizeof(struct prdic_band));
    if (bp == NULL)
        return (-1);
    memset(bp, '\0', sizeof(struct prdic_band));
    bp->epoch = pip->bands[0].epoch;
    band_init(bp, pip->bands[0].det_type, freq_hz);
    for (tbp = &pip->bands[0]; tbp->next != NULL; tbp = tbp->next)
        continue;
    bp->id = tbp->id + 1;
    assert(tbp->next == NULL);
    tbp->next = bp;
    return (bp->id);
}

static void
band_set_epoch(struct prdic_band *bp, struct timespec *epoch)
{

    bp->epoch = *epoch;
    switch (bp->det_type) {
    case PRD_BDET_FREQ:
        _prdic_FD_reset(&bp->detector.freq);
        break;
    case PRD_BDET_PHASE:
        _prdic_PFD_reset(&bp->detector.phase);
        break;
    default:
        abort();
    }
}

void
prdic_useband(void *prdic_inst, int bnum)
{
    struct prdic_inst *pip;
    struct prdic_band *bp, *tbp;
    int i;
    struct timespec nepoch, tepoch;

    pip = (struct prdic_inst *)prdic_inst;

    if (bnum == pip->ab->id)
        return;

    for (tbp = &pip->bands[0]; tbp != NULL; tbp = tbp->next) {
        if (tbp->id == bnum)
            break;
    }
    assert(tbp != NULL); /* prdic_useband() requested band is not found */
    SEC(&tepoch) = SEC(&pip->ab->last_tclk);
    NSEC(&tepoch) = 0;
    timespecmul(&nepoch, &tepoch, &pip->ab->tperiod);
    timespecadd(&nepoch, &pip->ab->epoch);
    band_set_epoch(tbp, &nepoch);
    pip->ab = tbp;
}

int
prdic_procrastinate(void *prdic_inst)
{
    struct prdic_inst *pip;

    pip = (struct prdic_inst *)prdic_inst;

    return (_prdic_procrastinate_FD(pip));
    return (0);
}

void
prdic_set_fparams(void *prdic_inst, double fcoef)
{
    struct prdic_inst *pip;

    pip = (struct prdic_inst *)prdic_inst;
    assert(pip->ab->loop_error.lastval == 0.0);
    _prdic_recfilter_adjust(&pip->ab->loop_error, fcoef);
}

void
prdic_set_epoch(void *prdic_inst, struct timespec *tp)
{
    struct prdic_inst *pip;

    pip = (struct prdic_inst *)prdic_inst;
    band_set_epoch(pip->ab, tp);
}

time_t
prdic_getncycles_ref(void *prdic_inst)
{
    struct prdic_inst *pip;

    pip = (struct prdic_inst *)prdic_inst;
    return (SEC(&pip->ab->last_tclk));
}

double
prdic_getload(void *prdic_inst)
{
    struct prdic_inst *pip;

    pip = (struct prdic_inst *)prdic_inst;
    return (pip->ab->sysload_fltrd.lastval);
}

void
prdic_free(void *prdic_inst)
{
    struct prdic_inst *pip;
    struct prdic_band *tbp, *fbp;

    pip = (struct prdic_inst *)prdic_inst;
    for (tbp = pip->bands[0].next; tbp != NULL;) {
        fbp = tbp;
        tbp = tbp->next;
        free(fbp);
    }
    free(prdic_inst);
}
