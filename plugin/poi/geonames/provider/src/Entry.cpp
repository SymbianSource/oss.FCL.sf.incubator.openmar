/*
 * Name        : Entry.cpp
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

#include "Entry.h"

CEntry* CEntry::NewL()
{
    CEntry* self = new(ELeave) CEntry;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CEntry::~CEntry()
{}

CEntry::CEntry()
{}

void CEntry::ConstructL()
{}

void CEntry::Reset()
{
    iTitle.Zero();
    iSummary.Zero();
    iElevation.Zero();
    iLat.Zero();
    iLng.Zero();
    iWikipediaUrl.Zero();
}

void CEntry::EnterState(const TDesC8& aTag)
{
    _LIT8(KTitleTag, "title");
    _LIT8(KSummaryTag, "summary");
//    _LIT8(KFeatureTag, "feature");
    _LIT8(KElevationTag, "elevation");
    _LIT8(KLatTag, "lat");
    _LIT8(KLngTag, "lng");
    _LIT8(KWikipediaUrlTag, "wikipediaUrl");

    if (aTag == KTitleTag)
        iState = ETitle;
    else if (aTag == KSummaryTag)
        iState = ESummary;
    else if (aTag == KElevationTag)
        iState = EElevation;
    else if (aTag == KLatTag)
        iState = ELat;
    else if (aTag == KLngTag)
        iState = ELng;
    else if (aTag == KWikipediaUrlTag)
        iState = EWikipediaUrl;
    else
        iState = EUnknown;
}

void CEntry::ExitState()
{
    iState = EUnknown;
}

void CEntry::FeedState(const TDesC8& aBytes)
{
    switch (iState)
    {
        case ETitle:
        {   
            TInt space = iTitle.MaxLength() - iTitle.Length();
            TInt min = Min(space, aBytes.Length());
            iTitle.Append(aBytes.Left(min));
            break;
        }

        case ESummary:
        {
            TInt space = iSummary.MaxLength() - iSummary.Length();
            TInt min = Min(space, aBytes.Length());
            iSummary.Append(aBytes.Left(min));
            break;
        }

        case EElevation:
        {
            TInt space = iElevation.MaxLength() - iElevation.Length();
            TInt min = Min(space, aBytes.Length());
            iElevation.Append(aBytes.Left(min));
            break;
        }

        case ELat:
        {
            TInt space = iLat.MaxLength() - iLat.Length();
            TInt min = Min(space, aBytes.Length());
            iLat.Append(aBytes.Left(min));
            break;
        }

        case ELng:
        {
            TInt space = iLng.MaxLength() - iLng.Length();
            TInt min = Min(space, aBytes.Length());
            iLng.Append(aBytes.Left(min));
            break;
        }

        case EWikipediaUrl:
        {
            TInt space = iWikipediaUrl.MaxLength() - iWikipediaUrl.Length();
            TInt min = Min(space, aBytes.Length());
            iWikipediaUrl.Append(aBytes.Left(min));
            break;
        }
    
        default:
            break;
    }
}
