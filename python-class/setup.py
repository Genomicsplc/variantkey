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
        errno = call([
            'py.test',
            '--verbose',
            '--cov=pyvariantkey',
            '--cov-report=term-missing',
            '--cov-config=.coveragerc',
        ])
        raise SystemExit(errno)


setup(
    name='pyvariantkey',
    version='4.3.3',
    keywords=('variantkey variant key genetic genomics'),
    description="VariantKey Python wrapper class",
    long_description=read('../README.md'),
    author='Nicola Asuni',
    author_email='nicola.asuni@genomicsplc.com',
    url='https://github.com/genomicsplc/variantkey',
    license='MIT',
    platforms='Linux',
    packages=find_packages(exclude=['doc', 'test*']),
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: MIT License',
        'Intended Audience :: Developers',
        'Programming Language :: Python',
    ],
    install_requires=[
        'numpy>=1.15.0',
        'variantkey>=4.3.3',
    ],
    extras_require={
        'test': [
            'coverage',
            'pytest',
            'pytest-benchmark',
            'pytest-cov',
            'pycodestyle',
        ],
    },
    cmdclass={'test': RunTests},
)
