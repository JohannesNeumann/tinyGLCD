/*
  ==============================================================================

    Parser.cpp
    Created: 26 Mar 2015 8:48:30am
    Author:  Johannes Neumann

  ==============================================================================
*/

#include "Parser.h"
#include "errorlog.h"
#include <math.h>

const char* ASCIIChars[128] = {
	"NULL", 
	"SOH", 
	"STX", 
	"ETX", 
	"EOT", 
	"ENQ", 
	"ACK", 
	"BELL", 
	"backspace", 
	"tab", 
	"LF", 
	"vertical tab", 
	"form feed / new page", 
	"CR",
	"shift out", 
	"shift in", 
	"DLE", 
	"DC1", 
	"DC2", 
	"DC3", 
	"DC4", 
	"NAK", 
	"SYN", 
	"ETB", 
	"CAN", 
	"EM", 
	"SUB", 
	"ESC", 
	"FS", 
	"GS", 
	"RS", 
	"US", 
	"Space", 
	"!", 
	"\"", 
	"#", 
	"$", 
	"%", 
	"&", 
	"'", 
	"(", 
	")", 
	"*", 
	"+", 
	",", 
	"-", 
	".", 
	"/", 
	"0", 
	"1", 
	"2", 
	"3", 
	"4", 
	"5", 
	"6", 
	"7", 
	"8", 
	"9", 
	":", 
	";", 
	"<", 
	"=", 
	">", 
	"?", 
	"@",
	"A", 
	"B", 
	"C", 
	"D", 
	"E", 
	"F", 
	"G", 
	"H", 
	"I",  
	"J", 
	"K", 
	"L", 
	"M", 
	"N", 
	"O", 
	"P", 
	"Q", 
	"R", 
	"S", 
	"T", 
	"U", 
	"V", 
	"W", 
	"X", 
	"Y", 
	"Z", 
	"[", 
	"\\", 
	"]", 
	"^", 
	"_", 
	"`", 
	"a", 
	"b",
	"c", 
	"d", 
	"e", 
	"f", 
	"g", 
	"h", 
	"i", 
	"j", 
	"k", 
	"l", 
	"m", 
	"n", 
	"o", 
	"p", 
	"q", 
	"r", 
	"s", 
	"t", 
	"u", 
	"v", 
	"w", 
	"x", 
	"y", 
	"z", 
	"{", 
	"|", 
	"}", 
	"~", 
	"DEL"
};


Parser::Parser()
: parsingThread(this)
{

}

Parser::~Parser()
{

}

void Parser::parseFolder (File inputFolder, File outputFolder)
{
	images.clear();
	fonts.clear();
	parsingThread.setPriority(1);
	parsingThread.startParsing(inputFolder, outputFolder);
}

Parser::ParsingThread::ParsingThread(Parser* parent)
	: Thread("ImageParser_parsingThread")
{
	p = parent;
}

void Parser::ParsingThread::startParsing(File input, File output)
{
	inputFolder = input;
	outputFolder = output;
	startThread(4);
}

