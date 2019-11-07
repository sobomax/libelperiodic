/*
 * Copyright (c) 2019 Sippy Software, Inc., http://www.sippysoft.com
 * Copyright (c) 2016-2018, Maksym Sobolyev <sobomax@sippysoft.com>
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
//#define PRD_DEBUG 1
#if defined(PRD_DEBUG)
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "elperiodic.h"
#include "prdic_math.h"
#include "prdic_timespecops.h"
#include "prdic_fd.h"
#include "prdic_pfd.h"
#include "prdic_main_pfd.h"
#include "prdic_band.h"
#include "prdic_inst.h"
#include "prdic_time.h"

int
_prdic_procrastinate_PFD(struct prdic_inst *pip)
{
    struct timespec tsleep, tremain;
    int rval;
    double eval;
    struct timespec eptime;
#if defined(PRD_DEBUG)
    static long long nrun = -1;

    nrun += 1;
#endif

    eval = pip->ab->loop_error.lastval - 0.5;
    if (eval <= 0.0)
        goto skipdelay;

    dtime2timespec(15.0 * eval / pip->ab->freq_hz, &tremain);

    do {
        tsleep = tremain;
        memset(&tremain, '\0', sizeof(tremain));
        rval = nanosleep(&tsleep, &tremain);
    } while (rval < 0 && !timespeciszero(&tremain));

skipdelay:
    getttime(&eptime, 1);

    timespecsub(&eptime, &pip->ab->epoch);
    timespecmul(&pip->ab->last_tclk, &eptime, &pip->ab->tfreq_hz);

    eval = PFD_get_error(&pip->ab->detector.phase, &pip->ab->last_tclk);
    if (eval != 0.0) {
        eval = pip->ab->loop_error.lastval + erf(eval - pip->ab->loop_error.lastval);
        _prdic_recfilter_apply(&pip->ab->loop_error, eval);
    }

#if defined(PRD_DEBUG)
    fprintf(stderr, "run=%lld raw_error=%f filtered_error=%f\n", nrun, eval, pip->ab->loop_error.lastval);
    fflush(stderr);
#endif

#if defined(PRD_DEBUG)
    fprintf(stderr, "error=%f\n", eval);
    if (eval == 0.0 || 1) {
        fprintf(stderr, "last=%lld target=%lld\n", SEC(&pip->ab->last_tclk),
          SEC(&pip->ab->detector.phase.target_tclk));
    }
    fflush(stderr);
#endif

    return (0);
}
