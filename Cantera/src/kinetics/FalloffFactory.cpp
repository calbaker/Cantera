/**
 *  @file FalloffFactory.cpp
 */

/*  $Author: hkmoffa $
 *  $Date: 2009-12-10 17:20:44 -0600 (Thu, 10 Dec 2009) $
 *  $Revision: 309 $
 */

// Copyright 2001  California Institute of Technology

#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif


#include "FalloffFactory.h"
#include "ctexceptions.h"

#include <cmath>

namespace Cantera {

  FalloffFactory* FalloffFactory::s_factory = 0;
#if defined(THREAD_SAFE_CANTERA)
  boost::mutex FalloffFactory::falloff_mutex ;
#endif

    
  //! The 3-parameter Troe falloff parameterization. 
  /*!
   * The falloff function defines the value of \f$ F \f$ in the following
   * rate expression
   *
   *  \f[
   *         k = k_{\infty} \left( \frac{P_r}{1 + P_r} \right) F   
   *  \f]
   *  where
   *  \f[
   *         P_r = \frac{k_0 [M]}{k_{\infty}}
   *  \f]
   *
   * This parameterization is  defined by
   * \f[
   *        F = F_{cent}^{1/(1 + f_1^2)} 
   * \f]
   * where
   * \f[
   *       F_{cent} = (1 - A)\exp(-T/T_3) + A \exp(-T/T_1)
   * \f]
   *
   * \f[ 
   *       f_1 = (\log_{10} P_r + C) / \left(N - 0.14 
   *             (\log_{10} P_r + C)\right) 
   * \f]
   * 
   * \f[
   *          C = -0.4 - 0.67 \log_{10} F_{cent} 
   * \f]
   * 
   * \f[
   *          N = 0.75 - 1.27 \log_{10} F_{cent}
   * \f]
   *
   *  There are a few requirements for the parameters
   *
   *   T_3 is required to greater than or equal to zero. If it is zero,
   *   then the term is set to zero. 
   *
   *   T_1 is required to greater than or equal to zero. If it is zero,
   *   then the term is set to zero.
   *
   * @ingroup falloffGroup 
   */
  class Troe3 : public Falloff {
  public:
        
    //! Default constructor.
    Troe3() : m_a (0.0), m_rt3 (0.0), m_rt1 (0.0) {}

    //! Destructor. Does nothing.
    virtual ~Troe3() {}

    /**
     * Initialize.
     * @param c Coefficient vector of length 3, 
     * with entries \f$ (A, T_3, T_1) \f$
     */
    virtual void init(const vector_fp& c) { 
      m_a  = c[0];

      if (c[1] <= 0.0) {
	if (c[1] == 0.0) {
	  m_rt3 = 1000.;
	} else {
	  throw CanteraError("Troe3::init()", "T3 parameter is less than zero");
	}
      } else {
	m_rt3 = 1.0/c[1];
      }
      if (c[2] <= 0.0) {
	if (c[2] == 0.0) {
	  m_rt1 = 1000.;
	} else {
	  throw CanteraError("Troe3::init()", "T1 parameter is less than zero");
	}
      } else {
	m_rt1 = 1.0/c[2];
      }
    }

    //! Update the temperature parameters in the representation
    /*!
     *   The workspace has a length of one
     *
     *   @param T         Temperature (Kelvin)
     *   @param work      Vector of working space representing
     *                    the temperature dependent part of the
     *                    parameterization.
     */
    virtual void updateTemp(doublereal T, workPtr work) const {
      doublereal Fcent = (1.0 - m_a) * exp(- T * m_rt3 ) 
	+ m_a * exp(- T * m_rt1 );
      *work = log10( fmaxx( Fcent, SmallNumber ) );
    }

    //! Function that returns <I>F</I>
    /*!
     *  @param pr   Value of the reduced pressure for this reaction
     *  @param work Pointer to the previously saved work space
     */
    virtual doublereal F(doublereal pr, const_workPtr work) const {
      doublereal lpr,f1,lgf, cc, nn;
      lpr = log10( fmaxx(pr,SmallNumber) );
      cc = -0.4 - 0.67 * (*work);
      nn = 0.75 - 1.27 * (*work);             
      f1 = ( lpr + cc )/ ( nn - 0.14 * ( lpr + cc ) );
      lgf = (*work) / ( 1.0 + f1 * f1 );
      return pow(10.0, lgf );
    }

