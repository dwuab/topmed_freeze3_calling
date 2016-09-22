// ***************************************************************************
// Read.h - stores everything related to reads. (pre-alignment)
// ---------------------------------------------------------------------------
// (c) 2006 - 2009 Michael Str�mberg
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Dual licenced under the GNU General Public License 2.0+ license or as
// a commercial license with the Marth Lab.
// ***************************************************************************

#ifndef READ_H_
#define READ_H_

#include "MosaikString.h"

#define SOLID_PREFIX_LENGTH 1

namespace Mosaik {

	struct Mate {
		CMosaikString Bases;
		CMosaikString Qualities;
		char SolidPrefixTransition[SOLID_PREFIX_LENGTH];

		Mate()
		    : Bases()
		    , Qualities()
		{}
		bool clear(void) {
			Bases.clear();
			Qualities.clear();

			return true;
		}
	};

	struct Read {
		unsigned int ReadGroupCode;
		unsigned int Owner; // the temporary file that contains the read
		CMosaikString Name;
		Mate Mate1;
		Mate Mate2;

		Read()
		    : ReadGroupCode(0)
		    , Owner(0)
		    , Name()
		    , Mate1()
		    , Mate2()
		{}
		
		bool clear(void) {
			ReadGroupCode = 0;
			Owner         = 0;
			Name.clear();
			Mate1.clear();
			Mate2.clear();

			return true;
		}

		bool operator<(const Read& i) const{
			return Name < i.Name;
		}
	};
}

#endif // READ_H_
