/*
  ==============================================================================

    errorlog.h
    Created: 26 Mar 2015 10:44:49am
    Author:  Johannes Neumann

  ==============================================================================
*/

#ifndef ERRORLOG_H_INCLUDED
#define ERRORLOG_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class ErrorLog
{
public:
	static void newLog(File f);
	static void stopLog();

	static void addLine(String line);
	static void addWarning(String warning);
	static void addError(String error);
	
	static int getNumWarnings() { return numWarnings; }
	static int getNumErrors() { return numErrors; }

private:
	static ScopedPointer<FileLogger> logger;
	static int numWarnings;
	static int numErrors;
};



#endif  // ERRORLOG_H_INCLUDED
