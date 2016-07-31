/*
  ==============================================================================

    Parser.h
    Created: 26 Mar 2015 8:48:30am
    Author:  Johannes Neumann

  ==============================================================================
*/

#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <stdint.h>
#include <map>
#include <memory>
#include "PFont.h"
#include "PMultiImage.h"

class Parser {
public:
	class Listener {
	public:
		virtual ~Listener() {}
		virtual void progressChanged(double newProgress) = 0;
		virtual void parsingStarted() = 0;
		virtual void parsingStopped() = 0;
	};

	Parser();
	~Parser();

	void parseFolder (File inputFolder, File outputFolder);

	void addListener(Listener* l);
	void removeListener(Listener* l);
    
    bool isRunning() { return parsingThread.isThreadRunning(); }

private:
	static String createArray(String name, int numTabsIn, const uint8_t* data, int numElements, int maxWidthInCharacters);
	std::map<String, PMultiImage> images;
	std::map<String, PFont> fonts;
	ListenerList<Listener> listeners;

	class VoidableCallbackMessage
	{
	public:
		VoidableCallbackMessage(Parser* p)
		{
			parser = p;
			parser->messages.add(this);
			valid = true;
		}
		~VoidableCallbackMessage() 
		{
			parser->messages.remove(this);
		}
			
		bool isValid() { return valid; }
	protected:
		Parser* parser;
	private:
		bool valid;
	};

	ListenerList<VoidableCallbackMessage> messages;

	class ParsingThread: public Thread
	{
	public:
		ParsingThread(Parser* p);
		virtual void run();

		void startParsing(File input, File output);
	private:
		Parser* p;
		File inputFolder;
		File outputFolder;
	} parsingThread;
};



#endif  // PARSER_H_INCLUDED
