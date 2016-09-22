// ***************************************************************************
// ReferenceSequence.h - stores everything related to reference sequences.
// ---------------------------------------------------------------------------
// (c) 2006 - 2009 Michael Str�mberg
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Dual licenced under the GNU General Public License 2.0+ license or as
// a commercial license with the Marth Lab.
// ***************************************************************************

#ifndef _REFERENCESEQUENCE_H_
#define _REFERENCESEQUENCE_H_

#include <string>
#include "Mosaik.h"
#include "LargeFileSupport.h"

using namespace std;

struct ReferenceSequence {
	off_type BasesOffset;
	uint64_t NumAligned;
	uint64_t Begin;
	uint64_t End;
	uint64_t NumBases;
	string Name;
	string Bases;
	string GenomeAssemblyID;
	string Species;
	string MD5;
	string URI;
	bool   Special;

	// constructor
	ReferenceSequence()
		: BasesOffset(0)
		, NumAligned(0)
		, Begin(0)
		, End(0)
		, NumBases(0)
		, Name()
		, Bases()
		, GenomeAssemblyID()
		, Species()
		, MD5()
		, URI()
		, Special(false)
	{}
};


#endif
