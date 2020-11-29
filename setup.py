#!/usr/bin/env python

try:
    from setuptools import setup
except ImportError:
    from distutils.core import setup
from distutils.core import Extension
from distutils.command.build_ext import build_ext
import os, sysconfig

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

def get_ext_filename_without_platform_suffix(filename):
    name, ext = os.path.splitext(filename)
    ext_suffix = sysconfig.get_config_var('EXT_SUFFIX')

    if ext_suffix == ext:
        return filename

    ext_suffix = ext_suffix.replace(ext, '')
    idx = name.find(ext_suffix)

    if idx == -1:
        return filename
    else:
        return name[:idx] + ext

class BuildExtWithoutPlatformSuffix(build_ext):
    def get_ext_filename(self, ext_name):
        filename = super().get_ext_filename(ext_name)
        return get_ext_filename_without_platform_suffix(filename)

kwargs = {'name':'ElPeriodic',
      'version':'1.0',
      'description':'Phase-locked userland scheduling library',
      'author':'Maksym Sobolyev',
      'author_email':'sobomax@gmail.com',
      'url':'https://github.com/sobomax/libelperiodic',
      'packages':['elperiodic',],
      'package_dir':{'elperiodic':'python'},
      'ext_modules': get_ex_mod(),
      'cmdclass': {'build_ext': BuildExtWithoutPlatformSuffix}
     }

if __name__ == '__main__':
    setup(**kwargs)
