"""
Compiles C Python module.
"""
from distutils.core import setup, Extension
from distutils.version import StrictVersion
from distutils.unixccompiler import UnixCCompiler
from numpy import get_include as np_includes
from numpy.version import version as np_version

# We need Numpy 1.8 or greater
assert StrictVersion(np_version) > StrictVersion("1.8")

COMPILER = UnixCCompiler(verbose=2)

MYRIAD_CPYTHON_DEFS = [("_POSIX_C_SOURCE", "200809L"),
                       ("NPY_NO_DEPRECATED_API", "NPY_1_7_API_VERSION")]

MYRIAD_COMM = Extension("myriad_comm",
                        define_macros=MYRIAD_CPYTHON_DEFS,
                        extra_compile_args=["-std=gnu99"],
                        include_dirs=["/usr/include", np_includes()],
                        library_dirs=["/usr/lib/"],
                        libraries=["rt", "pthread"],
                        sources=["pymyriad_communicator.c",
                                 "myriad_communicator.c"])

PYMYRIAD = Extension("pymyriad",
                     define_macros=MYRIAD_CPYTHON_DEFS,
                     extra_compile_args=["-std=gnu99"],
                     include_dirs=["/usr/include", np_includes()],
                     library_dirs=["/usr/lib/"],
                     libraries=["rt", "pthread"],
                     sources=["pymyriad.c", "pymyriadobject.c"])

% for module in dependencies:
    % if module.__name__ != "MyriadObject":
${module.__name__.upper()} = Extension("${module.__name__.lower()}",
                  define_macros=MYRIAD_CPYTHON_DEFS,
                  extra_compile_args=["-std=gnu99"],
                  include_dirs=["/usr/include", np_includes()],
                  library_dirs=["/usr/lib/"],
                  libraries=["rt", "pthread"],
                  sources=["py${module.__name__.lower() + '.c'}"])
    % endif
% endfor

setup(name="pymyriad",
      version="1.0",
      description="Myriad CPython package",
      ext_modules=[
              MYRIAD_COMM,
              PYMYRIAD,
% for module in dependencies:
    % if module.__name__ != "MyriadObject":
              ${module.__name__.upper()},
    % endif
% endfor
      ])
