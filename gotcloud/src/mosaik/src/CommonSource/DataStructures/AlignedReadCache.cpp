#include "AlignedReadCache.h"

CAlignedReadCache::CAlignedReadCache( unsigned int cacheSize )
    : _cache()
    , _currentIte()
    , _loadIte()
    , _cacheSize(cacheSize)
    , _currentNo(0)
    , _loadNo(0)
    , _full(false)
{
	_cache.resize( cacheSize );
        _currentIte = _cache.begin();
	_loadIte    = _cache.begin();
}

CAlignedReadCache::~CAlignedReadCache () {
	_cache.clear();
}

// add a new aligned read in _cache
bool CAlignedReadCache::Add ( const Mosaik::AlignedRead& ar ) {

	if ( _full ) return false;

	_currentIte->Clear();
	
	// copy 
	//_currentIte->ReadGroupCode    = ar.ReadGroupCode;
	//_currentIte->Name             = ar.Name;
	//_currentIte->Mate1Alignments  = ar.Mate1Alignments;
	//_currentIte->Mate2Alignments  = ar.Mate2Alignments;
	//_currentIte->IsLongRead       = ar.IsLongRead;
	//_currentIte->IsPairedEnd      = ar.IsPairedEnd;
	//_currentIte->IsResolvedAsPair = ar.IsResolvedAsPair;
	*_currentIte = ar;
	
	_currentIte++;
	_currentNo++;

	if ( _currentIte == _cache.end() )
		_full = true;
	
	return true;
}

bool CAlignedReadCache::LoadNextAlignedRead( Mosaik::AlignedRead& ar ) {
	
	if ( _currentNo == 0 ) return false;
	if ( _loadNo == _currentNo ) return false;
	if ( _loadIte == _cache.end() ) return false;
	

	//ar.ReadGroupCode = _loadIte->ReadGroupCode;
	//ar.Name = _loadIte->Name;
	//ar.Mate1Alignments  = _loadIte->Mate1Alignments;
	//ar.Mate2Alignments  = _loadIte->Mate2Alignments;
	//ar.IsLongRead       = _loadIte->IsLongRead;
	//ar.IsPairedEnd      = _loadIte->IsPairedEnd;
	//ar.IsResolvedAsPair = _loadIte->IsResolvedAsPair;
	ar = *_loadIte;

	_loadIte++;
	_loadNo++;

	return true;
}

// sort _cache by the position of the first mate in Mate1Alignments
void CAlignedReadCache::SortByPosition ( void ) {
	_cache.sort( SortBy1stMatePosition );
}

// sort _cache by read names
void CAlignedReadCache::SortByName ( void ) {
	_cache.sort( SortByReadName );
}


// reset _cache
bool CAlignedReadCache::Reset ( void ) {
	_currentIte = _cache.begin();
	_loadIte    = _cache.begin();
	_currentNo = 0;
	_loadNo    = 0;
	_full = false;

	for ( list<Mosaik::AlignedRead>::iterator ite = _cache.begin(); ite != _cache.end(); ite++ )
		ite->Clear();

	return true;
}

// return empty
bool CAlignedReadCache::isEmpty ( void ) {
	if ( _currentNo > 0 ) return false;
	return false;
}

// clear _cache
void CAlignedReadCache::Clear( void ) {
	_cache.clear();
	_cacheSize = 0;
	_currentNo = 0;
	_full = false;
}

inline bool CAlignedReadCache::SortBy1stMatePosition( const Mosaik::AlignedRead& ar1, const Mosaik::AlignedRead& ar2 ) {
	
	unsigned int nMate1Ar1 = ar1.Mate1Alignments.size();
	unsigned int nMate1Ar2 = ar2.Mate1Alignments.size();

	if ( nMate1Ar1 == 0 ) return false;
	if ( nMate1Ar2 == 0 ) return true;

	unsigned int refIndexAr1 = ar1.Mate1Alignments.begin()->ReferenceIndex;
	unsigned int refIndexAr2 = ar2.Mate1Alignments.begin()->ReferenceIndex;
	unsigned int refBeginAr1 = ar1.Mate1Alignments.begin()->ReferenceBegin;
	unsigned int refBeginAr2 = ar2.Mate1Alignments.begin()->ReferenceBegin;
	
	if ( refIndexAr1 == refIndexAr2 )
		return refBeginAr1 < refBeginAr2;
	
	return refIndexAr1 < refIndexAr2;
}

inline bool CAlignedReadCache::SortByReadName( const Mosaik::AlignedRead& ar1, const Mosaik::AlignedRead& ar2 ) {
	if ( ar1.Name.Length() == 0 ) return false;
	else if ( ar2.Name.Length() == 0 ) return true;

	return ar1.Name < ar2.Name;
}

