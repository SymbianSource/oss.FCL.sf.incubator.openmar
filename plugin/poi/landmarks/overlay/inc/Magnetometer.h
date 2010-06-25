/*
 * Name        : Magnetometer.h
 * Description : Magnetometer helper class
 * Project     : This file is part of OpenMAR, an Open Mobile Augmented Reality browser
 * Website     : http://OpenMAR.org
 *
 * Copyright (c) 2010 David Caabeiro
 *
 * All rights reserved. This program and the accompanying materials are made available 
 * under the terms of the Eclipse Public License v1.0 which accompanies this 
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 *
 */

#ifndef MAGNETOMETER_H_
#define MAGNETOMETER_H_

#include "Vector3d.h"

#include <SensrvDataListener.h>
#include <SensrvPropertyListener.h>

class CSensrvChannel;

/**
 * @brief Wrapper class over sensor channel to retrieve data from 3-axis magnetometer
 * 
 * This class also implements a simple lowpass filter
 */
class CMagnetometer : public CBase,
            public MSensrvDataListener,
            public MSensrvPropertyListener
{
public:
    static CMagnetometer* NewL();
    ~CMagnetometer();

protected:
    CMagnetometer();
    void ConstructL();

public:
    void StartL();
    void Stop();

    const Vector3d GetValue() const { return Vector3d(iX, iY, iZ); }

    enum TCalibration {
        ENone,
        ELow,
        EModerate,
        EHigh
    };

    TCalibration Calibration() const { return iCalibration; }

protected:
    // From MSensrvDataListener
    void DataReceived(CSensrvChannel& aChannel, TInt aCount, TInt aDataLost);
    void DataError(CSensrvChannel& aChannel, TSensrvErrorSeverity aError);
    void GetDataListenerInterfaceL(TUid aInterfaceUid, TAny*& aInterface);

    // From MSensrvPropertyListener
    void PropertyChanged(CSensrvChannel& aChannel, const TSensrvProperty& aChangedProperty);
    void PropertyError(CSensrvChannel& aChannel, TSensrvErrorSeverity aError);
    void SetPropertySuccessIndicationChanged(TSetPropertySuccessIndicator aIndication);
    void GetPropertyListenerInterfaceL(TUid aInterfaceUid, TAny*& aInterface);

private:
    CSensrvChannel* iSensorChannel;

    TReal iX;
    TReal iY;
    TReal iZ;

    TCalibration iCalibration;
};

#endif
