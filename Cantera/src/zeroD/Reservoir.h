/**
 * @file Reservoir.h
 */

/*
 * $Author: hkmoffa $
 * $Revision: 368 $
 * $Date: 2010-01-03 18:46:26 -0600 (Sun, 03 Jan 2010) $
 */

// Copyright 2001  California Institute of Technology

#ifndef CT_RESERVOIR_H
#define CT_RESERVOIR_H

#ifdef WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4503)
#endif

#include <iostream>
#include "ReactorBase.h"

namespace CanteraZeroD {

    class Reservoir : public ReactorBase {

    public:    

        Reservoir() {}
        virtual ~Reservoir() {}
        virtual int type() const { return ReservoirType; }
        virtual void initialize(doublereal t0 = 0.0) {}
        virtual void advance(doublereal time) {m_time = time;}

        void insert(ThermoPhase& contents) {
            setThermoMgr(contents);
        }

    private:

    };

}

#endif

