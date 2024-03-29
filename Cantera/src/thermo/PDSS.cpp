/**
 * @file PDSS.cpp
 * Implementation of a pressure dependent standard state 
 * virtual function
 * (see class \link Cantera::PDSS PDSS\endlink).
 */
/*
 * Copywrite (2006) Sandia Corporation. Under the terms of
 * Contract DE-AC04-94AL85000 with Sandia Corporation, the
 * U.S. Government retains certain rights in this software.
 */
/*
 * $Id: PDSS.cpp 279 2009-12-05 19:08:43Z hkmoffa $
 */

#include "ct_defs.h"
#include "xml.h"
#include "ctml.h"
#include "PDSS.h"

#include "ThermoFactory.h"
#include "SpeciesThermo.h"

#include "VPStandardStateTP.h"

namespace Cantera {
  /**
   * Basic list of constructors and duplicators
   */
  PDSS::PDSS() :
    m_pdssType(cPDSS_UNDEF),
    m_temp(-1.0),
    m_pres(-1.0),
    m_p0(-1.0),
    m_minTemp(-1.0),
    m_maxTemp(10000.0),
    m_tp(0),
    m_vpssmgr_ptr(0),
    m_mw(0.0),
    m_spindex(-1),
    m_spthermo(0),
    m_h0_RT_ptr(0),
    m_cp0_R_ptr(0),
    m_s0_R_ptr(0),
    m_g0_RT_ptr(0),
    m_V0_ptr(0),
    m_hss_RT_ptr(0),
    m_cpss_R_ptr(0),
    m_sss_R_ptr(0),
    m_gss_RT_ptr(0),
    m_Vss_ptr(0)
  {
  }

  PDSS::PDSS(VPStandardStateTP *tp, int spindex) :
    m_pdssType(cPDSS_UNDEF),
    m_temp(-1.0),
    m_pres(-1.0),
    m_p0(-1.0),
    m_minTemp(-1.0),
    m_maxTemp(10000.0),
    m_tp(tp),
    m_vpssmgr_ptr(0),
    m_mw(0.0),
    m_spindex(spindex),
    m_spthermo(0),
    m_h0_RT_ptr(0),
    m_cp0_R_ptr(0),
    m_s0_R_ptr(0),
    m_g0_RT_ptr(0),
    m_V0_ptr(0),
    m_hss_RT_ptr(0),
    m_cpss_R_ptr(0),
    m_sss_R_ptr(0),
    m_gss_RT_ptr(0),
    m_Vss_ptr(0)
  {
    if (tp) {
      m_spthermo = &(tp->speciesThermo());
    }
    if (tp) {
      m_vpssmgr_ptr = tp->provideVPSSMgr();
    }
  }


 

  PDSS::PDSS(const PDSS &b) :
    m_pdssType(cPDSS_UNDEF),
    m_temp(-1.0),
    m_pres(-1.0),
    m_p0(-1.0),
    m_minTemp(-1.0),
    m_maxTemp(10000.0),
    m_tp(0),
    m_vpssmgr_ptr(0),
    m_mw(b.m_mw),
    m_spindex(b.m_spindex),
    m_spthermo(b.m_spthermo),
    m_h0_RT_ptr(b.m_h0_RT_ptr),
    m_cp0_R_ptr(b.m_cp0_R_ptr),
    m_s0_R_ptr(b.m_s0_R_ptr),
    m_g0_RT_ptr(b.m_g0_RT_ptr),
    m_V0_ptr(b.m_V0_ptr),
    m_hss_RT_ptr(b.m_hss_RT_ptr),
    m_cpss_R_ptr(b.m_cpss_R_ptr),
    m_sss_R_ptr(b.m_sss_R_ptr),
    m_gss_RT_ptr(b.m_gss_RT_ptr),
    m_Vss_ptr(b.m_Vss_ptr)
  {
    /*
     * Use the assignment operator to do the brunt
     * of the work for the copy construtor.
     */
    *this = b;
  }

