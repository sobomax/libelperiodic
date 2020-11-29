#!/usr/bin/env python

from distutils.core import setup, Extension

elp_srcs = ['src/periodic.c', 'src/prdic_math.c', \
 'src/prdic_fd.c', \
 'src/prdic_pfd.c', \
 'src/prdic_main_fd.c', 'src/prdic_main_pfd.c', \
 'src/prdic_main.c', \
 'src/prdic_recfilter.c', 'src/prdic_shmtrig.c', \
 'src/prdic_sign.c']

module1 = Extension('_elperiodic', sources = elp_srcs, \
    extra_link_args = ['-Wl,--version-script=src/Symbol.map',])

kwargs = {'name':'ElPeriodic',
      'version':'1.0',
      'description':'Phase-locked userland scheduling library',
      'author':'Maksym Sobolyev',
      'author_email':'sobomax@gmail.com',
      'url':'https://github.com/sobomax/libelperiodic',
      'packages':['elperiodic',],
      'package_dir':{'elperiodic':'python'},
      'ext_modules': [module1]
     }

if __name__ == '__main__':
    setup(**kwargs)
