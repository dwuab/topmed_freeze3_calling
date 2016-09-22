#include <stdexcept>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>

#include "PileupElementBaseQual.h"

int PileupElementBaseQual::MAX_READS_PER_SITE = 10000;

PileupElementBaseQual::PileupElementBaseQual()
    : PileupElement(),
      myBases(NULL),
      myMapQualities(NULL),
      myQualities(NULL),
      myStrands(NULL),
      myCycles(NULL),
      myReadNameHashes(NULL),
      myGLScores(NULL),
      myAllocatedSize(0),
      myIndex(-1),
      myAddDelAsBase(false),
      myVcfOutFile(NULL),
      myLogGLMatrix(NULL)
{
    myAllocatedSize = 1024;
    myBases = (char*)malloc(myAllocatedSize + 1);
    myMapQualities = (int8_t*)malloc(myAllocatedSize * sizeof(int8_t));
    myQualities = (int8_t*)malloc(myAllocatedSize * sizeof(int8_t));
    myStrands = (char*)malloc(myAllocatedSize + 1);
    myCycles = (int8_t*)malloc(myAllocatedSize * sizeof(int8_t));
    myReadNameHashes = (uint32_t*)malloc(myAllocatedSize * sizeof(uint32_t));
    myGLScores = (int16_t*)malloc(myAllocatedSize * sizeof(int16_t));
    if((myBases == NULL ) 
       || (myMapQualities == NULL)
       || (myQualities == NULL)
       || (myStrands == NULL)
       || (myCycles == NULL)
       || (myReadNameHashes == NULL)
       || (myGLScores == NULL))
    {                     
        // TODO, check for malloc failures.
        std::cerr << "Failed Memory Allocation\n";
        abort();
    }
}

PileupElementBaseQual::PileupElementBaseQual(bool addDelAsBase)
    : PileupElement(),
      myBases(NULL),
      myMapQualities(NULL),
      myQualities(NULL),
      myStrands(NULL),
      myCycles(NULL),
      myGLScores(NULL),
      myAllocatedSize(0),
      myIndex(-1),
      myAddDelAsBase(addDelAsBase),
      myVcfOutFile(NULL)
{
    myAllocatedSize = 1024;
    myBases = (char*)malloc(myAllocatedSize + 1);
    myMapQualities = (int8_t*)malloc(myAllocatedSize * sizeof(int8_t));
    myQualities = (int8_t*)malloc(myAllocatedSize * sizeof(int8_t));
    myGLScores = (int16_t*)malloc(myAllocatedSize * sizeof(int16_t));
    myStrands = (char*)malloc(myAllocatedSize + 1);
    myCycles = (int8_t*)malloc(myAllocatedSize * sizeof(int8_t));
    myReadNameHashes = (uint32_t*)malloc(myAllocatedSize * sizeof(uint32_t));
    if((myBases == NULL ) 
       || (myMapQualities == NULL)
       || (myQualities == NULL)
       || (myStrands == NULL)
       || (myCycles == NULL)
       || (myReadNameHashes == NULL)
       || (myGLScores == NULL))
    {                     
        // TODO, check for malloc failures.
        std::cerr << "Failed Memory Allocation\n";
        abort();
    }                     
}  
                          
PileupElementBaseQual::PileupElementBaseQual(const PileupElementBaseQual& q)
    : PileupElement(),    
      myBases(NULL),      
      myMapQualities(NULL),
      myQualities(NULL), 
      myStrands(NULL),    
      myCycles(NULL),    
      myGLScores(NULL), 
      myAllocatedSize(0), 
      myIndex(-1),
      myVcfOutFile(NULL)
{                         
    myAllocatedSize = 1024;
    myBases = (char*)malloc(myAllocatedSize + 1);
    myMapQualities = (int8_t*)malloc(myAllocatedSize * sizeof(int8_t));
    myQualities = (int8_t*)malloc(myAllocatedSize * sizeof(int8_t));
    myStrands = (char*)malloc(myAllocatedSize + 1);
    myReadNameHashes = (uint32_t*)malloc(myAllocatedSize * sizeof(uint32_t));
    myCycles = (int8_t*)malloc(myAllocatedSize * sizeof(int8_t));
    myGLScores = (int16_t*)malloc(myAllocatedSize * sizeof(int16_t));
    myAddDelAsBase = q.myAddDelAsBase;
    if((myBases == NULL ) 
       || (myMapQualities == NULL)
       || (myQualities == NULL)
       || (myStrands == NULL)
       || (myCycles == NULL)
       || (myReadNameHashes == NULL)
       || (myGLScores == NULL))
    {
        // TODO, check for malloc failures.
        std::cerr << "Failed Memory Allocation\n";
        abort();
    }
}