  /**
   * Assignment operator
   *        ok -> we don't know what to do here, so we'll
   *              first implement a shallow copy.
   */
  PDSS& PDSS::operator=(const PDSS&b) {
    if (&b == this) return *this;

    m_pdssType     = b.m_pdssType;
    m_temp         = b.m_temp;
    m_pres         = b.m_pres;
    m_p0           = b.m_p0;
    m_minTemp      = b.m_minTemp;
    m_maxTemp      = b.m_maxTemp;

    // Pointers which are zero, are properly assigned in the
    // function, initAllPtrs(). which must be called after the
    // assignment operation.

    m_tp           = 0;
    m_vpssmgr_ptr  = 0;
    m_mw           = b.m_mw;
    m_spindex      = b.m_spindex;
    m_spthermo     = 0;
    m_cp0_R_ptr    = 0;
    m_h0_RT_ptr    = 0;
    m_s0_R_ptr     = 0;
    m_g0_RT_ptr    = 0;
    m_V0_ptr       = 0;
    m_cpss_R_ptr   = 0;
    m_hss_RT_ptr   = 0;
    m_sss_R_ptr    = 0;
    m_gss_RT_ptr   = 0;
    m_Vss_ptr      = 0;

    // Here we just fill these in so that local copies within the VPSS object work.
    m_tp           = b.m_tp;
    m_vpssmgr_ptr  = b.m_vpssmgr_ptr;  
    m_spthermo     = b.m_spthermo;
    m_cp0_R_ptr    = b.m_cp0_R_ptr;
    m_h0_RT_ptr    = b.m_h0_RT_ptr;
    m_s0_R_ptr     = b.m_s0_R_ptr;
    m_g0_RT_ptr    = b.m_g0_RT_ptr;
    m_V0_ptr       = b.m_V0_ptr;
    m_cpss_R_ptr   = b.m_cpss_R_ptr;
    m_hss_RT_ptr   = b.m_hss_RT_ptr;
    m_sss_R_ptr    = b.m_sss_R_ptr;
    m_gss_RT_ptr   = b.m_gss_RT_ptr;
    m_Vss_ptr      = b.m_Vss_ptr;
  
    return *this;
  }

  PDSS::~PDSS() { 
  }
  
  // Duplicator from the %PDSS parent class
  /*
   * Given a pointer to a %PDSS object, this function will
   * duplicate the %PDSS object and all underlying structures.
   * This is basically a wrapper around the copy constructor.
   *
   * @return returns a pointer to a %PDSS
   */
  PDSS *PDSS::duplMyselfAsPDSS() const {
    PDSS *ip = new PDSS(*this);
    return ip;
  }

  // Returns the type of the standard state parameterization
  /*
   * @return Returns the integer # of the parameterization
   */
  PDSS_enumType PDSS::reportPDSSType() const { 
    return m_pdssType;
  }

  void PDSS::initThermoXML(const XML_Node& phaseNode, std::string& id) {
    AssertThrow(m_tp != 0, "PDSS::initThermoXML()");
    m_p0 =  m_vpssmgr_ptr->refPressure(m_spindex);
    m_minTemp = m_vpssmgr_ptr->minTemp(m_spindex);
    m_maxTemp = m_vpssmgr_ptr->maxTemp(m_spindex); 
  }

  void PDSS::initThermo() {
    AssertThrow(m_tp != 0, "PDSS::initThermo()");
    m_vpssmgr_ptr = m_tp->provideVPSSMgr();
    initPtrs();
    m_mw = m_tp->molecularWeight(m_spindex);
  }
  
  void PDSS::initAllPtrs(VPStandardStateTP *tp, VPSSMgr *vpssmgr_ptr, 
			 SpeciesThermo* spthermo) {
    m_tp = tp;
    m_vpssmgr_ptr = vpssmgr_ptr;
    m_spthermo = spthermo;
    initPtrs();
  } 

