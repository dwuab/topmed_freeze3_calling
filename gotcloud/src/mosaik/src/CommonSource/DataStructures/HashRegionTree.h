// ***************************************************************************
// CHashRegionTree - aggregates hash positions and forms alignment candidates.
// ---------------------------------------------------------------------------
// (c) 2006 - 2009 Michael Str�mberg
// Marth Lab, Department of Biology, Boston College
// ---------------------------------------------------------------------------
// Dual licenced under the GNU General Public License 2.0+ license or as
// a commercial license with the Marth Lab.
// ***************************************************************************

#ifndef HASHREGIONTREE_H_
#define HASHREGIONTREE_H_

#include <iostream>
#include "HashRegion.h"

using namespace std;

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

#define HASH_SIZE 10

namespace AVLTree {

	struct HashRegionAvlNode {
		HashRegion Data;
		HashRegionAvlNode* Left;
		HashRegionAvlNode* Right;
		HashRegionAvlNode* Parent;
		int Balance;

		HashRegionAvlNode(HashRegion& data, HashRegionAvlNode* parent)
		: Data(data)
		, Left(NULL)
		, Right(NULL)
		, Parent(parent)
		, Balance(0)
		{}
	};

	class CHashRegionTree {
	public:
		CHashRegionTree(unsigned int queryLen, unsigned char hashSize);
		~CHashRegionTree();
		// clears the tree
		void Clear();
		// dumps the tree in sorted order
		void DumpTree();
		// go to the first entry
		void GotoFirstEntry();
		// go to the last entry
		void GotoLastEntry();
		// moves on to the next element in the tree
		bool GetNextEntry();
		// moves on to the previous element in the tree
		bool GetPreviousEntry(HashRegion& key);
		// insert into the tree; duplicates are ignored
		bool Insert(HashRegion& key);
		// returns the current size of the tree
		unsigned int GetCount();
		// gets the current hash region at the traversal pointer
		HashRegion* GetTraversalHashRegion();
		// sets the expected query length
		void SetExpectedQueryLength(unsigned int queryLen);
	private:
		// our tree root
		HashRegionAvlNode* mRoot;
		// our traversal pointer root
		HashRegionAvlNode* mTraverse;
		// our first entry
		//HashRegionAvlNode* mFirst;
		// our last entry
		//HashRegionAvlNode* mLast;
		// stores the expected query length
		unsigned int mQueryLength;
		// stores the current hash size
		unsigned char mHashSize;
		// stores the maximum expected indel size
		unsigned char mIndelSize;
		// the current element count
		unsigned int mCount;
		// rotates a given node left
		void LeftRotate(HashRegionAvlNode* n);
		// rotates a given node right
		void RightRotate(HashRegionAvlNode* n);
		// moves on to the previous element in the tree
		void MoveToPreviousEntry();

		CHashRegionTree (const AVLTree::CHashRegionTree&);
		CHashRegionTree& operator=(const AVLTree::CHashRegionTree&);
	};
}

#endif // HASHREGIONTREE_H_
