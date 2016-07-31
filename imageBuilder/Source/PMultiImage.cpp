/*
  ==============================================================================

    PMultiImage.cpp
    Created: 26 Mar 2015 4:33:59pm
    Author:  Johannes Neumann

  ==============================================================================
*/

#include "PMultiImage.h"
#include "errorlog.h"

PMultiImage::PMultiImage()
{
	width = 0;
	height = 0;
	data.resize(0);
	name = "";
}

PMultiImage::PMultiImage(const PMultiImage& other)
{
	operator=(other);
}

PMultiImage& PMultiImage::operator=(const PMultiImage& other)
{
	height = other.height;
	width = other.width;
	data.clear();
	data.addArray(other.data);
	name = other.name;

	return *this;
}

PMultiImage::~PMultiImage()
{
}

void PMultiImage::addSingleFromFile(File f)
{
	if (!f.existsAsFile())
	{
		ErrorLog::addError("File not found: '" + f.getFullPathName() + "'.");
		return;
	}

	// find index in multi image (==0 if no multi image)
	int index = f.getFileNameWithoutExtension().getTrailingIntValue();
	name = f.getFileNameWithoutExtension().trimCharactersAtEnd("_0123456789");
	if (data.size() <= index)
	{
		data.resize(index);
	}

	// load file
	juce::Image im = ImageFileFormat::loadFrom(f);

	// first file in multi-image? Adjust size
	if (width == 0 && height == 0)
	{
		width = im.getWidth();
		height = im.getHeight();
	}

	if (!addFromImage(im, index))
	{
		ErrorLog::addError("Can't read input file '" + f.getFullPathName() + "'.");
	}
}


void PMultiImage::loadFromMultiFile(File f, int ItemWidth)
{
	if (!f.existsAsFile())
	{
		ErrorLog::addError("File not found: '" + f.getFullPathName() + "'.");
		return;
	}

	name = f.getFileNameWithoutExtension().trimCharactersAtEnd("_0123456789");

	// load file
	juce::Image im = ImageFileFormat::loadFrom(f);

	width = ItemWidth;
	height = im.getHeight();

	int numItems = im.getWidth() / ItemWidth;
	if (numItems * ItemWidth != im.getWidth())
	{
		ErrorLog::addError("This file does not have a width of n * itemWidth.");
		return;
	}

	data.clear();
	
	if (data.size() <= numItems)
	{
		data.resize(numItems);
	}

	for (int i = 0; i < numItems; i++)
	{
		Image subIm = im.getClippedImage(Rectangle<int>(ItemWidth * i, 0, ItemWidth, im.getHeight()));
		if (!addFromImage(subIm, i))
		{
			ErrorLog::addError("Can't parse input file '" + f.getFullPathName() + "'.");
		}
	}
}

bool PMultiImage::addFromImage(Image im, int index)
{
	// file okay?
	if (im.isNull())
	{
		return false;
	}

	// not first file in multi image -> does it fit the proportions?
	else if (width != im.getWidth() || height != im.getHeight())
	{
		ErrorLog::addWarning("Trying to add an image that does not have the expected proportions of (" + String(width) + ", " + String(height) + "). Additional data will be ignored.");
	}
	
	// another file there already?
	if ((data.size() < index + 1) || (data[index].get() == nullptr))
	{
		// no other file there already => create new data buffer
		int numRowBytes = (int) ceil((float)height / 8.0f);
		data.set(index, std::shared_ptr<juce::Array<uint8_t> >(new Array<uint8_t>()));
		data[index]->resize(width * numRowBytes);
	}
	else 
	{
		ErrorLog::addWarning("This image overwrites an already existing image resource with name '" + name + "'[" + String(index) + "].");
	}

	// parse image
	int numRowBytes = (int) ceil((double)height / 8.0);
	for (int y8 = 0; y8 < numRowBytes; y8++)
	{
		for (int x = 0; x < width; x++)
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
			data[index]->setUnchecked(y8 * width + x, d);
		}
	}

	return true;
}

int PMultiImage::getNumBytesInPixeldata() const
{
	int numRowBytes = (int) ceil((double)height / 8.0);
	return width * numRowBytes;
}

int PMultiImage::getWidth() const
{
	return width;
}

int PMultiImage::getHeight() const 
{
	return height;
}

int PMultiImage::getByteHeight() const 
{
	int numRowBytes = (int) ceil((double)height / 8.0);
	return numRowBytes;
}

// returns a new buffer of pixeldata, does not take ownership
const uint8_t* PMultiImage::getPixeldata(int image) const
{
	return data[image]->getRawDataPointer();
}


int PMultiImage::getNumImages() const
{
	return data.size();
}

bool PMultiImage::isComplete() const
{
	for (int i = 0; i < getNumImages(); i++)
	{
		if (data[i].get() == nullptr)
			return false;
	}
	return true;
}