PileupElementBaseQual::~PileupElementBaseQual()
{
    if(myBases != NULL)
    {
        free(myBases);
        myBases = NULL;
    }
    if(myMapQualities != NULL)
    {
        free(myMapQualities);
        myQualities = NULL;
    }
    if(myQualities != NULL)
    {
        free(myQualities);
        myQualities = NULL;
    }
    if(myStrands != NULL)
    {
        free(myStrands);
        myStrands = NULL;
    }
    if(myCycles != NULL)
    {
        free(myCycles);
        myCycles = NULL;
    }
    if(myReadNameHashes != NULL)
    {
        free(myReadNameHashes);
        myReadNameHashes = NULL;
    }
    if(myGLScores != NULL)
    {
        free(myGLScores);
        myGLScores = NULL;
    }
}

void PileupElementBaseQual::computeGLScores(int index, int16_t* GLScores, char* bases, int8_t* baseQualities)
{
    int base;
    double result;
    for (int genotype=0; genotype<=9; ++genotype)
    {
    	result = 0;
        for (int i=0; i<=myIndex; ++i)
    	{
            switch(bases[i])
            {
                case 'A':
                    base = 0;
                    break;
                case 'C':
                    base = 1;
                    break;
                case 'G':
                    base = 2;
                    break;
                case 'T':
                    base = 3;
                    break;
                default :
                    base = -1;
            }

            if(base!=-1)
            {
                result += myLogGLMatrix[baseQualities[i]][genotype][base];
            }
    	}

    	GLScores[genotype] = -10*result;
    }
}

const char* PileupElementBaseQual::getRefAllele() 
{ 
    if (myRefAllele.empty() ) {
        myRefAllele = ".";
    }
    return(myRefAllele.c_str()); 
}
    
// Add an entry to this pileup element.  
void PileupElementBaseQual::addEntry(SamRecord& record)
{
    // Call the base class:
    PileupElement::addEntry(record);

    // Only add if not to the max reads per site
    if ( myIndex+1 >= MAX_READS_PER_SITE ) return;

    // Increment the index
    ++myIndex;
    
    // if the index has gone beyond the allocated space, double the size.
    if(myIndex >= myAllocatedSize)
    {
        char* tempBuffer = (char*)realloc(myBases, myAllocatedSize * 2);
        if(tempBuffer == NULL)
        {
            std::cerr << "Memory Allocation Failure\n";
            abort();
            // TODO
            return;
        }
        myBases = tempBuffer;
        int8_t* tempInt8Buffer = (int8_t*)realloc(myMapQualities, myAllocatedSize * 2 * sizeof(int8_t));
        if(tempInt8Buffer == NULL)
        {
            std::cerr << "Memory Allocation Failure\n";
            abort();
            // TODO
            return;
        }
        myMapQualities = tempInt8Buffer; 
        tempInt8Buffer = (int8_t*)realloc(myQualities, myAllocatedSize * 2 * sizeof(int8_t));
        if(tempInt8Buffer == NULL)
        {
            std::cerr << "Memory Allocation Failure\n";
            abort();
            // TODO
            return;
        }
        myQualities = tempInt8Buffer;
        tempBuffer = (char*)realloc(myStrands, myAllocatedSize * 2);
        if(tempBuffer == NULL)
        {
            std::cerr << "Memory Allocation Failure\n";
            abort();
            // TODO
            return;
        }
        myStrands = tempBuffer;
        tempInt8Buffer = (int8_t*)realloc(myCycles, myAllocatedSize * 2 * sizeof(int8_t));
        if(tempInt8Buffer == NULL)
        {
            std::cerr << "Memory Allocation Failure\n";
            abort();
            // TODO
            return;
        }
        myCycles = tempInt8Buffer; 
        uint32_t* tempUint32Buffer = (uint32_t*)realloc(myReadNameHashes, myAllocatedSize * 2 * sizeof(uint32_t));
        if(tempUint32Buffer == NULL)
        {
            std::cerr << "Memory Allocation Failure\n";
            abort();
            // TODO
            return;
        }
        myReadNameHashes = tempUint32Buffer; 
        int16_t* tempInt16Buffer = (int16_t*)realloc(myGLScores, myAllocatedSize * 2 * sizeof(int16_t));
        if(tempInt8Buffer == NULL)
        {
            std::cerr << "Memory Allocation Failure\n";
            abort();
            // TODO
            return;
        }
        myGLScores = tempInt16Buffer;
        myAllocatedSize = myAllocatedSize * 2;
    }

    Cigar* cigar = record.getCigarInfo();
    
    if(cigar == NULL)
    {
        throw std::runtime_error("Failed to retrieve cigar info from the record.");
    }

    int32_t readIndex = 
        cigar->getQueryIndex(getRefPosition(), record.get0BasedPosition());

    // If the readPosition is N/A, this is a deletion.
    if(readIndex != CigarRoller::INDEX_NA)
    {
        char base = record.getSequence(readIndex);
        int8_t mapQual = record.getMapQuality();
        //-33 to obtain the PHRED base quality
        char qual = record.getQuality(readIndex) - 33;
        if(qual == UNSET_QUAL)
        {
            qual = ' ';
        }
        char strand = (record.getFlag() & 0x0010) ? 'R' : 'F';
        int cycle = strand == 'F' ? readIndex + 1 : record.getReadLength() -  readIndex;
        myBases[myIndex] = base;
        myMapQualities[myIndex] = mapQual;
        myQualities[myIndex] = qual;
        myStrands[myIndex] = strand;
        myCycles[myIndex] = cycle;
	myReadNameHashes[myIndex] = hash16(record.getReadName());
    }
    else if(myAddDelAsBase)
    {
        int8_t mapQual = record.getMapQuality();
        char strand = (record.getFlag() & 0x0010) ? 'R' : 'F';
        myBases[myIndex] = '-';
        myMapQualities[myIndex] = mapQual;
        myQualities[myIndex] = -1;
        myStrands[myIndex] = strand;
        myCycles[myIndex] = -1;
	myReadNameHashes[myIndex] = hash16(record.getReadName());
    }
    else
    {
        // Do not add a deletion.
        // Did not add any entries, so decrement the index counter since the
        // index was not used.
        --myIndex;
    }
}

