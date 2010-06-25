/*
 * Name        : AutoRotation.h
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

#ifndef AUTOROTATION_H_
#define AUTOROTATION_H_

#include <e32base.h>

class CRepository;

/**
 * @brief Handles device autorotation setting
 * 
 * The N97 has an issue where the compass gets uncalibrated if autorotation is enabled.
 * 
 */
class CAutoRotation : public CActive
{
public:
    static CAutoRotation* NewL();
    ~CAutoRotation();

protected:
    CAutoRotation();
    void ConstructL();

    void RunL();
    void DoCancel();

    void Notify();

public:
    void ResetL();
    void RestoreL();

private:
    CRepository* iRepository;
    TInt iTurnCtrl;
};

#endif  // AUTOROTATION_H_
