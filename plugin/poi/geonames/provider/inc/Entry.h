/*
 * Name        : Entry.h
 * Description : Encapsulates a findNearbyWikipedia entry
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

#ifndef ENTRY_H_
#define ENTRY_H_

#include <e32base.h>
#include <EPos_CPosLandmark.h>

class CEntry : public CBase
{
public:
    static CEntry* NewL();
    ~CEntry();

protected:
    CEntry();
    void ConstructL();

public:
    void Reset();

    void EnterState(const TDesC8& aTag);
    void ExitState();
    void FeedState(const TDesC8& aBytes);

//    TBool IsValid() const;

protected:

private:
    enum TState {
        EUnknown,
        ETitle,
        ESummary,
//        EFeature,
        EElevation,
        ELat,
        ELng,
        EWikipediaUrl
    } iState;

//private:
public:
    // Limits should be defined by provider (geonames.org)
    TBuf8<KPosLmMaxTextFieldLength> iTitle;
    TBuf8<KPosLmMaxDescriptionLength> iSummary;
//    TBuf8<KPosLmMaxCategoryNameLength> iFeature;
    TBuf8<KDefaultRealWidth> iElevation;
    TBuf8<KDefaultRealWidth> iLat;
    TBuf8<KDefaultRealWidth> iLng;
    TBuf8<KPosLmMaxTextFieldLength> iWikipediaUrl;
};

#endif  // ENTRY_H_
