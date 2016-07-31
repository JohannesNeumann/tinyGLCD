/*
  ==============================================================================

    PMultiImage.h
    Created: 26 Mar 2015 4:33:59pm
    Author:  Johannes Neumann

  ==============================================================================
*/

#ifndef PMULTIIMAGE_H_INCLUDED
#define PMULTIIMAGE_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include <stdint.h>
#include <map>
#include <memory>

class PMultiImage 
{
public:
	PMultiImage();
	PMultiImage(const PMultiImage& other);
	PMultiImage& operator=(const PMultiImage& other);
	~PMultiImage();

	void addSingleFromFile(File f);
	void loadFromMultiFile(File f, int ItemWidth);
	int getWidth() const;
	int getHeight() const;
	int getByteHeight() const;
	int getNumImages() const;
	//String getName();

	bool isComplete() const;

	// returns a buffer of pixeldata, does take ownership
	const uint8_t* getPixeldata(int image) const; 
	int getNumBytesInPixeldata() const;
private:
	
	bool addFromImage(Image im, int index);

	Array<std::shared_ptr<Array<uint8_t> > > data;
	int width, height;
	String name;
};


#endif  // PMULTIIMAGE_H_INCLUDED
