# INSTALLATION

Currently, you can just compile everything and use the software.
To compile, enter:

    make

gcc will then build a binary named 'gosm' in the src-directory
This file can be executed from this dir by launching 

    ./gosm

from this directory or from every other path, too.

# REQUIREMENTS

The following libraries are required for running/building gosm.
I'm not sure about minimal version numbers, so that I'll just tell
which ones I'm using:

## RUNNING
for RUNNING gosm you'll need:

libglib2.0-0                            2.20.1-1
libgtk2.0-0                             2.14.7-5
libcairo2                               1.8.6-2+b1
libcurl3                                7.18.2-8.1
libpng12-0                              1.2.27-2
libwebkit-1.0-1                         1.0.1-4+b1
libbz2-1.0 				1.0.5
java					>=1.5		for PDF-Export

## BUILDING
for BUILDING gosm you'll need:

libglib2.0-dev                          2.20.1-1
libgtk2.0-dev                           2.14.7-5 
libcairo2-dev                           1.8.6-2+b1
libcurl4-openssl-dev                    7.18.2-8.1
libpng12-dev                            1.2.27-2
libwebkit-dev                           1.0.1-4
libbz2-dev				1.0.5

on a Debian-based system you can install all build dependencies using this
command line:

    apt-get install libglib2.0-dev libgtk2.0-dev libcairo2-dev libcurl4-openssl-dev libpng12-dev libwebkit-dev libbz2-dev