  void PDSS::initPtrs() {
    m_h0_RT_ptr  = &(m_vpssmgr_ptr->mPDSS_h0_RT[0]);
    m_cp0_R_ptr  = &(m_vpssmgr_ptr->mPDSS_cp0_R[0]);
    m_s0_R_ptr   = &(m_vpssmgr_ptr->mPDSS_s0_R[0]);
    m_g0_RT_ptr  = &(m_vpssmgr_ptr->mPDSS_g0_RT[0]);
    m_V0_ptr     = &(m_vpssmgr_ptr->mPDSS_V0[0]);

    m_hss_RT_ptr  = &(m_vpssmgr_ptr->mPDSS_hss_RT[0]);
    m_cpss_R_ptr  = &(m_vpssmgr_ptr->mPDSS_cpss_R[0]);
    m_sss_R_ptr   = &(m_vpssmgr_ptr->mPDSS_sss_R[0]);
    m_gss_RT_ptr  = &(m_vpssmgr_ptr->mPDSS_gss_RT[0]);
    m_Vss_ptr     = &(m_vpssmgr_ptr->mPDSS_Vss[0]);
  }



  // Return the molar enthalpy in units of J kmol-1
  /*
   * Returns the species standard state enthalpy in J kmol-1 at the
   * current temperature and pressure.
   * (NOTE: assumes that ThermoPhase Ref Polynomials are up-to-date)
   */
  doublereal PDSS::enthalpy_mole() const {
    err("enthalpy_mole()");
    return (0.0);
  }
  
  doublereal PDSS::enthalpy_RT() const {
    double RT = GasConstant * m_temp;
    return (enthalpy_mole()/RT);
  }

  // Return the molar internal Energy in units of J kmol-1
  /*
   * Returns the species standard state internal Energy in J kmol-1 at the
   * current temperature and pressure.
   *
   * @return returns the species standard state internal Energy in  J kmol-1
   */
  doublereal PDSS::intEnergy_mole() const {
    err("intEnergy_mole()");
    return (0.0);
  }

  // Return the molar entropy in units of J kmol-1 K-1
  /*
   * Returns the species standard state entropy in J kmol-1 K-1 at the
   * current temperature and pressure.
   *
   * @return returns the species standard state entropy in J kmol-1 K-1
   */
  doublereal PDSS::entropy_mole() const {
    err("entropy_mole()");
    return (0.0);
  }

  doublereal PDSS::entropy_R() const {
    return(entropy_mole()/GasConstant);
  }

  // Return the molar gibbs free energy in units of J kmol-1
  /*
   * Returns the species standard state gibbs free energy in J kmol-1 at the
   * current temperature and pressure.
   *
   * @return returns the species standard state gibbs free energy in  J kmol-1
   */
  doublereal PDSS::gibbs_mole() const {
    err("gibbs_mole()");
    return (0.0);
  }

  doublereal PDSS::gibbs_RT() const {
    double RT = GasConstant * m_temp;
    return (gibbs_mole()/RT);
  }

  // Return the molar const pressure heat capacity in units of J kmol-1 K-1
  /*
   * Returns the species standard state Cp in J kmol-1 K-1 at the
   * current temperature and pressure.
   *
   * @return returns the species standard state Cp in J kmol-1 K-1
   */
  doublereal PDSS::cp_mole() const {
    err("cp_mole()");
    return (0.0);
  }

  doublereal PDSS::cp_R() const {
    return (cp_mole()/GasConstant);
  }

  doublereal PDSS::molarVolume() const {
    err("molarVolume()");
    return 0.0;
  }

  doublereal PDSS::density() const {
    err("density()");
    return 0.0;
  }

  // Return the molar const volume heat capacity in units of J kmol-1 K-1
  /*
   * Returns the species standard state Cv in J kmol-1 K-1 at the
   * current temperature and pressure.
   *
   * @return returns the species standard state Cv in J kmol-1 K-1
   */
  doublereal PDSS::cv_mole() const {
    err("cv_mole()");
    return (0.0);
  }
   
  doublereal PDSS::gibbs_RT_ref() const {
    err("gibbs_RT_ref()");
    return 0.0;
  }

  doublereal PDSS::enthalpy_RT_ref() const {
    err("enthalpy_RT_ref()");
    return 0.0;
  }

  doublereal PDSS::entropy_R_ref() const {
    err("entropy_RT_ref()");
    return 0.0;
  }

