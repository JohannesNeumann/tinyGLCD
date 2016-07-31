/*
  ==============================================================================

    errorlog.cpp
    Created: 26 Mar 2015 10:44:49am
    Author:  Johannes Neumann

  ==============================================================================
*/

#include "errorlog.h"



void ErrorLog::newLog(File f)
{
	logger = ScopedPointer<FileLogger>(new FileLogger(f, "============ Processing started ============", 0));
	numErrors = 0;
	numWarnings = 0;
}

void ErrorLog::addError(String error)
{

	logger->logMessage("Error: " + error);
    std::cout << "Error: " + error << newLine;
	numErrors++;
}

void ErrorLog::addLine(String line)
{
    logger->logMessage(line);
    std::cout << line << newLine;
}

void ErrorLog::addWarning(String warning)
{
    logger->logMessage("Warning: " + warning);
    std::cout << "Warning: " + warning << newLine;
	numWarnings++;
}


void ErrorLog::stopLog()
{
	delete logger.release();
}

ScopedPointer<FileLogger> ErrorLog::logger;
int ErrorLog::numErrors = 0;
int ErrorLog::numWarnings = 0;