// ***************************************************************************
// SequencingTechnologies.h - stores the internal codes relating to different
//                            sequencing technologies.
// ---------------------------------------------------------------------------
// (c) 2006 - 2009 Michael Str�mberg
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Dual licenced under the GNU General Public License 2.0+ license or as
// a commercial license with the Marth Lab.
// ***************************************************************************

#ifndef SEQUENCETECHNOLOGIES_H_
#define SEQUENCETECHNOLOGIES_H_

typedef unsigned short SequencingTechnologies;

// we have space for 9 additional sequencing technologies
#define ST_UNKNOWN               0
#define ST_454                   1
#define ST_HELICOS               2
#define ST_ILLUMINA              4
#define ST_PACIFIC_BIOSCIENCES   8
#define ST_SOLID                16
#define ST_SANGER               32
#define ST_ILLUMINA_LONG        64

#endif // SEQUENCETECHNOLOGIES_H_
