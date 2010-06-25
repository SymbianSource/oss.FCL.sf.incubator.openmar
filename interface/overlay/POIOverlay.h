/*
 * Name        : POIOverlay.h
 * Description : ECOM interface definition
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

#ifndef POIOVERLAY_H_
#define POIOVERLAY_H_

#include "InterfaceUid.hrh"

#include <ecom/ecom.h>

class RWindow;
class TRect;
class CFbsBitmap;

namespace OpenMAR {

/**
 * @brief POI overlay interface definition
 * 
 * The overlay is in charge of getting input from sensors for position and orientation
 * estimation, and retrieving proper POIs from providers.
 *
 */
class CPOIOverlay : public CBase
{
public:
    struct SParameter {
        RWindow& iWindow;
        const TRect& iRect;
    };

public:
    static CPOIOverlay* NewL(const TDesC8& aMatch, SParameter& aParam);
    static CPOIOverlay* NewL(const TUid aImplementationUid, SParameter& aParam);
    ~CPOIOverlay();

    virtual void StartL() = 0;
    virtual void Stop() = 0;
    virtual const CFbsBitmap& RenderScene() = 0;

    virtual TInt GetFocusedPOI() = 0;

private:
    TUid iDtor_ID_Key;
};

#include "POIOverlay.inl"

}

#endif  // POIOVERLAY_H_
