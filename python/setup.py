#!/usr/bin/env python

from codecs import open
from os.path import dirname, join
from subprocess import call
from setuptools import setup, find_packages, Extension, Command

def read(fname):
    return open(join(dirname(__file__), fname)).read()

class RunTests(Command):
    """Run all tests."""
    description = 'run tests'
    user_options = []

    def initialize_options(self):
        pass

    def finalize_options(self):
        pass

    def run(self):
        """Run all tests!"""
        errno = call(['py.test', '--verbose'])
        raise SystemExit(errno)


setup(
    name='variantkey',
    version='3.0.1',
    keywords=('variantkey variant key genetic genomics'),
    description="VariantKey Bindings for Python",
    long_description=read('../README.md'),
    author='Nicola Asuni',
    author_email='nicola.asuni@genomicsplc.com',
    url='https://github.com/genomicsplc/variantkey',
    license='MIT',
    platforms='Linux',
    packages=find_packages(exclude=['docs', 'tests*']),
    ext_modules=[
        Extension('variantkey', [
             '../src/binsearch.c',
            '../src/rsidvar.c',
            '../src/variantkey.c',
            'variantkey/pyvariantkey.c',
        ],
        include_dirs=[
            '../src',
            'variantkey',
        ],
        extra_compile_args=[
            "-O3",
            "-pedantic",
            "-std=c99",
            "-Wall",
            "-Wextra",
            "-Wno-strict-prototypes",
            "-Wunused-value",
            "-Wcast-align",
            "-Wundef",
            "-Wformat-security",
            "-Wshadow",
            "-I../src",
        ])
    ],
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: MIT License',
        'Intended Audience :: Developers',
        'Programming Language :: C',
        'Programming Language :: Python',
    ],
    extras_require={
        'test': [
            'coverage',
            'pytest',
            'pytest-benchmark',
            'pytest-cov',
            'pytest-pep8',
        ],
    },
    cmdclass={'test': RunTests},
)
