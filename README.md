[![Build Status@GitHub](https://github.com/sobomax/libelperiodic/workflows/Main%20CI/badge.svg?branch=master)](https://github.com/sobomax/libelperiodic/actions?query=branch%3Amaster++)
[![Build Status](https://travis-ci.com/sobomax/libelperiodic.svg?branch=master)](https://travis-ci.com/sobomax/libelperiodic)
[![Coverage Status](https://coveralls.io/repos/github/sobomax/libelperiodic/badge.svg?branch=master)](https://coveralls.io/github/sobomax/libelperiodic?branch=master)

# libElPeriodic
Library to run frequent periodic tasks.

## Principle of Operation
The libElPeriodic is designed to simplify writing control loops that are
expected to run at constant "tick" intervals with smallest possible overhead
and little or no support from the underlying run time environment.

The library is optimized to align active periods of the control loop
to the set frequency (and optionally phase as well) by applying phase
locked loop design with a proportional phase detector and a low-pass
filter as an error amplifier.

## Usage

Sample usage pattern is demonstrated below. The code block denoted by the square
brackets will be executing 125.5 times a second, untul the value returned by the
`is_runnable()` routine is non-zero. Provided of course that the "logic"
does not take more than 0.01 second to run on average and that OS scheduler
plays the ball.

    #include <elperiodic.h>

    extern int is_runnable(void);

    void
    event_loop(void)
    {
        double frequency = 125.5; /* Hz */
        void *elp;
        int i;

        prd = prdic_init(freq, 0.0);
        assert(prd != NULL);

        while (is_runnable()) {
    //      [----------------------];
    //      [Insert your logic here];
    //      [----------------------];
            prdic_procrastinate(prd);
        }
        prdic_free(prd);
    }

## Story

It came about having to write the same code over and over again in multiple
real-time projects, ranging from game [Digger](https://github.com/sobomax/digger),
RTP relay server [RTPProxy](https://github.com/sippy/rtpproxy). It has also
been recently utilized to replace a heavy-weight (and at the time not portable
to Python 3) "Twisted" framework in the
[Python Sippy B2BUA](https://github.com/sippy/b2bua) project.
