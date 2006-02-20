/*
For general Scribus (>=1.3.2) copyright and licensing information please refer
to the COPYING file provided with the program. Following this notice may exist
a copyright and/or license notice that predates the release of Scribus 1.3.2
for which a new license (GPL+exception) is in place.
*/
/***************************************************************************
						main.cpp  -  description
							-------------------
	begin                : Fre Apr  6 21:47:55 CEST 2001
	copyright            : (C) 2001 by Franz Schmid
	email                : Franz.Schmid@altmuehlnet.de
	copyright            : (C) 2004 by Alessandro Rimoldi
	email                : http://ideale.ch/contact
	copyright            : (C) 2005 by Craig Bradney
	email                : cbradney@zip.com.au
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#include <iostream>
#include <signal.h>

#ifndef _WIN32
#error "This file compiles on win32 platform only"
#endif

#include <qapplication.h>
#include <exception>
using namespace std;

#define BASE_QM "scribus"

#include "scribusapp.h"
#include "scribus.h"

#include "scconfig.h"

#include <windows.h>

int mainApp(ScribusQApp& app);
static LONG exceptionFilter(DWORD exceptionCode);
static QString exceptionDescription(DWORD exceptionCode);
static void defaultCrashHandler(DWORD exceptionCode);

ScribusMainWindow SCRIBUS_API *ScMW;
ScribusQApp SCRIBUS_API *ScQApp;
bool emergencyActivated;

int main(int argc, char *argv[])
{
	int result;
	emergencyActivated = false;
	ScribusQApp app(argc, argv);
	result =  mainApp(app);
	return result;
}

/*!
\fn int mainApp(ScribusQApp& app)
\author Franz Schmid
\author Alessandro Rimoldi
\author Craig Bradney
\author Jean Ghali
\date Mon Feb  9 14:07:46 CET 2004
\brief Launches the Gui
\param app a Scribus QApplication object
\retval int Error code from the execution of Scribus
*/
int mainApp(ScribusQApp& app)
{
	int appRetVal;
#ifndef _DEBUG
	__try
	{
#endif
		app.parseCommandLine();
		if (app.usingGUI())
		{
			appRetVal = app.init();
			if (appRetVal != EXIT_FAILURE)
				appRetVal = app.exec();
		}
#ifndef _DEBUG
	}
	__except( exceptionFilter(GetExceptionCode()) )
	{
		defaultCrashHandler( GetExceptionCode() );
	}
#endif
	return appRetVal;
}

/*!
\fn int exceptionFilter(DWORD exceptionCode)
\author Jean Ghali
\date Sun Oct 30 14:30:30 CET 2005
\brief describe the behavior to be adopted against specific exception
\param DWORD exceptionCode the value returned by GetExceptionCode()
\retval EXCEPTION_EXECUTE_HANDLER if handler is to be executed or EXCEPTION_CONTINUE_EXECUTION if not
*/
LONG exceptionFilter(DWORD exceptionCode)
{
 LONG result;
	switch( exceptionCode )
	{
	case EXCEPTION_ACCESS_VIOLATION:
	case EXCEPTION_DATATYPE_MISALIGNMENT:
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
	case EXCEPTION_FLT_DENORMAL_OPERAND:
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	case EXCEPTION_FLT_INEXACT_RESULT:
	case EXCEPTION_FLT_INVALID_OPERATION:
	case EXCEPTION_FLT_OVERFLOW:
	case EXCEPTION_FLT_STACK_CHECK:
	case EXCEPTION_FLT_UNDERFLOW:
	case EXCEPTION_ILLEGAL_INSTRUCTION:
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
	case EXCEPTION_INT_OVERFLOW:
	case EXCEPTION_INVALID_HANDLE:
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
	case EXCEPTION_STACK_OVERFLOW:
		result = EXCEPTION_EXECUTE_HANDLER;
		break;
	case EXCEPTION_BREAKPOINT:
	case EXCEPTION_SINGLE_STEP:
		result = EXCEPTION_CONTINUE_EXECUTION;
		break;
	default:
		result = EXCEPTION_EXECUTE_HANDLER;
		break;
	}
	return result;
}

