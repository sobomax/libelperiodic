#!/usr/bin/env python

try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup
from distutils.core import Extension
import os

elp_srcs = ['src/periodic.c', 'src/prdic_math.c', \
 'src/prdic_fd.c', \
 'src/prdic_pfd.c', \
 'src/prdic_main_fd.c', 'src/prdic_main_pfd.c', \
 'src/prdic_main.c', \
 'src/prdic_recfilter.c', 'src/prdic_shmtrig.c', \
 'src/prdic_sign.c']

module1 = Extension('_elperiodic', sources = elp_srcs, \
    extra_link_args = ['-Wl,--version-script=src/Symbol.map',])

def get_ex_mod():
    if 'NO_PY_EXT' in os.environ:
        return None
    return [module1]

with open("README.md", "r") as fh:
    long_description = fh.read()

kwargs = {
      'name':'ElPeriodic',
      'version':'1.2',
      'description':'Phase-locked userland scheduling library',
      'long_description': long_description,
      'long_description_content_type': "text/markdown",
      'author':'Maksym Sobolyev',
      'author_email':'sobomax@gmail.com',
      'url':'https://github.com/sobomax/libelperiodic',
      'packages':['elperiodic',],
      'package_dir':{'elperiodic':'python'},
      'ext_modules': get_ex_mod(),
      'python_requires': '>=2.7',
      'classifiers': [
            'License :: OSI Approved :: BSD License',
            'Operating System :: POSIX',
            'Programming Language :: C',
            'Programming Language :: Python'
      ]
}

if __name__ == '__main__':
    setup(**kwargs)
