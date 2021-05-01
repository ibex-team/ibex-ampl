[![Build Status](https://travis-ci.org/ibex-team/ibex-lib.svg?branch=master)](https://travis-ci.org/ibex-team/ibex-lib)
[![Build status](https://ci.appveyor.com/api/projects/status/9w1wxhvymsohs4gr/branch/master?svg=true)](https://ci.appveyor.com/project/Jordan08/ibex-lib-q0c47/branch/master)

Interface of IBEX with AMPL
========

http://www.ibex-lib.org

1) Install IBEX with cmake and FILIB:
[https://github.com/ibex-team/ibex-lib](https://github.com/ibex-team/ibex-lib)

2) download the ASL librairy and install it with cmake
[https://github.com/ampl/asl](https://github.com/ampl/asl)

3) Then build this project with cmake
example with some path option:

* mkdir build
* cd build
* cmake .. -DCMAKE_INSTALL_PREFIX=../../OUT -DCMAKE_PREFIX_PATH=/dir-where-ibex-is-installed/ -DASL_DIR=/dir-where-ASL-is-installed/