void Parser::ParsingThread::run()
{
	ErrorLog::newLog(outputFolder.getParentDirectory().getChildFile("log.txt"));

	ErrorLog::addLine("Scanning input folder for '*.gif' and 'Multi_[width]_*.gif' image files: '" + inputFolder.getFullPathName() + "'");

	if (!inputFolder.exists())
	{
		ErrorLog::addError("Input folder does not exist.");
		return;
	}

	if (!inputFolder.isDirectory())
	{
		ErrorLog::addError("Input folder is not a directory.");
		return;
	}

	DirectoryIterator it(inputFolder, false, "*.gif", File::findFiles);

	int totalNumFiles = 0;
	while (it.next())
	{
		if (threadShouldExit())
		{
			ErrorLog::addError("Parsing aborted. ");
			return;
		}

		File f = it.getFile();
		totalNumFiles++;


		String resourceName;
		if (f.getFileNameWithoutExtension().startsWith("Multi_"))
		{
			String s = f.getFileNameWithoutExtension().substring(6);

			String widthString = s.upToFirstOccurrenceOf("_", false, true);
			if (widthString == s)
			{
				ErrorLog::addError(">> Cannot parse '" + f.getFullPathName() + "': This file is a multi-image but does not follow the naming convention of 'Multi_Width_name.*'");
				continue;
			}
			int width = widthString.getIntValue();
			resourceName = s.fromFirstOccurrenceOf("_", false, true);
			if (resourceName == s)
			{
				ErrorLog::addError(">> Cannot parse '" + f.getFullPathName() + "': This file is a multi-image but does not follow the naming convention of 'Multi_WWxHH_name.*'");
				continue;
			}

			ErrorLog::addLine(">> '" + f.getFullPathName() + "' (= M[" + widthString + "])");
			p->images[resourceName].loadFromMultiFile(f, width);
		}
		else
		{
			resourceName = f.getFileNameWithoutExtension().trimCharactersAtEnd("0123456789");
			ErrorLog::addLine(">> '" + f.getFullPathName() + "'");
			p->images[resourceName].addSingleFromFile(f);
		}
	}

	ErrorLog::addLine("All image files scanned. Scanning font characters in sub folder 'fonts/' ");

	DirectoryIterator it2(inputFolder.getChildFile("fonts"), false, "*.gif", File::findFiles);

	while (it2.next())
	{
		if (threadShouldExit())
		{
			ErrorLog::addError("Parsing aborted. ");
			return;
		}

		File f = it2.getFile();
		totalNumFiles++;

		ErrorLog::addLine(">> '" + f.getFullPathName() + "'");

		String fontName = f.getFileNameWithoutExtension().trimCharactersAtEnd("_0123456789");
		p->fonts[fontName].addFromFile(f);
	}

	ErrorLog::addLine("All files scanned. Assembling images resource file. ");

	String imagesc = "";

	// holds image definitions to be inserted into images.h file
	String imageDefinitions = "";

	std::map<String, PMultiImage>::iterator i;
	for (i = p->images.begin(); i != p->images.end(); i++)
	{
		if (!i->second.isComplete())
		{
			ErrorLog::addError("Resource '" + i->first + "' is incomplete. Skipping.");
			continue;
		}

		if (i->second.getNumImages() == 0)
		{
			ErrorLog::addError("Resource '" + i->first + "' has no image data. Weird! Skipping.");
			continue;
		}
		// this contains just a single image. 
		else if (i->second.getNumImages() == 1)
		{
			ErrorLog::addLine("<< '" + i->first + "'");

			imagesc << "// image resource: '" << i->first << "'" << newLine;
			imagesc << Parser::createArray(i->first + "_pxdata", 
									   0, 
									   i->second.getPixeldata(0), 
									   i->second.getNumBytesInPixeldata(),
									   72
									   );
			imagesc << newLine;
			imagesc << "const image_t " << i->first << " = { " << newLine;
			imagesc << "    " << String(i->second.getWidth()) << ", " << String(i->second.getHeight()) << ", " << String(i->second.getByteHeight()) << ", "<< newLine;
			imagesc << "    " << i->first + "_pxdata };" << newLine << newLine;

			imageDefinitions += "extern const image_t " + i->first + ";"  + newLine;
		}
		// assemble a multi-image file
		else
		{
			ErrorLog::addLine("<< '" + i->first + "'");

			imagesc << "// multi image resource: '" << i->first << "'" << newLine;


			String pixeldatas = "";
			for (int img = 0; img < i->second.getNumImages(); img++)
			{
				// write pixeldata to the file
				imagesc << Parser::createArray(i->first + "_pxdata" + String(img), 
										   0, 
										   i->second.getPixeldata(img), 
										   i->second.getNumBytesInPixeldata(),
										   72
										   );
				imagesc << newLine;

				// append pointer to this string for assembly of the resource array
				pixeldatas += i->first + "_pxdata" + String(img);
				// last one? 
				if (img != i->second.getNumImages() -1)
					pixeldatas += String(", ") + newLine + "    ";
			}
		
			imagesc << "const uint8_t* " << i->first + "_pxdataPtrs" << "[" << i->second.getNumImages() << "] = {" << newLine;
			imagesc << "    " << pixeldatas << " };" << newLine;

			imagesc << "const multiImage_t " << i->first << " = { " << newLine;
			imagesc << "    " << String(i->second.getWidth()) << ", " << String(i->second.getHeight())<< ", " << String(i->second.getByteHeight()) << ", ";
			imagesc << String(i->second.getNumImages()) << ", " << newLine;
			imagesc << "    " << i->first + "_pxdataPtrs"  << " };" << newLine << newLine;

			imageDefinitions += "extern const multiImage_t " + i->first + ";"  + newLine;
		}
	}
	
	String c = outputFolder.getChildFile("images.c").loadFileAsString();
	int sectionStart = c.lastIndexOf("// START__");
	int sectionEnd = c.lastIndexOf("// END__");

	if (sectionStart > sectionEnd)
	{
		ErrorLog::addError("Can't insert definitions into images.c because the '// START__' and '// END__' operators were in the wrong order!");
	}
	else if ((sectionStart <= 0) || (sectionEnd <= 0))
	{
		ErrorLog::addError("Can't insert definitions into images.c because the '// START__' or '// END__' operators were not found!");
	}
	else 
	{
		outputFolder.getChildFile("images.c").deleteFile();
		juce::FileOutputStream outc (outputFolder.getChildFile("images.c"));

		int startIndex = sectionStart + String("// START__").length();
		outc << c.replaceSection(startIndex, sectionEnd - startIndex,  newLine.operator String() + imagesc);
	}

	//========================================================================================================
	//========================================================================================================
	ErrorLog::addLine("Including definitions to images.h header file ...");

	
	String h = outputFolder.getChildFile("images.h").loadFileAsString();
	sectionStart = h.lastIndexOf("// START__");
	sectionEnd = h.lastIndexOf("// END__");

	if (sectionStart > sectionEnd)
	{
		ErrorLog::addError("Can't insert definitions into images.h because the '// START__' and '// END__' operators were in the wrong order!");
	}
	else if ((sectionStart <= 0) || (sectionEnd <= 0))
	{
		ErrorLog::addError("Can't insert definitions into images.h because the '// START__' or '// END__' operators were not found!");
	}
	else 
	{
		outputFolder.getChildFile("images.h").deleteFile();
		juce::FileOutputStream outh (outputFolder.getChildFile("images.h"));

		int startIndex = sectionStart + String("// START__").length();
		outh << h.replaceSection(startIndex, sectionEnd - startIndex,  newLine.operator String() + imageDefinitions);
	}

	//========================================================================================================
	//========================================================================================================

	ErrorLog::addLine("Assembling font resource file fonts.c ...");
	
	String fontsc = "";
	
	// add empty symbol
	fontsc << "// empty character..." << newLine;
	fontsc << "const character_t emptyChar = { 0, NULL };" << newLine;
	fontsc << newLine << newLine;

	String fontDefinitions = "";

	std::map<String, PFont>::iterator j;
	for (j = p->fonts.begin(); j != p->fonts.end(); j++)
	{
		if (j->second.getNumCharacters() == 0)
		{
			ErrorLog::addError("Font '" + j->first + "' has no character data. Weird! Skipping.");
			continue;
		}

		// assemble the font images file
		else
		{
			ErrorLog::addLine("<< '" + j->first + "'");

			fontsc << "// font resource: '" << j->first << "'" << newLine;

			for (int c = 0; c < j->second.getNumCharacters(); c++)
			{
				if (j->second.getPixeldata(c) == nullptr)
					continue;

				// write pixeldata to the file
				fontsc << Parser::createArray(j->first + "_char" + String(c) + "_pxdata", 
										    0, 
										    j->second.getPixeldata(c), 
										    j->second.getNumBytesInPixeldata(c),
										    72
										    );
				fontsc << newLine;
				fontsc << "const character_t " << j->first + "_char" + String(c) << " = { ";
				fontsc << String(j->second.getWidth(c)) <<  ", ";
				fontsc << " " << j->first + "_char" + String(c)  + "_pxdata" << " };" << newLine;
			}

		
			fontsc << "const font_t " << j->first << " = { " << newLine;
			fontsc << "    " << String(j->second.getHeight()) << ", " << String(j->second.getByteHeight()) << ", {" << newLine;
			for (int c = 0; c < 128; c++)
			{
				if (j->second.getPixeldata(c) == nullptr)
					fontsc << "   &emptyChar,";
				else
					fontsc << "   &" << j->first + "_char" + String(c) + ",";


				fontsc << "   // Character " + String(ASCIIChars[c]);

				if (c != 127)
					fontsc << ", " << newLine;
			}

			fontsc << newLine << "   }" << newLine << "};" << newLine << newLine << newLine;

			fontDefinitions += "extern const font_t " + j->first + ";" + newLine;
		}
	}
	
	String d = outputFolder.getChildFile("fonts.c").loadFileAsString();
	sectionStart = d.lastIndexOf("// START__");
	sectionEnd = d.lastIndexOf("// END__");

	if (sectionStart > sectionEnd)
	{
		ErrorLog::addError("Can't insert definitions into fonts.c because the '// START__' and '// END__' operators were in the wrong order!");
	}
	else if ((sectionStart <= 0) || (sectionEnd <= 0))
	{
		ErrorLog::addError("Can't insert definitions into fonts.c because the '// START__' or '// END__' operators were not found!");
	}
	else 
	{
		outputFolder.getChildFile("fonts.c").deleteFile();
		juce::FileOutputStream outc (outputFolder.getChildFile("fonts.c"));

		int startIndex = sectionStart + String("// START__").length();
		outc << d.replaceSection(startIndex, sectionEnd - startIndex,  newLine.operator String() + fontsc);
	}

	//========================================================================================================
	//========================================================================================================
	ErrorLog::addLine("Including definitions to fonts.h header file ...");

	
	String fh = outputFolder.getChildFile("fonts.h").loadFileAsString();
	int fsectionStart = fh.lastIndexOf("// START__");
	int fsectionEnd = fh.lastIndexOf("// END__");

	if (fsectionStart > fsectionEnd)
	{
		ErrorLog::addError("Can't insert definitions into fonts.h because the '// START__' and '// END__' operators were in the wrong order!");
	}
	else if ((fsectionStart <= 0) || (fsectionEnd <= 0))
	{
		ErrorLog::addError("Can't insert definitions into fonts.h because the '// START__' or '// END__' operators were not found!");
	}
	else 
	{
		outputFolder.getChildFile("fonts.h").deleteFile();
		juce::FileOutputStream outfh (outputFolder.getChildFile("fonts.h"));

		int fstartIndex = fsectionStart + String("// START__").length();
		outfh << fh.replaceSection(fstartIndex, fsectionEnd - fstartIndex,  newLine.operator String() + fontDefinitions);
	}
}

