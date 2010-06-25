/*
 * Name        : Accelerometer.cpp
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

#include "Accelerometer.h"

#include <SensrvAccelerometerSensor.h>

#include <SensrvChannel.h>
#include <SensrvChannelFinder.h>
#include <SensrvChannelInfo.h>

CAccelerometer* CAccelerometer::NewL()
{
    CAccelerometer* self = new(ELeave) CAccelerometer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CAccelerometer::~CAccelerometer()
{
    iSensorChannel->CloseChannel();
}

CAccelerometer::CAccelerometer()
{}

void CAccelerometer::ConstructL()
{
    CSensrvChannelFinder* sensorChannelFinder = CSensrvChannelFinder::NewLC();

    RSensrvChannelInfoList channelInfoList;
    CleanupClosePushL(channelInfoList);

    TSensrvChannelInfo mySearchConditions;
    mySearchConditions.iChannelType = KSensrvChannelTypeIdAccelerometerXYZAxisData;

    sensorChannelFinder->FindChannelsL(channelInfoList, mySearchConditions);

    TSensrvChannelInfo channelInfo;

    if (channelInfoList.Count() > 0)
        channelInfo = channelInfoList[0];
    else
        User::Leave(KErrNotFound);

    CleanupStack::PopAndDestroy(&channelInfoList);
    CleanupStack::PopAndDestroy(sensorChannelFinder);

    iSensorChannel = CSensrvChannel::NewL(channelInfo);
    iSensorChannel->OpenChannelL();
}

void CAccelerometer::StartL()
{
    iSensorChannel->StartDataListeningL(this, 1, 8, 0);
}

void CAccelerometer::Stop()
{
    iSensorChannel->StopDataListening();
}

void CAccelerometer::DataReceived(CSensrvChannel& aChannel, TInt aCount, TInt aDataLost)
{
    if (aChannel.GetChannelInfo().iChannelType ==  KSensrvChannelTypeIdAccelerometerXYZAxisData)
    {
        TPckgBuf<TSensrvAccelerometerAxisData> dataBuffer;
        TSensrvAccelerometerAxisData data;

        for (TInt i = 0; i < aCount; ++i)
        {
            aChannel.GetData(dataBuffer);

            data = dataBuffer();
            // Do something with the date in data variable
            // data.iTimeStamp

            // Compensate axis for landscape mode
            TInt x = -data.iAxisY;
            TInt y =  data.iAxisX;
            TInt z =  data.iAxisZ;
#if 0
            TInt x = data.iAxisX;
            TInt y = data.iAxisY;
            TInt z = data.iAxisZ;
#endif
            const TReal K128 = 0.0078125;   // 1/128
            const TReal KPole1 = 0.97;      // Pole of lowpass filter applied to X and Y
            const TReal KPole2 = 0.90;      // Pole of lowpass filter applied to Z
            const TReal KPole3 = 0.75;      // Pole of highpass filter applied to Z

            // Single-pole lowpass-filtering of X and Y
            iX1 = KPole1 * iX1 + ( 1.0 - KPole1 ) * ( K128 * x );
            iY1 = KPole1 * iY1 + ( 1.0 - KPole1 ) * ( K128 * y );

            // Single-pole lowpass-filtering of Z
            iZ0 = KPole2 * iZ1 + ( 1.0 - KPole2 ) * ( K128 * z );
            // 1st order highpass-filtering of Z (zero at 1)
            iZ2 = KPole3 * iZ2 + ( 1.0 - KPole3 ) * ( iZ0 - iZ1 );

            // Update variables for use at next sample
            iZ1 = iZ0;
        }
    }
}

void CAccelerometer::DataError(CSensrvChannel& aChannel, TSensrvErrorSeverity aError)
{
}

void CAccelerometer::GetDataListenerInterfaceL(TUid aInterfaceUid, TAny*& aInterface)
{}
