/*
 * Name        : AppUi.h
 * Description : 
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

#ifndef __APPUI_H__
#define __APPUI_H__

#include <AknViewAppUi.h>

class CSettingManager;

class CAppUi : public CAknViewAppUi
{
public:
    void ConstructL();
    ~CAppUi();

private:
    void HandleCommandL(TInt aCommand);
    CArrayFix<TCoeHelpContext>* HelpContextL() const;

private:
    CSettingManager* iSetting;
};

#endif // __APPUI_H__
