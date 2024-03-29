
OPTION(BUILD_PYTHON_PACKAGE "Build the Python user interface?" ON)
OPTION(BUILD_CXX_INTERFACE "Build the C++ user interface?" ON)
OPTION(BUILD_F90_INTERFACE "Build the Fortran 90 user interface?" ON)
OPTION(BUILD_MATLAB_TOOLBOX "Build the Matlab Toolbox?" OFF)

OPTION(CANTERA_BUILD_WITH_F2C "Use f2c versions of 3rd party numerical routines" ON)

OPTION (BUILD_LAPACK "Build the lapack library?" 1)
OPTION (HAVE_SUNDIALS "Use Sundials from LLNL?" 0)

# SET(PYTHON_CMD ${PYTHON_EXE})

SET(CANTERA_VERSION "1.7.2")


SET( WITH_PURE_FLUIDS  1 )
SET( WITH_LATTICE_SOLID 1 )
SET( WITH_METAL 1 )
SET( WITH_STOICH_SUBSTANCE 1 )
SET( WITH_IDEAL_SOLUTIONS 0 )
SET(WITH_ELECTROLYTES 0 )
