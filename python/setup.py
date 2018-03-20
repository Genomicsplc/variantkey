#!/usr/bin/env python

from codecs import open
from os.path import abspath, dirname, join
from subprocess import call
from setuptools import setup, find_packages, Extension, Command
from variantkey import __version__ as VERSION


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
        errno = call(['py.test'])
        raise SystemExit(errno)


setup(
    name='variantkey',
    version=VERSION,
    keywords=('variantkey variant key genetic genomics'),
    description="VariantKey Bindings for Python",
    long_description="Generate and process numerical representations for genetic variants",
    author='Nicola Asuni',
    author_email='nicola.asuni@genomicsplc.com',
    url='https://github.com/genomicsplc/variantkey',
    packages=find_packages(exclude=['docs', 'tests*']),
    ext_modules=[
        Extension('variantkey', [
             '../src/binsearch.c',
            '../src/farmhash64.c',
            '../src/rsidvar64.c',
            '../src/rsidvar128.c',
            '../src/variantkey.c',
            'variantkey/pyvariantkey.c'
        ], include_dirs=['../src', 'variantkey'], extra_compile_args=["-O3"])
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
