 ont_minimap2
 ============
 
 Oxford Nanopore Technologies wrapper for minimap2
 
 This project uses cmake to build
 * the original minimap2 executable
 * a shared (dynamic-link) library with core alignment functions that is called by ONT software

The project is provided under the Terms and Conditions of the Nanopore Community.
These are available on the Oxford Nanopore Technologies website (https://nanoporetech.com).

This project is shipped with a pre-built version of pthreads-win32 from https://sourceforge.net/projects/pthreads4w/.
to support pthread functionality also under Windows. This software is released under LGPL, see
https://sourceforge.net/p/pthreads4w/code/ci/master/tree/COPYING.

Prerequisites
-------------
* All platforms: cmake and the platform's standard compiler (either gcc, clang or MSVC)
* Windows: Intel Compiler Collection (icc). Current minimap2 might also compile with Microsoft Visual Studio,
  but we have not tested this.

Quick Start
-----------
 
    mkdir build; cd build
    cmake ..
    cmake --build .
