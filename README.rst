====================================
 A tiny FTP server / client example
====================================

About
------
This was done as a university project at UCL, LLN, Belgium,
for the course "Distributed Application Design".
It is *not* meant to be a complete FTP implementation,
but rather an example of how to use (or not use?) the socket API.

Licensing
----------
This code is licensed under a BSD-like license, further
information in COPYING.

Build requirements
-------------------
We tried to use POSIX compliant code as far as possible,
we tested it only under Linux, though.
In order to build this code you need a recent version of
libedit, a BSD licensed readline-like library.
Furthermore to build the doxygen documentation you need
*doxygen* installed.


Building & running
-------------------

Building::

  user:$ git clone git@github.com:mfischer/lln-ftp-thingy.git
  user:$ cd lln-ftp-thingy && make

Running::

  user:$ ./server
  user:$ ./client [IP]:[Port]


Building Doxygen HTML Documentation
------------------------------------

Building Documentation::

  user:$ git clone git@github.com:mfischer/lln-ftp-thingy.git
  user:$ cd lln-ftp-thingy && make doc

After the documentation can be found in the *html/* subdirectory.
