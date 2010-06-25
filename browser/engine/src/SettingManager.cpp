/*
 * Name        : SettingManager.cpp
 * Description : Manages application settings
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

#include "SettingManager.h"

CSettingManager* CSettingManager::NewL()
{
    CSettingManager* self = new(ELeave) CSettingManager;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CSettingManager::~CSettingManager()
{
}

CSettingManager::CSettingManager()
{
}

void CSettingManager::ConstructL()
{
}