void PileupElementBaseQual::analyze()
{
    if(getRefPosition() != UNSET_POSITION && myIndex != -1)
    {
    	char tempCStr[11];
    	std::string tempStr;
    	tempStr.append(getChromosome());
    	tempStr.append("\t");
    	sprintf(tempCStr, "%d", getRefPosition()+1 );
    	tempStr.append(tempCStr);
    	tempStr.append("\t.\t");
    	tempStr.append(getRefAllele());
    	tempStr.append("\t.\t.\t.\t.\t");

        tempStr.append("N:BASE:MAPQ:BASEQ:STRAND:CYCLE:RHASH:PL\t");
        
        sprintf(tempCStr, "%d", myIndex+1 );
    	tempStr.append(tempCStr);
    	tempStr.append(":");

        sprintf(tempCStr, "%c", myBases[0]);
        tempStr.append(tempCStr);
        for (int i=1; i<=myIndex; ++i)
        {
            sprintf(tempCStr, ",%c", myBases[i]);
            tempStr.append(tempCStr);
        }
        tempStr.append(":");	
		
        sprintf(tempCStr, "%d", myMapQualities[0]);
        tempStr.append(tempCStr);
        for (int i=1; i<=myIndex; ++i)
        {
            sprintf(tempCStr, ",%d", myMapQualities[i]);
            tempStr.append(tempCStr);
        }
        tempStr.append(":");

        sprintf(tempCStr, "%d", myQualities[0]);
        tempStr.append(tempCStr);    
        for (int i=1; i<=myIndex; ++i)
        {
            sprintf(tempCStr, ",%d", myQualities[i]);
            tempStr.append(tempCStr);
        }
        tempStr.append(":");
 
        sprintf(tempCStr, "%c", myStrands[0]);
        tempStr.append(tempCStr);
        for (int i=1; i<=myIndex; ++i)
        {
            sprintf(tempCStr, ",%c", myStrands[i]);
            tempStr.append(tempCStr);
        }
        tempStr.append(":");
 
	sprintf(tempCStr, "%d", myCycles[0]);
	tempStr.append(tempCStr); 
        for (int i=1; i<=myIndex; ++i)
        {
            sprintf(tempCStr, ",%d", myCycles[i]);
            tempStr.append(tempCStr);        	
        }
        tempStr.append(":");

	sprintf(tempCStr, "%d", myReadNameHashes[0]);
	tempStr.append(tempCStr); 
        for (int i=1; i<=myIndex; ++i)
        {
            sprintf(tempCStr, ",%d", myReadNameHashes[i]);
            tempStr.append(tempCStr);        	
        }
        tempStr.append(":");

        computeGLScores(myIndex, myGLScores, myBases, myQualities);

        sprintf(tempCStr, "%d", myGLScores[0]);
        tempStr.append(tempCStr);    
        for (int i=1; i<=9; ++i)
        {
            sprintf(tempCStr, ",%d", myGLScores[i]);
            tempStr.append(tempCStr);
        }
        
        tempStr.append("\n");  	
       	myVcfOutFile->ifwrite(tempStr.c_str(), tempStr.length());
    }
    
    //to ensure this does not print when reflushed
    myIndex=-1;
}


void PileupElementBaseQual::reset(int refPosition)
{	
    // Call the base class.      
    PileupElement::reset(refPosition);

    myIndex = -1;
}


void PileupElementBaseQual::reset(int refPosition, InputFile* vcfOutFile, bool addDelAsBase, double*** logGLMatrix)
{
    // Assign pointer to myLogGLMatrix
    if (myLogGLMatrix == NULL)
    {
        myLogGLMatrix = logGLMatrix;
    }
		
    // Assign pointer to myVcfOutFile
    if (myVcfOutFile == NULL)
    {
        myVcfOutFile = vcfOutFile;
    }
	
    myAddDelAsBase = addDelAsBase;	
    myRefAllele.clear();
	
    // Call the base class.      
    PileupElement::reset(refPosition);

    myIndex = -1;
}
