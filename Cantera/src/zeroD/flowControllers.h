/**
 *  @file flowControllers.h
 *
 *  Some flow devices derived from class FlowDevice.
 *
 * $Author: hkmoffa $
 * $Revision: 368 $
 * $Date: 2010-01-03 18:46:26 -0600 (Sun, 03 Jan 2010) $
 */

// Copyright 2001  California Institute of Technology


#ifndef CT_FLOWCONTR_H
#define CT_FLOWCONTR_H

#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif

#include "FlowDevice.h"
#include "ReactorBase.h"
//#include "PID_Controller.h"
#include "Func1.h"

namespace CanteraZeroD {


    /**
     * A class for mass flow controllers. The mass flow rate is constant,
     * independent of any other parameters.
     */
    class MassFlowController : public FlowDevice {
    public:

        MassFlowController() : FlowDevice() {
            m_type = MFC_Type;
        }

        virtual ~MassFlowController() {}

        virtual bool ready() {
            return FlowDevice::ready() && m_mdot >= 0.0;
        }

        /// If a function of time has been specified for
        /// mdot, then update the stored mass flow rate.
        /// Otherwise, mdot is a constant, and does not need
        /// updating.
        virtual void updateMassFlowRate(doublereal time) {
            if (m_func) m_mdot = m_func->eval(time);
            if (m_mdot < 0.0) m_mdot = 0.0;
        }

    protected:

    private:
    };

    /**
     * A class for mass flow controllers. The mass flow rate is constant,
     * independent of any other parameters.
     */
    class PressureController : public FlowDevice {
    public:

        PressureController() : FlowDevice(), m_master(0) {
            m_type = PressureController_Type;
        }

        virtual ~PressureController() {}

        virtual bool ready() {
            return FlowDevice::ready() && m_master != 0;
        }

        void setMaster(FlowDevice* master) {
            m_master = master;
        }

        virtual void updateMassFlowRate(doublereal time) {
            doublereal master_mdot = m_master->massFlowRate(time);
            m_mdot = master_mdot + m_coeffs[0]*(in().pressure() - 
                out().pressure());
            if (m_mdot < 0.0) m_mdot = 0.0;
        }

    protected:
        FlowDevice* m_master;

    private:
    };


    /// Valve objects supply a mass flow rate that is a function of the
    /// pressure drop across the valve. The default behavior is a linearly
    /// proportional to the pressure difference. Note that
    /// real valves do not have this behavior, so this class
    /// does not model real, physical valves.
    class Valve : public FlowDevice {
    public:

        Valve() : FlowDevice() {
            m_type = Valve_Type;
        }

        virtual ~Valve() {}

        virtual bool ready() {
            return FlowDevice::ready() && m_coeffs.size() >= 1;
        }

        /// Compute the currrent mass flow rate, based on
        /// the pressure difference.
        virtual void updateMassFlowRate(doublereal time) {
            double delta_P = in().pressure() - out().pressure();
            if (m_func) {
                m_mdot = m_func->eval(delta_P);
            }
            else {
                m_mdot = m_coeffs[0]*delta_P;
            }
            if (m_mdot < 0.0) m_mdot = 0.0;
        }

    protected:

    private:
    };

}
#endif

