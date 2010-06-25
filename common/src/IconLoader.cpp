/*
 * Name        : IconLoader.cpp
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

#include "IconLoader.h"

#include <AknIconUtils.h>
#include <GulIcon.h>

CGulIcon* CreateIconL(const TDesC& aIconFile, TInt aBitmapId, TInt aMaskId, const TSize& aSize)
{
    CFbsBitmap* bitmap = 0;
    CFbsBitmap* mask = 0;

    AknIconUtils::CreateIconL(bitmap, mask, aIconFile, aBitmapId, aMaskId); 

    AknIconUtils::PreserveIconData(bitmap);

    TSize size;

    if (aSize.iHeight == -1 || aSize.iWidth == -1)
        AknIconUtils::GetContentDimensions(bitmap, size);
    else
        size = aSize;

    AknIconUtils::SetSize(bitmap, size, EAspectRatioPreserved);
    AknIconUtils::SetSize(mask, size, EAspectRatioPreserved);

    AknIconUtils::DestroyIconData(bitmap);

    CleanupStack::PushL(bitmap);
    CleanupStack::PushL(mask);

    CGulIcon* icon = CGulIcon::NewL(bitmap, mask);

    CleanupStack::Pop(2, bitmap);

    return icon;
}