    //! Utility function that returns the size of the workspace
    virtual size_t workSize() { return 1; }

  protected:

    //! parameter a in the  4-parameter Troe falloff function
    /*!
     * This is unitless
     */
    doublereal m_a;

    //! parameter 1/T_3 in the  4-parameter Troe falloff function
    /*!
     * This has units of Kelvin-1
     */
    doublereal m_rt3;

    //! parameter 1/T_1 in the  4-parameter Troe falloff function
    /*!
     * This has units of Kelvin-1
     */
    doublereal m_rt1;

  };

  //! The 4-parameter Troe falloff parameterization.
  /*!
   * The falloff function defines the value of \f$ F \f$ in the following
   * rate expression
   *
   *  \f[
   *         k = k_{\infty} \left( \frac{P_r}{1 + P_r} \right) F   
   *  \f]
   *  where
   *  \f[
   *         P_r = \frac{k_0 [M]}{k_{\infty}}
   *  \f]
   *
   * This parameterization is defined by
   *
   * \f[
   *         F = F_{cent}^{1/(1 + f_1^2)}
   * \f]
   *    where
   * \f[
   *         F_{cent} = (1 - A)\exp(-T/T_3) + A \exp(-T/T_1) + \exp(-T_2/T)
   * \f]
   *
   * \f[
   *        f_1 = (\log_{10} P_r + C) / 
   *              \left(N - 0.14 (\log_{10} P_r + C)\right)
   * \f]
   *
   * \f[
   *         C = -0.4 - 0.67 \log_{10} F_{cent} 
   * \f]
   * 
   * \f[
   *          N = 0.75 - 1.27 \log_{10} F_{cent} 
   * \f]
   *
   *
   *  There are a few requirements for the parameters
   *
   *   T_3 is required to greater than or equal to zero. If it is zero,
   *   then the term is set to zero. 
   *
   *   T_1 is required to greater than or equal to zero. If it is zero,
   *   then the term is set to zero.
   *
   *   T_2 is required to be greater than  zero. 
   *
   * @ingroup falloffGroup 
   */
  class Troe4 : public Falloff {
  public:
    //! Constructor
    Troe4() : m_a (0.0), m_rt3 (0.0), m_rt1 (0.0),
	      m_t2 (0.0) {}

    //! Destructor
    virtual ~Troe4() {}


    //! Initialization of the object
    /*!
     * @param c Vector of four doubles: The doubles are the parameters,
     *          a,, T_3, T_1, and T_2 of the SRI parameterization
     */
    virtual void init(const vector_fp& c) {
      m_a  = c[0];
      if (c[1] <= 0.0) {
	if (c[1] == 0.0) {
	  m_rt3 = 1000.;
	} else {
	  throw CanteraError("Troe4::init()", "T3 parameter is less than zero");
	}
      } else {
	m_rt3 = 1.0/c[1];
      }
      if (c[2] <= 0.0) {
	if (c[2] == 0.0) {
	  m_rt1 = 1000.;
	} else {
	  throw CanteraError("Troe4::init()", "T1 parameter is less than zero");
	}
      } else {
	m_rt1 = 1.0/c[2];
      }
      if (c[3] < 0.0) {
	throw CanteraError("Troe4::init()", "T2 parameter is less than zero");
      }
      m_t2 = c[3];
    }

    //! Update the temperature parameters in the representation
    /*!
     *   The workspace has a length of one
     *
     *   @param T         Temperature (Kelvin)
     *   @param work      Vector of working space representing
     *                    the temperature dependent part of the
     *                    parameterization.
     */
    virtual void updateTemp(doublereal T, workPtr work) const {
      doublereal Fcent = (1.0 - m_a) * exp(- T * m_rt3 ) 
	+ m_a * exp(- T * m_rt1 )
	+ exp(- m_t2 / T );
      *work = log10( fmaxx( Fcent, SmallNumber ) );
    }

    //! Function that returns <I>F</I>
    /*!
     *  @param pr   Value of the reduced pressure for this reaction
     *  @param work Pointer to the previously saved work space
     */
    virtual doublereal F(doublereal pr, const_workPtr work) const {
      doublereal lpr,f1,lgf, cc, nn;
      lpr = log10( fmaxx(pr,SmallNumber) );
      cc = -0.4 - 0.67 * (*work);
      nn = 0.75 - 1.27 * (*work);             
      f1 = ( lpr + cc )/ ( nn - 0.14 * ( lpr + cc ) );
      lgf = (*work) / ( 1.0 + f1 * f1 );
      return pow(10.0, lgf );
    }

