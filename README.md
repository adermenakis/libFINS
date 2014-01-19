libFINS
=======

This library provides an implementation for the Omron FINS protocol over
othernet. It allows communicating with Omron PLCs under linux and other
*NIX systems. Since most of the time the functionality required is
reading and writing memory blocks to the PLC these are the two functions
provided. In order to add more functionality you have to contribut to the
library since it is under LGPL.

License
=======

LGPL v2.1

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA  02110-1301  USA

Compiling
=======

Execute in the terminal make and this will generate the file libFINS.so
I have included also a CMakeLists.txt in case you are using CMake as your
build environment and would like to add the library into your current
build system.

Installation
=======

Since this library most probably will have to be install manually
or will be statically linked in most applications I did not provide
a make install. Hence just copy the libFINS.so library to /usr/lib/
or /usr/lib64 (depending if you are using 64 bit system) and the FINS.h
header file into /usr/include/.

Usage
=======

Take a look at sample.cpp for information.
Steps:
* Create PLC connection
* Connect
* Read/Write
* Close Connection
* Free connection struct

