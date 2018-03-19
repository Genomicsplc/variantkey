#!/usr/bin/env python

from codecs import open
from subprocess import call
from setuptools import setup, find_packages, Extension, Command

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
    version='2.0.6',
    keywords=('variantkey variant key genetic genomics'),
    description="VariantKey Bindings for Python",
    long_description=open('../README.md', 'r').read(),
    author='Nicola Asuni',
    author_email='nicola.asuni@genomicsplc.com',
    url='https://github.com/genomicsplc/variantkey',
    packages=find_packages('src', exclude=['docs', 'tests*']),
    package_dir={'': 'src'},
    ext_modules=[
        Extension('variantkey', [
            '../src/binsearch.c',
            '../src/farmhash64.c',
            '../src/rsidvar64.c',
            '../src/rsidvar128.c',
            '../src/variantkey.c',
            'src/pyvariantkey.c'
        ], extra_compile_args=["-O3"])
    ],
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: MIT License',
        'Intended Audience :: Developers',
        'Programming Language :: C',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 3',
    ],
    extras_require = {
        'test': [
            'coverage',
            'pytest',
            'pytest-benchmark',
            'pytest-cov',
            'pytest-pep8',
        ],
    },
    cmdclass = {'test': RunTests},
)
