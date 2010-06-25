/*
 * Name        : SettingManager.h
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

#ifndef SETTINGMANAGER_H_
#define SETTINGMANAGER_H_

#include <e32base.h>

/**
 * @brief Manages application global settings
 * 
 */
class CSettingManager : public CBase
{
public:
    static CSettingManager* NewL();
    ~CSettingManager();

protected:
    CSettingManager();
    void ConstructL();

private:
};

#endif
