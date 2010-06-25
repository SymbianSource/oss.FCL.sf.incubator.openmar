/*
 * Name        : Logger.cpp
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

#include "Logger.h"

#include <bautils.h>
#include <PathInfo.h>

void CLogger::Write(const TDesC8& aText)
{
    CLogger::InstanceL().DoWrite(aText);
}

void CLogger::Write(TRefByValue<const TDesC8> aFmt, ...)
{
    VA_LIST list;
    VA_START(list, aFmt);

    CLogger::InstanceL().iBuffer.FormatList(aFmt, list);
    CLogger::InstanceL().DoWrite(CLogger::InstanceL().iBuffer);

    VA_END(list);
}

void CLogger::Close()
{
    delete static_cast<CLogger*>(UserSvr::DllTls(0));
    UserSvr::DllFreeTls(0);
}

CLogger& CLogger::InstanceL()
{
    CLogger* self = static_cast<CLogger*>(UserSvr::DllTls(0));

    if (self == 0)
    {
        self = new(ELeave) CLogger;
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(self);

        UserSvr::DllSetTls(0, self);
    }

    return *self;
}

void CLogger::ConstructL()
{
    User::LeaveIfError(iFs.Connect());

    TFileName logName(PathInfo::PhoneMemoryRootPath());
    TPtrC appName(BaflUtils::ExtractAppNameFromFullName(RThread().FullName()));
    logName.Append(appName);
    _LIT(KLogFileExt, ".log");
    logName.Append(KLogFileExt);

    BaflUtils::EnsurePathExistsL(iFs, logName);
    User::LeaveIfError(iFile.Replace(iFs, logName, EFileShareAny | EFileWrite));
}

CLogger::~CLogger()
{
    iFile.Close();
    iFs.Close();
}

void CLogger::DoWrite(const TDesC8& aText)
{
    const TInt KTimeRecordSize = 12;
    TBuf8<KTimeRecordSize> timeText;

    TTime time;
    time.HomeTime();
    TDateTime dateTime = time.DateTime();

    _LIT8(KTimeFormat,"%02d:%02d:%02d.%02d ");
    timeText.Format(KTimeFormat, dateTime.Hour(), dateTime.Minute(), dateTime.Second(), dateTime.MicroSecond());

    iFile.Write(timeText);
    iFile.Write(aText);

    _LIT8(KNewLine, "\r\n");
    iFile.Write(KNewLine);
//    iFile.Flush();
}
