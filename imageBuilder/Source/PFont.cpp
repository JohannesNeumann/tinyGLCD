/*
  ==============================================================================

    PFont.cpp
    Created: 26 Mar 2015 4:33:51pm
    Author:  Johannes Neumann

  ==============================================================================
*/

#include "PFont.h"
#include "errorlog.h"

PFont::PFont()
{
	height = 0;
	data.resize(0);
	widths.resize(0);
	name = "";
}

PFont::PFont(const PFont& other)
{
	operator=(other);
}

PFont& PFont::operator=(const PFont& other)
{
	height = other.height;
	data.clear();
	data.addArray(other.data);
	widths.clear();
	widths.addArray(other.widths);
	name = other.name;

	return *this;
}

PFont::~PFont()
{
}

void PFont::addFromFile(File f)
{
	if (!f.existsAsFile())
	{
		ErrorLog::addError("File not found: '" + f.getFullPathName() + "'.");
		return;
	}

	// find index in multi image (==0 if no multi image)
	int index = f.getFileNameWithoutExtension().getTrailingIntValue();

	String FontName = f.getFileNameWithoutExtension().trimCharactersAtEnd("_0123456789");
	if (data.size() <= index)
	{
		data.resize(index);
	}

	// load file
	juce::Image im = ImageFileFormat::loadFrom(f);

	// file okay?
	if (im.isNull())
	{
		ErrorLog::addError("Can't read input file '" + f.getFullPathName() + "'.");
		return;
	}

	// first file in the font? Adjust size
	if (height == 0)
	{
		height = im.getHeight();
		name = FontName;
	}
	// not first file in multi image -> does it fit the proportions?
	else if (height != im.getHeight())
	{
		ErrorLog::addWarning("Image file '" + f.getFullPathName() + "' is part of a font with name '" + name + 
			"' but does not have the expected heigh of " + String(height) + "px. Additional data will be ignored.");
	}
	
	// character already existing?
	if (data[index].get() != nullptr)
	{
		ErrorLog::addWarning("Image file '" + f.getFullPathName() + "' overwrites an already existing character in font with name '" + name + "'.");
	}

	// no other file there already => create new data buffer
	int numRowBytes = (int) ceil((float)height / 8.0f);

	if (data.size() < index + 1)
		data.resize(index + 1);
	data.set(index, std::shared_ptr<juce::Array<uint8_t> >(new Array<uint8_t>()));
	data[index]->resize((uint8_t) im.getWidth() * numRowBytes);

	if (widths.size() < index + 1)
		widths.resize(index + 1);
	widths.set(index, (uint8_t) im.getWidth());

	// parse character image
	for (int y8 = 0; y8 < numRowBytes; y8++)
	{
		for (int x = 0; x < widths[index]; x++)
		{
			uint8_t d = 0;
			for (int y = 0; y < 8; y++)
			{
				d = d>>1;
				if (im.getHeight() >  y + y8 * 8)
				{
					if (im.getPixelAt(x, y + y8 * 8).getBrightness() < 0.5)
						d |= 0x80;
				}
			}
			data[index]->setUnchecked(y8 * widths[index] + x, d);
		}
	}
}

int PFont::getNumBytesInPixeldata(int index) const
{
	int numRowBytes = (int) ceil((double)height / 8.0);
	return widths[index] * numRowBytes;
}

int PFont::getWidth(int index) const
{
	return widths[index];
}

int PFont::getHeight() const 
{
	return height;
}

int PFont::getByteHeight() const 
{
	int numRowBytes = (int) ceil((double)height / 8.0);
	return numRowBytes;
}

// returns a new buffer of pixeldata, does not take ownership
const uint8_t* PFont::getPixeldata(int image) const
{
	if (data[image].get() == nullptr)
		return nullptr;
	return data[image]->getRawDataPointer();
}


int PFont::getNumCharacters() const
{
	return data.size();
}