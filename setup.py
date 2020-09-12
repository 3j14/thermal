from setuptools import setup, Extension


extensions = [
    Extension("thermal._pylibthermal", sources=["src/pylibthermal.cpp"], language="c++")
]

setup(ext_modules=extensions, name="thermal")