/***********************************************************************
 *  $RCSfile: equil.h,v $
 *  $Author: hkmoffa $
 *  $Date: 2010-01-03 18:46:26 -0600 (Sun, 03 Jan 2010) $
 *  $Revision: 368 $
 ***********************************************************************/
//  Copyright 2001  California Institute of Technology

/**
 *  @file equil.h
 *   This file contains the definition of some high level general equilibration
 *   routines and the text for the module \ref equilfunctions. 
 * 
 *   It also contains the Module doxygen text for the Equilibration Solver
 *   capability within %Cantera. see \ref equilfunctions 
 */
#ifndef CT_KERNEL_EQUIL_H
#define CT_KERNEL_EQUIL_H

//#include "ChemEquil.h"
#include "MultiPhase.h"

namespace Cantera {

  /*!
   * @defgroup equilfunctions Equilibrium Solver Capability
   *
   * Cantera has several different equilibrium routines.
   */
  //@{
  //@}


  //-----------------------------------------------------------
  //              convenience functions
  //-----------------------------------------------------------
  
  //! Equilibrate a ThermoPhase object
  /*!
   *  Set a single-phase chemical solution to chemical equilibrium.
   *  This is a convenience function that uses one or the other of
   *  the two chemical equilibrium solvers. The XY parameter indicates what two
   *  thermodynamic quantities, other than element composition, are to be held
   *  constant during the equilibration process.
   *
   *  @param s         ThermoPhase object that will be equilibrated.
   *  @param XY        String representation of what two properties 
   *                   are being held constant
   *  @param solver    ID of the solver to be used to equlibrate the phase.
   *                   If solver = 0, the ChemEquil solver will be used,
   *                   and if solver = 1, the
   *                   MultiPhaseEquil solver will be used (slower than ChemEquil,
   *                   but more stable). If solver < 0 (default, then ChemEquil will
   *                   be tried first, and if it fails MultiPhaseEquil will be tried.
   *  @param rtol      Relative tolerance
   *  @param maxsteps  Maximum number of steps to take to find the solution
   *  @param maxiter   For the MultiPhaseEquil solver only, this is
   *                   the maximum number of outer temperature or pressure iterations
   *                   to take when T and/or P is not held fixed.
   *  @param loglevel  loglevel Controls amount of diagnostic output. loglevel
   *                   = 0 suppresses diagnostics, and increasingly-verbose messages
   *                   are written as loglevel increases. The messages are written to
   *                   a file in HTML format for viewing in a web browser.
   *                   @see HTML_logs
   *
   * @return
   * Return variable is equal to the number of subroutine attempts
   * it took to equilibrate the system.
   *
   * 
   *      @ingroup equilfunctions
   *      @ingroup equil
   */
  int equilibrate(thermo_t& s, const char* XY,
		  int solver = -1, doublereal rtol = 1.0e-9, int maxsteps = 5000, 
		  int maxiter = 100, int loglevel = -99);

  //! Equilibrate a MultiPhase object
  /*!
   *  Equilibrate a MultiPhase object. The XY parameter indicates what two
   *  thermodynamic quantities, other than element composition, are to be held
   *  constant during the equilibration process.
   *
   * This is the top-level driver for multiphase equilibrium. It
   * doesn't do much more than call the equilibrate method of class
   * MultiPhase, except that it adds some messages to the logfile,
   * if loglevel is set > 0.
   *
   *  @param s       MultiPhase object that will be equilibrated.
   *  @param XY      String representation of what is being held constant
   *  @param rtol    Relative tolerance
   *  @param maxsteps  Maximum number of steps
   *  @param maxiter  Maximum iterations
   *  @param loglevel loglevel
   *
   * @return
   * Return variable is equal to the number of subroutine attempts
   * it took to equilibrate the system.
   * 
   *      @ingroup equilfunctions
   *      @ingroup equil
   */
  doublereal equilibrate(MultiPhase& s, const char* XY,
			 doublereal rtol = 1.0e-9, int maxsteps = 5000, int maxiter = 100,
			 int loglevel = -99);
  
}

#endif