    //! Utility function that returns the size of the workspace
    virtual size_t workSize() { return 1; }

  protected:

    //! parameter a in the  4-parameter Troe falloff function
    /*!
     * This is unitless
     */
    doublereal m_a;

    //! parameter 1/T_3 in the  4-parameter Troe falloff function
    /*!
     * This has units of Kelvin-1
     */
    doublereal m_rt3;

    //! parameter 1/T_1 in the  4-parameter Troe falloff function
    /*!
     * This has units of Kelvin-1
     */
    doublereal m_rt1;

    //! parameter T_2 in the  4-parameter Troe falloff function
    /*!
     * This has units of Kelvin
     */
    doublereal m_t2;
  };

  
  //! The 3-parameter SRI falloff function for <I>F</I>
  /*!
   * The falloff function defines the value of \f$ F \f$ in the following
   * rate expression
   *
   *  \f[
   *         k = k_{\infty} \left( \frac{P_r}{1 + P_r} \right) F   
   *  \f]
   *  where
   *  \f[
   *         P_r = \frac{k_0 [M]}{k_{\infty}}
   *  \f]
   *
   *  \f[
   *         F = {\left( a \; exp(\frac{-b}{T}) + exp(\frac{-T}{c})\right)}^n
   *  \f]
   *      where 
   *  \f[
   *         n = \frac{1.0}{1.0 + {\log_{10} P_r}^2}
   *  \f]
   *
   *   \f$ c \f$ s required to greater than or equal to zero. If it is zero,
   *   then the corresponding term is set to zero.
   *
   * @ingroup falloffGroup 
   */
  class SRI3 : public Falloff {
      
  public:

    //! Constructor
    SRI3() {}

    //! Destructor
    virtual ~SRI3() {}
        
    //! Initialization of the object
    /*!
     * @param c Vector of three doubles: The doubles are the parameters,
     *          a, b, and c of the SRI parameterization
     */
    virtual void init(const vector_fp& c) {
      m_a = c[0];
      m_b = c[1];
      m_c = c[2];
    }
  
    //! Update the temperature parameters in the representation
    /*!
     *   The workspace has a length of one
     *
     *   @param T         Temperature (Kelvin)
     *   @param work      Vector of working space representing
     *                    the temperature dependent part of the
     *                    parameterization.
     */
    virtual void updateTemp(doublereal T, workPtr work) const {
      *work = m_a * exp( - m_b / T);
      if (m_c != 0.0) *work += exp( - T/m_c );
    }

    //! Function that returns <I>F</I>
    /*!
     *  @param pr   Value of the reduced pressure for this reaction
     *  @param work Pointer to the previously saved work space
     */
    virtual doublereal F(doublereal pr, const_workPtr work) const {
      doublereal lpr = log10( fmaxx(pr,SmallNumber) );
      doublereal xx = 1.0/(1.0 + lpr*lpr);
      doublereal ff = pow( *work , xx);
      return ff;
    }

    //! Utility function that returns the size of the workspace
    virtual size_t workSize() { return 1; }

  protected:

    //! parameter a in the  3-parameter SRI falloff function
    /*!
     * This is unitless
     */
    doublereal m_a;

    //! parameter b in the  3-parameter SRI falloff function
    /*!
     * This has units of Kelvin
     */
    doublereal m_b;

    //! parameter c in the  3-parameter SRI falloff function
    /*!
     * This has units of Kelvin
     */
    doublereal m_c;
  };


  //! The 5-parameter SRI falloff function.
  /*!
   * The falloff function defines the value of \f$ F \f$ in the following
   * rate expression
   *
   *  \f[
   *         k = k_{\infty} \left( \frac{P_r}{1 + P_r} \right) F   
   *  \f]
   *  where
   *  \f[
   *         P_r = \frac{k_0 [M]}{k_{\infty}}
   *  \f]
   *
   *  \f[
   *         F = {\left( a \; exp(\frac{-b}{T}) + exp(\frac{-T}{c})\right)}^n
   *              \;  d \; exp(\frac{-e}{T})
   *  \f]
   *      where 
   *  \f[
   *         n = \frac{1.0}{1.0 + {\log_{10} P_r}^2}
   *  \f]
   *
   *   \f$ c \f$ s required to greater than or equal to zero. If it is zero,
   *   then the corresponding term is set to zero.
   *
   *   m_c is required to greater than or equal to zero. If it is zero,
   *   then the corresponding term is set to zero.
   *
   *   m_d is required to be greater than zero.
   *
   * @ingroup falloffGroup 
   */
  class SRI5 : public Falloff {
      
