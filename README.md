
Interface of IBEX with AMPL
========

[http://www.ibex-lib.org](http://www.ibex-lib.org)

1) Install IBEX with cmake with the interval library FILIB and a linear solver (soplex, CLP,...):

[https://github.com/ibex-team/ibex-lib](https://github.com/ibex-team/ibex-lib)

* `cmake .. -DINTERVAL_LIB=filib `


2) Then build this project with cmake.

Here, an example of some path option:

* `mkdir build`
* `cd build`
* `cmake .. -DCMAKE_INSTALL_PREFIX=<path/where/you/want/to/install> -DCMAKE_PREFIX_PATH=<path/to/ibex> `
* `make install`


If the ASL library is already installed, you can give the path to cmake:
* by setting the environment variable `ASL_DIR` to the correct path
* or by using the cmake command-line option `-DASL_DIR=<path/to/ASL>`

Otherwise ASL will be automatically installed from the archive of the folder "ibex-ampl/3rd". 

See [https://github.com/ampl/asl](https://github.com/ampl/asl)

AMPL IDE
====

To use IbexOpt inside the [AMPL IDE](https://ampl.com/products/ide/), copy/paste the binary "ibexopt" in the folder amplide (in the same place as the other solver already available in AMPL IDE).


