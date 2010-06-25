/*
 * Name        : Magnetometer.cpp
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

#include "Magnetometer.h"

#include <SensrvMagnetometerSensor.h>

#include <SensrvChannel.h>
#include <SensrvChannelFinder.h>
#include <SensrvChannelInfo.h>
#include <SensrvProperty.h>

CMagnetometer* CMagnetometer::NewL()
{
    CMagnetometer* self = new(ELeave) CMagnetometer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CMagnetometer::~CMagnetometer()
{
    iSensorChannel->CloseChannel();
}

CMagnetometer::CMagnetometer()
{}

void CMagnetometer::ConstructL()
{
    CSensrvChannelFinder* sensorChannelFinder = CSensrvChannelFinder::NewLC();

    RSensrvChannelInfoList channelInfoList;
    CleanupClosePushL(channelInfoList);

    TSensrvChannelInfo mySearchConditions;
    mySearchConditions.iChannelType = KSensrvChannelTypeIdMagnetometerXYZAxisData;

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

    // Get current calibration value
    TSensrvProperty property;
    iSensorChannel->GetPropertyL(KSensrvPropCalibrationLevel, KSensrvItemIndexNone, property);
    TInt calibration = 0;
    property.GetValue(calibration);

    iCalibration = calibration == 0? ENone :
                   calibration == 1? ELow :
                   calibration == 2? EModerate :
                   calibration == 3? EHigh : ENone;

    iSensorChannel->SetPropertyListenerL(this);
}

void CMagnetometer::StartL()
{
    iSensorChannel->StartDataListeningL(this, 1, 4, 0);
}

void CMagnetometer::Stop()
{
    iSensorChannel->StopDataListening();
}

void CMagnetometer::DataReceived(CSensrvChannel& aChannel, TInt aCount, TInt aDataLost)
{
    if (aChannel.GetChannelInfo().iChannelType ==  KSensrvChannelTypeIdMagnetometerXYZAxisData)
    {
        TPckgBuf<TSensrvMagnetometerAxisData> dataBuffer;
        TSensrvMagnetometerAxisData data;

        for (TInt i = 0; i < aCount; ++i)
        {
            aChannel.GetData(dataBuffer);

            data = dataBuffer();
            // Do something with the date in data variable
            // data.iTimeStamp

            // Compensate axis for landscape mode
            TInt x = -data.iAxisYCalibrated;
            TInt y =  data.iAxisXCalibrated;
            TInt z =  data.iAxisZCalibrated;
#if 0
            TInt x = data.iAxisXCalibrated;
            TInt y = data.iAxisYCalibrated;
            TInt z = data.iAxisZCalibrated;
#endif
            const TReal KPole = 0.85;

            iX = KPole * iX + (1.0 - KPole) * x;
            iY = KPole * iY + (1.0 - KPole) * y;
            iZ = KPole * iZ + (1.0 - KPole) * z;
        }
    }
}

void CMagnetometer::DataError(CSensrvChannel& aChannel, TSensrvErrorSeverity aError)
{
}

void CMagnetometer::GetDataListenerInterfaceL(TUid aInterfaceUid, TAny*& aInterface)
{}

void CMagnetometer::PropertyChanged(CSensrvChannel& aChannel, const TSensrvProperty& aChangedProperty)
{
    TSensrvChannelInfo info = aChannel.GetChannelInfo();

    if (info.iChannelType == KSensrvChannelTypeIdMagnetometerXYZAxisData &&
            aChangedProperty.GetPropertyId() == KSensrvPropCalibrationLevel)
    {
        TInt calibration = 0;
        aChangedProperty.GetValue(calibration);

        iCalibration = calibration == 0? ENone :
                       calibration == 1? ELow :
                       calibration == 2? EModerate :
                       calibration == 3? EHigh : ENone;
    }
}

void CMagnetometer::PropertyError(CSensrvChannel& aChannel, TSensrvErrorSeverity aError)
{}

void CMagnetometer::SetPropertySuccessIndicationChanged(TSetPropertySuccessIndicator aIndication)
{}

void CMagnetometer::GetPropertyListenerInterfaceL(TUid aInterfaceUid, TAny*& aInterface)
{}
