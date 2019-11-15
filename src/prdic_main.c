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

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "prdic_main.h"
#include "elperiodic.h"
#include "prdic_timespecops.h"
#include "prdic_math.h"
#include "prdic_fd.h"
#include "prdic_pfd.h"
#include "prdic_recfilter.h"
#include "prdic_types.h"
#include "prdic_procchain.h"
#include "prdic_shmtrig.h"
#include "prdic_band.h"
#include "prdic_time.h"

void
_prdic_do_procrastinate(struct prdic_band *pip_ab, int skipdelay)
{
    struct timespec tsleep, tremain;
    int rval;
    double add_delay;
    struct timespec eptime;

    if (skipdelay)
        goto skipdelay;
    add_delay = pip_ab->period * pip_ab->add_delay_fltrd.lastval;
    dtime2timespec(add_delay, &tremain);

    do {
        tsleep = tremain;
        memset(&tremain, '\0', sizeof(tremain));
        rval = nanosleep(&tsleep, &tremain);
    } while (rval < 0 && !timespeciszero(&tremain));

skipdelay:
    getttime(&eptime, 1);

    timespecsub(&eptime, &pip_ab->epoch);
    timespecmul(&pip_ab->last_tclk, &eptime, &pip_ab->tfreq_hz);
}