/*!
\fn int exceptionDescription(DWORD exceptionCode)
\author Jean Ghali
\date Sun Oct 30 14:30:30 CET 2005
\brief return a string describing a specific exception
\param DWORD exceptionCode the value returned by GetExceptionCode()
\retval string describing the specified exception
*/
static QString exceptionDescription(DWORD exceptionCode)
{
 QString description;
	if ( exceptionCode == EXCEPTION_ACCESS_VIOLATION )
		description = "EXCEPTION_ACCESS_VIOLATION";
	else if ( exceptionCode == EXCEPTION_DATATYPE_MISALIGNMENT )
		description = "EXCEPTION_DATATYPE_MISALIGNMENT";
	else if ( exceptionCode == EXCEPTION_ARRAY_BOUNDS_EXCEEDED )
		description = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
	else if ( exceptionCode == EXCEPTION_FLT_DENORMAL_OPERAND )
		description = "EXCEPTION_FLT_DENORMAL_OPERAND";
	else if ( exceptionCode == EXCEPTION_FLT_DIVIDE_BY_ZERO )
		description = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
	else if ( exceptionCode == EXCEPTION_FLT_INEXACT_RESULT )
		description = "EXCEPTION_FLT_INEXACT_RESULT";
	else if ( exceptionCode == EXCEPTION_FLT_INVALID_OPERATION )
		description = "EXCEPTION_FLT_INVALID_OPERATION";
	else if ( exceptionCode == EXCEPTION_FLT_OVERFLOW )
		description = "EXCEPTION_FLT_OVERFLOW";
	else if ( exceptionCode == EXCEPTION_FLT_STACK_CHECK )
		description = "EXCEPTION_FLT_STACK_CHECK";
	else if ( exceptionCode == EXCEPTION_FLT_UNDERFLOW )
		description = "EXCEPTION_FLT_UNDERFLOW";
	else if ( exceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION )
		description = "EXCEPTION_ILLEGAL_INSTRUCTION";
	else if ( exceptionCode == EXCEPTION_INT_DIVIDE_BY_ZERO )
		description = "EXCEPTION_INT_DIVIDE_BY_ZERO";
	else if ( exceptionCode == EXCEPTION_INT_OVERFLOW )
		description = "EXCEPTION_INT_OVERFLOW";
	else if ( exceptionCode == EXCEPTION_INVALID_HANDLE )
		description = "EXCEPTION_INVALID_HANDLE";
	else if ( exceptionCode == EXCEPTION_NONCONTINUABLE_EXCEPTION )
		description = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
	else if ( exceptionCode == EXCEPTION_STACK_OVERFLOW )
		description = "EXCEPTION_STACK_OVERFLOW";
	else
		description = "UNKNOWN EXCEPTION";
	return description;
}

void defaultCrashHandler(DWORD exceptionCode)
{
	static int crashRecursionCounter = 0;
	crashRecursionCounter++;
	if (crashRecursionCounter < 2)
	{
		emergencyActivated=true;
		crashRecursionCounter++;
		QString expDesc = exceptionDescription(exceptionCode);
		QString expHdr=QObject::tr("Scribus Crash");
		QString expLine="-------------";
		QString expMsg=QObject::tr("Scribus crashes due to the following exception : %1").arg(expDesc);
		std::cout << (const char*) expHdr << std::endl;
		std::cout << (const char*) expLine << std::endl;
		std::cout << (const char*) expMsg << std::endl;
		if (ScribusQApp::useGUI)
		{
			ScMW->closeSplash();
			QMessageBox::critical(ScMW, expHdr, expMsg, QObject::tr("&OK"));
			ScMW->emergencySave();
			ScMW->close();
		}
	}
	ExitProcess(255);
}
