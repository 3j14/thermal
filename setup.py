from setuptools import setup, Extension


extensions = [
    Extension("thermal", sources=["src/thermal.cpp"], language="c++")
]

setup(ext_modules=extensions, name="thermal")