  public:

    //! Constructor
    SRI5() {}

    //! Destructor
    virtual ~SRI5() {}

    //! Initialization of the object
    /*!
     * @param c Vector of five doubles: The doubles are the parameters,
     *          a, b, c, d, and e of the SRI parameterization
     */
    virtual void init(const vector_fp& c) {
      m_a = c[0];
      m_b = c[1];
      m_c = c[2];
      m_d = c[3];
      m_e = c[4];
    }

    //! Update the temperature parameters in the representation
    /*!
     *   The workspace has a length of two
     *
     *   @param T         Temperature (Kelvin)
     *   @param work      Vector of working space representing
     *                    the temperature dependent part of the
     *                    parameterization.
     */
    virtual void updateTemp(doublereal T, workPtr work) const {
      *work = m_a * exp( - m_b / T);
      if (m_c != 0.0) *work += exp( - T/m_c );
      work[1] = m_d * pow(T,m_e);
    }

    //! Function that returns <I>F</I>
    /*!
     *  @param pr   Value of the reduced pressure for this reaction
     *  @param work Pointer to the previously saved work space
     */
    virtual doublereal F(doublereal pr, const_workPtr work) const {
      doublereal lpr = log10( fmaxx(pr,SmallNumber) );
      doublereal xx = 1.0/(1.0 + lpr*lpr);
      return pow( *work, xx) * work[1]; 
    }

    //! Utility function that returns the size of the workspace
    virtual size_t workSize() { return 2; }

  protected:

    //! parameter a in the  5-parameter SRI falloff function
    /*!
     * This is unitless
     */
    doublereal m_a;

    //! parameter b in the  5-parameter SRI falloff function
    /*!
     * This has units of Kelvin
     */
    doublereal m_b;

    //! parameter c in the  5-parameter SRI falloff function
    /*!
     * This has units of Kelvin
     */
    doublereal m_c;
    
    //! parameter d in the  5-parameter SRI falloff function
    /*!
     * This is unitless
     */
    doublereal m_d;

    //! parameter d in the  5-parameter SRI falloff function
    /*!
     * This is unitless
     */
    doublereal m_e;

  };


  //! Wang-Frenklach falloff function. 
  /*!
   *
   * The falloff function defines the value of \f$ F \f$ in the following
   * rate expression
   *
   *  \f[
   *         k = k_{\infty} \left( \frac{P_r}{1 + P_r} \right) F   
   *  \f]
   *  where
   *  \f[
   *         P_r = \frac{k_0 [M]}{k_{\infty}}
   *  \f]
   *
   *  \f[
   *         F = 10.0^{Flog}
   *  \f]
   *  where
   *  \f[
   *         Flog = \frac{\log_{10} F_{cent}}{\exp{(\frac{\log_{10} P_r - \alpha}{\sigma})^2}}
   *  \f]
   *    where
   * 
   *  \f[
   *       F_{cent} = (1 - A)\exp(-T/T_3) + A \exp(-T/T_1) + \exp(-T/T_2)
   *  \f]
   *
   *  \f[
   *      \alpha = \alpha_0 + \alpha_1 T + \alpha_2 T^2
   *  \f]
   *
   *  \f[
   *      \sigma = \sigma_0 + \sigma_1 T + \sigma_2 T^2
   *  \f]
   *  
   *
   * Reference: Wang, H., and
   *            Frenklach, M., Chem. Phys. Lett. vol. 205, 271 (1993).
   *
   *
   * @ingroup falloffGroup 
   */
  class WF93 : public Falloff {
      
  public:

    //! Default constructpr
    WF93() {}

    //! Destructor
    virtual ~WF93() {}

