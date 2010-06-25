/*
 * Name        : Accelerometer.h
 * Description : Accelerometer helper class
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

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include "Vector3d.h"

#include <SensrvDataListener.h>

class CSensrvChannel;

/**
 * @brief Wrapper class over sensor channel to retrieve data from 3-axis accelerometer
 * 
 * This class also implements a simple bandpass filter
 */
class CAccelerometer : public CBase, 
            public MSensrvDataListener
{
public:
    static CAccelerometer* NewL();
    ~CAccelerometer();

protected:
    CAccelerometer();
    void ConstructL();

public:
    void StartL();
    void Stop();

    const Vector3d GetValue() const { return Vector3d(iX1, iY1, iZ1); }

protected:
    // From MSensrvDataListener
    void DataReceived(CSensrvChannel& aChannel, TInt aCount, TInt aDataLost);
    void DataError(CSensrvChannel& aChannel, TSensrvErrorSeverity aError);
    void GetDataListenerInterfaceL(TUid aInterfaceUid, TAny*& aInterface);

private:
    CSensrvChannel* iSensorChannel;

    TReal iX1;
    TReal iY1;
    TReal iZ0;
    TReal iZ1;
    TReal iZ2;
};

#endif
