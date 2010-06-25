/*
 * Name        : AutoRotation.cpp
 * Description : Handles device autorotation setting
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

#include "AutoRotation.h"

#include "Logger.h"

#include <CentralRepository.h>

// Taken from SensorPluginDomainCrKeys.h
const TUid KCRUidSensorSettings = { 0x10282DF0 };

/*
 * Bitmask of variated use cases for Turning interaction.
 *
 * Possible values:
 *  0000 0000: No selected use cases, Turning interactions disabled
 *  0000 0001: Only Silence calls use case enabled
 *  0000 0010: Only Snooze alarm use case enabled
 *  0000 0100: Only Display orientation use case enabled
 *
 * Bitmask may contain different variations of above selections
 *
 * Value is an integer.
 *
 * Default value: 7
 */

//const TUint32 KSenSetVariationTurnCtrl = 0x00100003;
const TUint32 KSenSettingsTurnCtrl     = 0x00000003;

CAutoRotation* CAutoRotation::NewL()
{
    CAutoRotation* self = new(ELeave) CAutoRotation;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CAutoRotation::~CAutoRotation()
{
    Cancel();
}

CAutoRotation::CAutoRotation()
    : CActive(CActive::EPriorityStandard)
{
    CActiveScheduler::Add(this);
}

void CAutoRotation::ConstructL()
{
    LOGTXT("CAutoRotation::ConstructL()");

    iRepository = CRepository::NewL(KCRUidSensorSettings);

    RunL();
}

void CAutoRotation::RunL()
{
    // Store setting value upon any external change
    User::LeaveIfError(iRepository->Get(KSenSettingsTurnCtrl, iTurnCtrl));

    Notify();
}

void CAutoRotation::DoCancel()
{
    iRepository->NotifyCancel(KSenSettingsTurnCtrl);
}

void CAutoRotation::Notify()
{
    iRepository->NotifyRequest(KSenSettingsTurnCtrl, iStatus);
    SetActive();
}

/*
 * Turn autorotation off
 */
void CAutoRotation::ResetL()
{
    // We don't want to get notification of our own modifications
    Cancel();

    TInt turnCtrl = iTurnCtrl & 0xfffffffb;
    User::LeaveIfError(iRepository->Set(KSenSettingsTurnCtrl, turnCtrl));

    Notify();
}

/*
 * Restore original value
 */
void CAutoRotation::RestoreL()
{
    // We don't want to get notification of our own modifications
    Cancel();

    User::LeaveIfError(iRepository->Set(KSenSettingsTurnCtrl, iTurnCtrl));

    Notify();
}
