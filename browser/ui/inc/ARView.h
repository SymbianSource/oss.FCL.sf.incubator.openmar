/*
 * Name        : ARView.h
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

#ifndef __ARVIEW_H__
#define __ARVIEW_H__

#include <AknView.h>

#include <AknToolbar.h>
#include <AknToolbarExtension.h>
#include <AknToolbarObserver.h>

class CARContainer;

/**
 * @brief AR Avkon view
 */
class CARView : public CAknView, public MAknToolbarObserver
{
public:
    static CARView* NewL();
    ~CARView();

protected:
    CARView();
    void ConstructL();

protected:
    TUid Id() const;
    void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage);
    void DoDeactivate();

    void DynInitToolbarL(TInt aResourceId, CAknToolbar* aToolbar);

    void OfferToolbarEventL(TInt aCommand);
    void HandleCommandL(TInt aCommand);

private:
    TVwsViewId iPrevViewId;
    CARContainer* iRenderer;
};

#endif  // __ARVIEW_H__