    //! Initialization routine
    /*!
     *  @param c  Vector of 10 doubles
     *            with the following ordering:
     *               a, T_1, T_2, T_3, alpha0, alpha1, alpha2
     *               sigma0, sigma1, sigma2
     */
    virtual void init(const vector_fp& c) {
      m_a = c[0];
      m_rt1 = 1.0/c[1];
      m_t2 = c[2]; 
      m_rt3  = 1.0/c[3];
      m_alpha0 = c[4];
      m_alpha1 = c[5];
      m_alpha2 = c[6];
      m_sigma0 = c[7];
      m_sigma1 = c[8];
      m_sigma2 = c[9];
    }
    
    //! Update the temperature parameters in the representation
    /*!
     *   The workspace has a length of three
     *
     *   @param T         Temperature (Kelvin)
     *   @param work      Vector of working space representing
     *                    the temperature dependent part of the
     *                    parameterization.
     */
    virtual void updateTemp(doublereal T, workPtr work) const {
      work[0] = m_alpha0 + (m_alpha1 + m_alpha2*T)*T; // alpha
      work[1] = m_sigma0 + (m_sigma1 + m_sigma2*T)*T; // sigma
      doublereal Fcent = (1.0 - m_a) * exp(- T * m_rt3 ) 
	+ m_a * exp(- T * m_rt1 ) + exp(-m_t2/T);
      work[2] = log10(Fcent); 
    }

    //! Function that returns <I>F</I>
    /*!
     *  @param pr   Value of the reduced pressure for this reaction
     *  @param work Pointer to the previously saved work space
     */
    virtual doublereal F(doublereal pr, const_workPtr work) const {
      doublereal lpr = log10( fmaxx(pr, SmallNumber) );
      doublereal x = (lpr - work[0])/work[1];
      doublereal flog = work[2]/exp(x*x);
      return pow( 10.0, flog);
    }

    //! Utility function that returns the size of the workspace
    virtual size_t workSize() { return 3; }

  protected:

    //! Value of the \f$ \alpha_0 \f$ coefficient
    /*!
     *  This is the fifth coefficient in the xml list
     */
    doublereal m_alpha0;

    //! Value of the \f$ \alpha_1 \f$ coefficient
    /*!
     *  This is the 6th coefficient in the xml list
     */
    doublereal m_alpha1;

    //! Value of the \f$ \alpha_2 \f$ coefficient
    /*!
     *  This is the 7th coefficient in the xml list
     */
    doublereal m_alpha2;

    //! Value of the \f$ \sigma_0 \f$ coefficient
    /*!
     *  This is the 8th coefficient in the xml list
     */
    doublereal m_sigma0;

    //! Value of the \f$ \sigma_1 \f$ coefficient
    /*!
     *  This is the 9th coefficient in the xml list
     */
    doublereal m_sigma1;

    //! Value of the \f$ \sigma_2 \f$ coefficient
    /*!
     *  This is the 10th coefficient in the xml list
     */
    doublereal m_sigma2;

    //! Value of the \f$ a \f$ coefficient
    /*!
     *  This is the first coefficient in the xml list
     */
    doublereal m_a;

    //! Value of inverse of the \f$ t1 \f$ coefficient
    /*!
     *  This is the second coefficient in the xml list
     */
    doublereal m_rt1;

    //! Value of the \f$ t2 \f$ coefficient
    /*!
     *  This is the third coefficient in the xml list
     */
    doublereal m_t2;

    //! Value of the inverse of the  \f$ t3 \f$ coefficient
    /*!
     *  This is the 4th coefficient in the xml list
     */
    doublereal m_rt3;

  private:

  };

  // Factory routine that returns a new Falloff parameterization object
  /*
   *  @param type  Integer type of the falloff parameterization. These
   *               integers are listed in reaction_defs.h
   *
   *  @param c     Vector of input parameterizations for the Falloff 
   *               object. The function is initialized with this vector.
   *
   *  @return  Returns a pointer to a newly malloced Falloff object
   */
  Falloff* FalloffFactory::newFalloff(int type, const vector_fp& c) {
    Falloff* f;
    switch(type) {
    case TROE3_FALLOFF:
      f = new Troe3(); break;
    case TROE4_FALLOFF: 
      f = new Troe4(); break;
    case SRI3_FALLOFF: 
      f = new SRI3(); break;
    case SRI5_FALLOFF: 
      f = new SRI5(); break;
    case WF_FALLOFF:
      f = new WF93(); break; 
    default: return 0;
    }   
    f->init(c); 
    return f;
  }

}