  doublereal PDSS::cp_R_ref() const {
    err("entropy_RT_ref()");
    return 0.0;
  }

  doublereal PDSS::molarVolume_ref() const {
    err("molarVolume_ref()");
    return 0.0;
  }

  /**
   * Return the difference in enthalpy between current p
   * and ref p0, in mks units of
   * in units of J kmol-1
   */
  doublereal PDSS::
  enthalpyDelp_mole() const {
    doublereal RT = m_temp * GasConstant;
    doublereal tmp = enthalpy_RT_ref();
    return(enthalpy_mole() - RT * tmp);
  }


  /**
   *  Return the difference in entropy between current p
   * and ref p0, in mks units of
   * J kmol-1 K-1
   */
  doublereal PDSS::entropyDelp_mole() const {
    doublereal tmp = entropy_R_ref();
    return(entropy_mole() - GasConstant * tmp);
 
  }

  /**
   * Calculate the difference in Gibbs free energy between current p and
   * the ref p0, in mks units of
   * J kmol-1 K-1.
   */
  doublereal PDSS::gibbsDelp_mole() const {
    doublereal RT = m_temp * GasConstant;
    doublereal tmp = gibbs_RT_ref();
    return(gibbs_mole() - RT * tmp);
  }

  // Return the molar const volume heat capacity in units of J kmol-1 K-1
  /*
   * Returns the species standard state Cv in J kmol-1 K-1 at the
   * current temperature and pressure.
   *
   * @return returns the species standard state Cv in J kmol-1 K-1
   */
  doublereal PDSS::cpDelp_mole() const {
    doublereal tmp = cp_R_ref();
    return(cp_mole() - GasConstant * tmp);
  }
  
  /**
   * Calculate the pressure (Pascals), given the temperature and density
   *  Temperature: kelvin
   *  rho: density in kg m-3
   */
  doublereal PDSS::pressure() const {
    return (m_pres);
  }
   
  // Return the volumetric thermal expansion coefficient. Units: 1/K.
  /*
   * The thermal expansion coefficient is defined as
   * \f[
   * \beta = \frac{1}{v}\left(\frac{\partial v}{\partial T}\right)_P
   * \f]
   */
  doublereal PDSS::thermalExpansionCoeff() const {
    throw CanteraError("PDSS::thermalExpansionCoeff()", "unimplemented");
    return (0.0);
  }
  
  /// critical temperature 
  doublereal PDSS::critTemperature() const { 
    err("critTemperature()");
    return (0.0);
  }
        
  /// critical pressure
  doublereal PDSS::critPressure() const {
    err("critPressure()");
    return (0.0);
  }
        
  /// critical density
  doublereal PDSS::critDensity() const {
    err("critDensity()");
    return (0.0);
  }

  void PDSS::setPressure(doublereal pres) {
    m_pres = pres;
  }


  /**
   * Return the temperature 
   *
   * Obtain the temperature from the owning ThermoPhase object
   * if you can. 
   */
  doublereal PDSS::temperature() const {
    return m_temp;
  }
 
  void PDSS::setTemperature(doublereal temp) {
    m_temp = temp;
  }

  doublereal PDSS::molecularWeight() const {
    return m_mw;
  }
  void PDSS::setMolecularWeight(doublereal mw) {
    m_mw = mw;
  }

  void PDSS::setState_TP(doublereal temp, doublereal pres) {
    err("setState_TP()");
  }

  void PDSS::setState_TR(doublereal temp, doublereal rho) {
    err("setState_TR()");
  }

  /// saturation pressure
  doublereal PDSS::satPressure(doublereal t){
    err("satPressure()");
    return (0.0);
  }
    

  void PDSS::err(std::string msg) const {
    throw CanteraError("PDSS::" + msg, "unimplemented");
  }


  void PDSS::reportParams(int &kindex, int &type,
			  doublereal * const c,
			  doublereal &minTemp,
			  doublereal &maxTemp,
			  doublereal &refPressure) const {
    kindex = m_spindex;
    type = m_pdssType;
    minTemp = m_minTemp;
    maxTemp = m_maxTemp;
    refPressure = m_p0;
  }
}
