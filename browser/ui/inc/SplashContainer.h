/*
 * Name        : SplashContainer.h
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

#ifndef SPLASHCONTAINER_H_
#define SPLASHCONTAINER_H_

#include <coecntrl.h>
#include <coemain.h>

class CAknView;
class CFbsBitmap;
class CGulIcon;

class CSplashContainer : public CCoeControl
{
public:
    CSplashContainer(CAknView& aView);
    ~CSplashContainer();
    void ConstructL(const TRect& aRect, const CCoeControl* aParent = 0);

protected:
    void HandleResourceChange(TInt aType);
    void Draw(const TRect& aRect) const;

private:
    CAknView& iView;

    CFbsBitmap* iBitmap;
    CFbsBitmapDevice* iBitmapDevice;
    CFbsBitGc* iBitmapContext;

    CGulIcon* iBackground;
};

#endif  // SPLASHCONTAINER_H_
