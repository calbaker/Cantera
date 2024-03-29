/**
 *  @file funcs.h Header for a file containing miscellaneous
 *                numerical functions. 
 */
/*
 *  $Author: hkmoffa $
 *  $Date: 2010-01-03 18:46:26 -0600 (Sun, 03 Jan 2010) $
 *  $Revision: 368 $
 *
 *  Copyright 2001-2003 California Institute of Technology
 *  See file License.txt for licensing information
 *
 */
#ifndef CT_FUNCS_H
#define CT_FUNCS_H

#include "ct_defs.h"
 
namespace Cantera {

  //! Linearly interpolate a function defined on a discrete grid.
  /*!
   * Vector xpts contains a monotonic sequence of grid points, and 
   * vector fpts contains function values defined at these points.
   * The value returned is the linear interpolate at point x.
   * If x is outside the range of xpts, the value of fpts at the 
   * nearest end is returned.
   *
   * @param x value of the x coordinate
   * @param xpts value of the grid points
   * @param fpts value of the interpolant at the grid points
   *
   * @return Returned value is the value of of the interpolated
   *         function at x.
   */
  doublereal linearInterp(doublereal x, const vector_fp& xpts, 
			  const vector_fp& fpts);
}

#endif
