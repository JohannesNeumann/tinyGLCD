/*
  ==============================================================================

    PFont.h
    Created: 26 Mar 2015 4:33:51pm
    Author:  Johannes Neumann

  ==============================================================================
*/

#ifndef PFONT_H_INCLUDED
#define PFONT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <stdint.h>
#include <map>
#include <memory>

class PFont
{
public:
	PFont();
	PFont(const PFont& other);
	PFont& operator=(const PFont& other);
	~PFont();

	void addFromFile(File f);
	int getWidth(int index) const;
	int getHeight() const;
	int getByteHeight() const;
	int getNumCharacters() const;
	String getFontName();

	// returns a buffer of pixeldata, does take ownership
	const uint8_t* getPixeldata(int image) const; 
	int getNumBytesInPixeldata(int image) const;
private:
	Array<std::shared_ptr<Array<uint8_t> > > data;
	Array<uint8_t> widths;
	int height;
	String name;
};



#endif  // PFONT_H_INCLUDED
