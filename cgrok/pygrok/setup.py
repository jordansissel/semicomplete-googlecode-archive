from distutils.core import setup, Extension
setup(name="pygrok", version="1.0",
    ext_modules=[Extension("pygrok", ["pygrok.cpp"])])
