// ***************************************************************************
// CFastq - imports reads from the FASTQ file format.
// ---------------------------------------------------------------------------
// (c) 2006 - 2009 Michael Str�mberg
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Dual licenced under the GNU General Public License 2.0+ license or as
// a commercial license with the Marth Lab.
// ***************************************************************************

#pragma once

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <vector>
#include <zlib.h>
#include "FileUtilities.h"
#include "LargeFileSupport.h"
#include "Mosaik.h"
#include "MosaikString.h"
#include "MemoryUtilities.h"
#include "Read.h"
#include "SequenceUtilities.h"
#include "RegexUtilities.h"

using namespace std;

#define NORMAL_FASTQ_OFFSET      33
#define ILLUMINA_FASTQ_OFFSET    64

class CFastq {
public:
	// constructor
	CFastq(void);
	// destructor
	~CFastq(void);
	// checks to see if this is truly a FASTQ file
	static bool CheckFile(const string& filename, const bool showError);
	// closes the FASTQ file
	void Close(void);
	// loads the next read from the FASTQ file
	bool LoadNextMate(CMosaikString& readName, Mosaik::Mate& m);
	// opens the alignment archive
	void Open(const string& filename);
	// sets the file pointer to the beginning of the read data
	void Rewind(void);
	// sets the BQ offset
	void SetOffset(const unsigned char offset);
	// create a temporary FASTQ which is sorted by read names
	void SortByName(const string filename);

private:
	// denotes the status of the output stream
	bool mIsOpen;
	// denotes if the file is compressed
	bool mAreBasesCompressed;
	// denotes the FASTQ style
	bool mIsFastqStyleKnown;
	bool mUsingIlluminaStyle;
	unsigned char mFastqOffset;
	// our compressed output stream
	FILE* mInStream;
	gzFile mInZStream;
	// our input buffer
	char* mBuffer;
	unsigned int mBufferLen;
	// our input filename
	string mInputFilename;
	// stores the start of the read data (handles csfasta case)
	off_type mReadDataOffset;
	// our Illumina to Phred BQ LUT
	unsigned char mIlluminaToPhredLUT[256];
	// given a vector containing reads, find the min read and return the vector id
	unsigned int FindMinRead ( vector<Mosaik::Read>& tops );
	// print the record of a read to the given ofstream
	inline void PrintRead ( Mosaik::Read& read, ofstream& file );
	inline void PrintRead ( Mosaik::Read& read, FILE* file );
	inline void PrintRead ( Mosaik::Read& read, char*& buffer, char*& bufferPtr, unsigned int& bufferUsed, unsigned int& bufferLen );

	//CFastq (const CFastq&);
	//CFastq& operator=(const CFastq&);
};
