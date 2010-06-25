/*
 * Name        : Logger.h
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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <f32file.h>

#if defined(__DEBUG__)

#define LOGCLOSE                CLogger::Close();
#define LOGARG(text,param...)   { _LIT8(KText, text); CLogger::Write(KText, param); }
#define LOGTXT(text)            { _LIT8(KText, text); CLogger::Write(static_cast<const TDesC8&>(KText)); }

#else

#define LOGCLOSE
#define LOGARG
#define LOGTXT

#endif

class CLogger : public CBase
{
public:
    static void Write(const TDesC8& aText);
    static void Write(TRefByValue<const TDesC8> aFmt, ...);

    static void Close();

protected:
    static CLogger& InstanceL();

    void ConstructL();
    ~CLogger();

    void DoWrite(const TDesC8& aText);

private:
    RFs   iFs;
    RFile iFile;

    TBuf8<1024> iBuffer;
};

#endif