String Parser::createArray(String name, int numTabsIn, const uint8_t* data, int numElements, int maxWidthInCharacters)
{
	int numCharsPerElement = String("0xAA, ").length();
	const String tab = "    ";
	
	int numElementsPerLine = (maxWidthInCharacters - tab.length()*(numTabsIn + 1)) / numCharsPerElement;
	int numElementsInCurrentLine = 0;

	String result;
	// initial tabs
	for (int j = 0; j < numTabsIn; j++)
				result += tab;
	result += "const uint8_t " + name + "[" + String(numElements) + "] = {" + newLine;

	// tabs before first value
	for (int j = 0; j < (numTabsIn+1); j++)
				result += tab;

	// add elements
	for (int i = 0; i < numElements; i++)
	{
		if (numElementsInCurrentLine >= numElementsPerLine)
		{
			numElementsInCurrentLine = 0;
			result += newLine;
			for (int j = 0; j < (numTabsIn+1); j++)
				result += tab;
		}

		result += "0x" + String::toHexString(&data[i], 1, 1);
		numElementsInCurrentLine++;

		// last Element
		if (i == numElements-1)
		{
			result += " };";
		} 
		else
		{
			result += ", ";
		}
	}

	return result;
}

void Parser::addListener(Parser::Listener* l)
{
	if (!listeners.contains(l))
		listeners.add(l);
}

void Parser::removeListener(Parser::Listener* l)
{
	if (listeners.contains(l))
		listeners.remove(l);
}
