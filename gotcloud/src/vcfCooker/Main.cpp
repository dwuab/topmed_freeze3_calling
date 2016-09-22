////////////////////////////////////////////////////////////////////// 
// vcfCooker/Main.cpp 
// (c) 2010-2014 Hyun Min Kang, Matthew Flickenger, Matthew Snyder, Paul Anderson
//          Tom Blackwell, Mary Kate Wing, and Goncalo Abecasis
// 
// This file is distributed as part of the vcfCooker source code package   
// and may not be redistributed in any form, without prior written    
// permission from the author. Permission is granted for you to       
// modify this file for your own personal use, but modified versions  
// must retain this copyright notice and must not be distributed.     
// 
// Permission is granted for you to use this file to compile vcfCooker
// 
// All computer programs have bugs. Use this file at your own risk.   
// 
// Thursday November 11th, 2010

#include <map>
#include <limits.h>
#include <vector>

#include "Parameters.h"
#include "InputFile.h"
#include "Error.h"
#include "libVcfVcfFile.h"
#include "Logger.h"
using namespace libVcf;

Logger* Logger::gLogger = NULL;

int main(int argc, char ** argv)
{
   printf("vcfCooker 1.1.1 -- Manipulate VCF files\n"
          "(c) 2010-2014 Hyun Min Kang, Matthew Flickinger, Matthew Snyder, Paul Anderson, Tom Blackwell, Mary Kate Wing, and Goncalo Abecasis\n\n");

   bool bRecipesWriteBed = false;
   bool bRecipesWriteVcf = false;
   bool bRecipesSummarize = false;
   bool bRecipesUpgrade = false; // upgrade from glfMultiples v3.3 to v4.0 format
   //bool bRecipesMerge = false;
   bool bRecipesFilter = false;
   //bool bRecipesProfile = false;
   bool bRecipesSubset = false;

   bool bFiltOnlySubset = false;
   bool bMonoSubset = false;

   String sInputVcf, sInputBfile, sInputBed, sInputBim, sInputFam, sInputSubset;
   String sFasta("/data/local/ref/karma.ref/human.g1k.v37.fa");

   String sOut("./vcfCooker");

   int nMinGQ = 0;
   int nMinGD = 0;
   int qGeno = 0;
   int nPrintUnit = 10000;

   //bool bMergeGeno = false;
   //int nMergeWay = 0;

   //bool bResetFilter = false;
   int nMinQUAL = 0;
   int nMinMQ = 0;
   int nMaxDP = INT_MAX;
   int nMinDP = 0;
   int nMaxAB = 100;
   int nWinFFRQ = 0;
   int nMaxFFRQ = 0;
   double fMaxFFRQ = 0;
   int nWinFVAR = 0;
   int nMerFVAR = 0;
   int nMaxFVAR = 0;
   int nMinNS = 0;
   int nWinIndel = 0;
   int nMaxSTP = INT_MAX;
   int nMaxTTT = INT_MAX;
   int nMinTTT = INT_MIN;
   int nMaxSTR = 100;
   int nMinSTR = -100;
   int nMaxSTZ = INT_MAX;
   int nMinSTZ = INT_MIN;
   int nMaxLQZ = INT_MAX;
   int nMinLQZ = INT_MIN;
   int nMaxRBZ = INT_MAX;
   int nMinRBZ = INT_MIN;
   int nMaxCBR = 100;
   int nMinCBR = -100;
   int nMaxQBR = 100;
   int nMinQBR = -100;
   int nMaxCBZ = INT_MAX;
   int nMaxCSR = 100;
   int nMinCSR = -100;
   int nMaxIOZ = INT_MAX;
   int nMinIOR = INT_MIN;
   int nMaxIOR = INT_MAX;
   int nMaxAOZ = INT_MAX;
   int nMaxAOI = INT_MAX;
   int nMaxMQ0 = 100;
   int nMaxMQ10 = 100;
   int nMaxMQ20 = 100;
   int nMinFIC = INT_MIN;
   int nMinABE = -100;
   int nMaxABE = 100;
   int nMinMBR = -100;
   int nMaxMBR = 100;
   int nMaxLQR = 100;
   int nMinABZ = INT_MIN;
   int nMaxABZ = INT_MAX;
   int nMaxBCS = INT_MAX;

   String sIndelVcf;

   bool bVerbose = false;
   bool bOutPlain = true;
   bool bOutBgzf = false;
   bool bOutGzip = false;
   bool bKeepFilter = false;

   ParameterList pl;

   BEGIN_LONG_PARAMETERS(longParameters)
     LONG_PARAMETER_GROUP("Recipes")
     LONG_PARAMETER("write-bed",&bRecipesWriteBed)
     LONG_PARAMETER("write-vcf",&bRecipesWriteVcf)
     LONG_PARAMETER("upgrade",&bRecipesUpgrade)
     LONG_PARAMETER("summarize",&bRecipesSummarize)
     //LONG_PARAMETER("merge",&bRecipesMerge)
     LONG_PARAMETER("filter",&bRecipesFilter)
     LONG_PARAMETER("subset",&bRecipesSubset)
     //LONG_PARAMETER("profile",&bRecipesProfile)

     LONG_PARAMETER_GROUP("VCF Input options")
     LONG_STRINGPARAMETER("in-vcf",&sInputVcf)

     LONG_PARAMETER_GROUP("BED Input options")
     LONG_STRINGPARAMETER("in-bfile",&sInputBfile)
     LONG_STRINGPARAMETER("in-bed",&sInputBed)
     LONG_STRINGPARAMETER("in-bim",&sInputBim)
     LONG_STRINGPARAMETER("in-fam",&sInputFam)
     LONG_STRINGPARAMETER("ref",&sFasta)

     LONG_PARAMETER_GROUP("Subsetting options")
     LONG_STRINGPARAMETER("in-subset",&sInputSubset)
     LONG_PARAMETER("mono-subset",&bMonoSubset)
     LONG_PARAMETER("filt-only-subset",&bFiltOnlySubset)

     LONG_PARAMETER_GROUP("Output Options")
     LONG_STRINGPARAMETER("out",&sOut)
     LONG_INTPARAMETER("qGeno",&qGeno)
     LONG_INTPARAMETER("print-every",&nPrintUnit)

     LONG_PARAMETER_GROUP("Output compression Options")
     EXCLUSIVE_PARAMETER("plain",&bOutPlain)
     EXCLUSIVE_PARAMETER("bgzf",&bOutBgzf)
     EXCLUSIVE_PARAMETER("gzip",&bOutGzip)

     LONG_PARAMETER_GROUP("Genotype-level Filter Options")
     LONG_INTPARAMETER("minGQ",&nMinGQ)
     LONG_INTPARAMETER("minGD",&nMinGD)

     //LONG_PARAMETER_GROUP("Merge Options")
     //LONG_PARAMETER("merge-geno",&bMergeGeno)
     //LONG_INTPARAMETER("merge-nway",&nMergeWay)

     LONG_PARAMETER_GROUP("Filter Options")
     LONG_INTPARAMETER("winIndel",&nWinIndel)
     LONG_STRINGPARAMETER("indelVCF",&sIndelVcf)
     //LONG_PARAMETER("reset-filter",&bResetFilter)
     LONG_INTPARAMETER("minQUAL",&nMinQUAL)
     LONG_INTPARAMETER("minMQ",&nMinMQ)
     LONG_INTPARAMETER("maxDP",&nMaxDP)
     LONG_INTPARAMETER("minDP",&nMinDP)
     LONG_INTPARAMETER("maxABL",&nMaxAB)
     LONG_INTPARAMETER("winFFRQ",&nWinFFRQ)
     LONG_INTPARAMETER("maxFFRQ",&nMaxFFRQ)
     LONG_INTPARAMETER("winFVAR",&nWinFVAR)
     LONG_INTPARAMETER("merFVAR",&nMerFVAR)
     LONG_INTPARAMETER("maxFVAR",&nMaxFVAR)
     LONG_INTPARAMETER("minNS",&nMinNS)
     LONG_INTPARAMETER("maxSTP",&nMaxSTP)
     LONG_INTPARAMETER("maxTTT",&nMaxTTT)
     LONG_INTPARAMETER("minTTT",&nMinTTT)
     LONG_INTPARAMETER("maxSTR",&nMaxSTR)
     LONG_INTPARAMETER("minSTR",&nMinSTR)
     LONG_INTPARAMETER("maxSTZ",&nMaxSTZ)
     LONG_INTPARAMETER("minSTZ",&nMinSTZ)
     LONG_INTPARAMETER("maxCBR",&nMaxCBR)
     LONG_INTPARAMETER("minCBR",&nMinCBR)
     LONG_INTPARAMETER("maxQBR",&nMaxQBR)
     LONG_INTPARAMETER("minQBR",&nMinQBR)
     LONG_INTPARAMETER("maxCBZ",&nMaxCBZ)
     LONG_INTPARAMETER("maxCSR",&nMaxCSR)
     LONG_INTPARAMETER("minCSR",&nMinCSR)
     LONG_INTPARAMETER("maxLQZ",&nMaxLQZ)
     LONG_INTPARAMETER("minLQZ",&nMinLQZ)
     LONG_INTPARAMETER("maxRBZ",&nMaxRBZ)
     LONG_INTPARAMETER("minRBZ",&nMinRBZ)
     LONG_INTPARAMETER("maxIOZ",&nMaxIOZ)
     LONG_INTPARAMETER("minIOR",&nMinIOR)
     LONG_INTPARAMETER("maxIOR",&nMaxIOR)
     LONG_INTPARAMETER("maxAOZ",&nMaxAOZ)
     LONG_INTPARAMETER("maxAOI",&nMaxAOI)
     LONG_INTPARAMETER("maxMQ0",&nMaxMQ0)
     LONG_INTPARAMETER("maxMQ10",&nMaxMQ10)
     LONG_INTPARAMETER("maxMQ20",&nMaxMQ20)
     LONG_INTPARAMETER("minFIC",&nMinFIC)
     LONG_INTPARAMETER("minABE",&nMinABE)
     LONG_INTPARAMETER("maxABE",&nMaxABE)
     LONG_INTPARAMETER("maxLQR",&nMaxLQR)
     LONG_INTPARAMETER("minMBR",&nMinMBR)
     LONG_INTPARAMETER("maxMBR",&nMaxMBR)
     LONG_INTPARAMETER("minABZ",&nMinABZ)
     LONG_INTPARAMETER("maxABZ",&nMaxABZ)
     LONG_INTPARAMETER("maxBCS",&nMaxBCS)
     LONG_PARAMETER("keepFilter",&bKeepFilter)
   END_LONG_PARAMETERS();

   pl.Add(new LongParameters("Available Options", longParameters));
   pl.Read(argc, argv);
   pl.Status();
   
   // create objects for logging
   if ( sOut.IsEmpty() ) {
     fprintf(stderr,"ERROR: output prefix is empty");
     abort();
   }
   Logger::gLogger = new Logger((sOut+".log").c_str(), bVerbose);

   time_t t;
   time(&t);
   Logger::gLogger->writeLog("Analysis started on %s", ctime(&t));

   ////////////////////////////////////////////////////////////
   // check the compatibility of arguments
   ///////////////////////////////////////////////////////////
   // Check the sanity of input file arguments
   ///////////////////////////////////////////////////////////
   bool bVCF = true;
   if ( sInputVcf.IsEmpty() ) {  // No VCF, BED mode should be enabled
     if ( ! sInputBfile.IsEmpty() )  { // --bfile is given
       if ( sInputBim.IsEmpty() && sInputFam.IsEmpty() && sInputBed.IsEmpty() ) {
	 sInputBim = sInputBfile + ".bim";
	 sInputBed = sInputBfile + ".bed";
	 sInputFam = sInputBfile + ".fam";
	 bVCF = false;
       }
       else {
	 Logger::gLogger->error("when --in-bfile is option given, --in-bed, --in-bim, --in-fam cannot be combined together");
       }
     }
     else if ( sInputBim.IsEmpty() || sInputFam.IsEmpty() || sInputBed.IsEmpty() ) {
       // if any of --bim, --fam, --bed is missing report an error
       Logger::gLogger->error("--in-vcf, --in-bfile or all three of --in-bed, --in-bim, --in-fam arguments must be present");
     }
     else {
       // fine. go ahead
       bVCF = false;
     }
   }
   else {  // VCF mode
     if ( sInputBfile.IsEmpty() && sInputBim.IsEmpty() && sInputFam.IsEmpty() && sInputBed.IsEmpty() ) {
	 // if no BED argument was given, fine -- proceed
       bVCF = true;
     }
     else {
       // if BED argument was given, report error
       Logger::gLogger->error("when --in-vcf is option given, other BED format related options cannot be combined");
     }
   }

   if ( bRecipesFilter ) {
     if ( !bVCF ) {
       Logger::gLogger->error("--filter recipes are compatible only with VCF input");
     }
   }

   if ( bRecipesSubset ) {
     if ( sInputSubset.IsEmpty() ) {
       Logger::gLogger->error("--in-subset option is required for --subset");
     }
   }

   try {

     std::map<uint64_t,int> freqLeft;
     std::map<uint64_t,int> freqRight;
     std::vector<uint64_t> leftKeys;
     std::vector<uint64_t> rightKeys;
     std::vector<std::string> leftFlanks;
     std::vector<std::string> rightFlanks;
     std::vector<double> maxFVARs;

     if ( ( bRecipesFilter ) && ( ( ( nWinFFRQ > 0 ) && ( nMaxFFRQ > 0 ) ) || ( ( nWinFVAR > 0 ) && ( nMaxFVAR ) ) ) ) {
       Logger::gLogger->writeLog("Reading VCF file and calculating the distribution of flanking %d-mers",nWinFFRQ);

       fMaxFFRQ = VcfHelper::vPhred2Err[nMaxFFRQ];

       // read over the VCF files and calculate the frequency of flanking k-mers
       char* lefts = new char[nWinFFRQ+1];
       char* rights = new char[nWinFFRQ+1];
       char* leftsV = new char[nWinFVAR+1];
       char* rightsV = new char[nWinFVAR+1];
       lefts[nWinFFRQ] = '\0';
       rights[nWinFFRQ] = '\0';
       VcfFile* pVcf;
       pVcf = new VcfFile();
       pVcf->setUpgrade(false);
       pVcf->setSiteOnly(true);
       pVcf->setParseValues(false);
       pVcf->setParseGenotypes(false);
       pVcf->setParseDosages(false);
       pVcf->openForRead(sInputVcf.c_str(), 1);

       GenomeSequence genomeSequence;
       genomeSequence.setReferenceName(sFasta.c_str());

       genomeSequence.useMemoryMap(true);

       //Logger::gLogger->writeLog("Loading reference file %s",sRefFile.c_str());
       
       if ( genomeSequence.open() ) {
	 // write a message that new index file is being created
	 if ( genomeSequence.create(false) ) {
	   throw VcfFileException("Failed creating index file of the reference. Please check the file permission");
	 }
	 if ( genomeSequence.open() ) {
	   throw VcfFileException("Failed opening index file of the reference.");
	 }
       }

       //fprintf(stderr,"foo\n");
       while( pVcf->iterateMarker() ) {
	 //fprintf(stderr,"bar\n");

	 VcfMarker* pMarker = pVcf->getLastMarker();

	 genomeIndex_t markerIndex = genomeSequence.getGenomePosition(pMarker->sChrom.c_str(), pMarker->nPos);
	 for(int i=0; i < nWinFFRQ; ++i) {
	   lefts[nWinFFRQ-i-1] = genomeSequence[markerIndex-i-1];
	   rights[i] = genomeSequence[markerIndex+i+1];
	 }
	 for(int i=0; i < nWinFVAR; ++i) {
	   leftsV[nWinFVAR-i-1] = genomeSequence[markerIndex-i-1];
	   rightsV[i] = genomeSequence[markerIndex+i+1];
	 }
	 uint32_t leftKey = VcfHelper::str2TwoBits(lefts, nWinFFRQ);
	 uint32_t rightKey = VcfHelper::str2TwoBits(rights, nWinFFRQ);

	 std::map<uint64_t,int> freqLeftV;
	 std::map<uint64_t,int> freqRightV;
	 std::map<uint64_t,int>::iterator it;
	 for(int i=0; i < nWinFVAR-nMerFVAR+1; ++i) {
	   uint32_t leftKeyV = VcfHelper::str2TwoBits(leftsV, nMerFVAR);
	   uint32_t rightKeyV = VcfHelper::str2TwoBits(rightsV, nMerFVAR);
	   ++(freqLeftV[leftKeyV]);
	   ++(freqRightV[rightKeyV]);
	 }

	 int ln = 0, ls2 = 0, rn = 0, rs2 = 0;
	 for(it = freqLeftV.begin(); it != freqLeftV.end(); ++it) {
	   ++ln;
	   ls2 += (it->second * it->second);
	 }
	 for(it = freqRightV.begin(); it != freqRightV.end(); ++it) {
	   ++rn;
	   rs2 += (it->second * it->second);
	 }
	 //double lv = ((double)ls2/ln - (double)(nWinFVAR-nMerFVAR-1)/ln*(nWinFVAR-nMerFVAR-1)/ln);
	 //double rv = ((double)rs2/rn - (double)(nWinFVAR-nMerFVAR-1)/rn*(nWinFVAR-nMerFVAR-1)/rn);

	 ++(freqLeft[leftKey]);
	 ++(freqRight[rightKey]);
	 leftKeys.push_back(leftKey);
	 rightKeys.push_back(rightKey);
	 leftFlanks.push_back(lefts);
	 rightFlanks.push_back(rights);
       }

       delete [] lefts;
       delete [] rights;
       delete [] leftsV;
       delete [] rightsV;
       delete pVcf;

       Logger::gLogger->writeLog("Finished calculating the distribution of flanking %d-mers",nWinFFRQ);
     }
     //fprintf(stderr,"bar\n");

     StringArray filterKeys;
     std::vector<bool> filterMinMax; // true : min, false : max
     std::vector<double> filterThres;
     std::vector<int> filterIndices;
     StringArray filterNames;
     VcfFile* pIndelVcf = NULL;

     if ( bRecipesFilter ) {
       if ( nMinMQ > 0 ) {
	 filterKeys.Add("MQ");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinMQ));
	 filterNames.Add(String("m")+nMinMQ);
       }
       if ( nMinDP > 0 ) {
	 filterKeys.Add("DP");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinDP));
	 filterNames.Add(String("dp")+nMinDP);
       }
       if ( nMaxDP < INT_MAX ) {
	 filterKeys.Add("DP");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxDP));
	 filterNames.Add(String("DP")+nMaxDP);
       }
       if ( nMinNS > 0 ) {
	 filterKeys.Add("NS");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinNS));
	 filterNames.Add(String("ns")+nMinNS);
       }
       if ( nMaxAB < 100 ) {
	 filterKeys.Add("AB");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxAB)/100.);
	 filterNames.Add(String("AB")+nMaxAB);
       }
       if ( nMaxSTP < INT_MAX ) {
	 filterKeys.Add("STP");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxSTP));
	 filterNames.Add(String("STP")+nMaxSTP);
       }
       if ( nMaxTTT < INT_MAX ) {
	 filterKeys.Add("TTT");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxTTT));
	 filterNames.Add(String("TTT")+nMaxTTT);
       }
       if ( nMinTTT > INT_MIN ) {
	 filterKeys.Add("TTT");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinTTT));
	 filterNames.Add(String("ttt")+nMinTTT);
       }
       if ( nMaxSTR < 100 ) {
	 filterKeys.Add("STR");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxSTR)/100.);
	 filterNames.Add(String("STR")+nMaxSTR);
       }
       if ( nMinSTR > -100 ) {
	 filterKeys.Add("STR");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinSTR)/100.);
	 filterNames.Add(String("str")+nMinSTR);
       }
       if ( nMaxLQZ < INT_MAX ) {
	 filterKeys.Add("LQZ");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxLQZ));
	 filterNames.Add(String("LQZ")+nMaxLQZ);
       }
       if ( nMinSTZ > INT_MIN ) {
	 filterKeys.Add("STZ");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinSTZ));
	 filterNames.Add(String("stz")+nMinSTZ);
       }
       if ( nMaxSTZ < INT_MAX ) {
	 filterKeys.Add("STZ");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxSTZ));
	 filterNames.Add(String("STZ")+nMaxSTZ);
       }
       if ( nMinLQZ > INT_MIN ) {
	 filterKeys.Add("LQZ");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinLQZ));
	 filterNames.Add(String("lqz")+nMinLQZ);
       }
       if ( nMaxRBZ < INT_MAX ) {
	 filterKeys.Add("RBZ");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxRBZ));
	 filterNames.Add(String("RBZ")+nMaxRBZ);
       }
       if ( nMinRBZ > INT_MIN ) {
	 filterKeys.Add("RBZ");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinRBZ));
	 filterNames.Add(String("rbz")+nMinRBZ);
       }
       if ( nMaxCBZ < INT_MAX ) {
	 filterKeys.Add("CBZ");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxCBZ));
	 filterNames.Add(String("CBZ")+nMaxCBZ);
       }
       if ( nMaxCBR < 100 ) {
	 filterKeys.Add("CBR");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxCBR)/100.);
	 filterNames.Add(String("CBR")+nMaxCBR);
       }
       if ( nMinCBR > -100 ) {
	 filterKeys.Add("CBR");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinCBR)/100.);
	 filterNames.Add(String("cbr")+nMinCBR);
       }
       if ( nMaxQBR < 100 ) {
	 filterKeys.Add("QBR");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxQBR)/100.);
	 filterNames.Add(String("QBR")+nMaxQBR);
       }
       if ( nMinQBR > -100 ) {
	 filterKeys.Add("QBR");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinQBR)/100.);
	 filterNames.Add(String("qbr")+nMinQBR);
       }
       if ( nMaxCSR < 100 ) {
	 filterKeys.Add("CSR");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxCSR)/100.);
	 filterNames.Add(String("CSR")+nMaxCSR);
       }
       if ( nMinCSR > -100 ) {
	 filterKeys.Add("CSR");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinCSR)/100.);
	 filterNames.Add(String("csr")+nMinCSR);
       }
       if ( nMaxIOZ < INT_MAX ) {
	 filterKeys.Add("IOZ");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxIOZ));
	 filterNames.Add(String("IOZ")+nMaxIOZ);
       }
       if ( nMinIOR > INT_MIN ) {
	 filterKeys.Add("IOR");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinIOR)/100.);
	 filterNames.Add(String("ior")+nMinIOR);
       }
       if ( nMaxIOR < INT_MAX ) {
	 filterKeys.Add("IOR");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxIOR));
	 filterNames.Add(String("IOR")+nMaxIOR);
       }
       if ( nMaxAOZ < INT_MAX ) {
	 filterKeys.Add("AOZ");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxAOZ));
	 filterNames.Add(String("AOZ")+nMaxAOZ);
       }
       if ( nMaxAOI < INT_MAX ) {
	 filterKeys.Add("AOI");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxAOI));
	 filterNames.Add(String("AOI")+nMaxAOI);
       }
       if ( nMaxMQ0 < 100 ) {
	 filterKeys.Add("MQ0");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxMQ0)/100.);
	 filterNames.Add(String("MQ0_")+nMaxMQ0);
       }
       if ( nMaxMQ10 < 100 ) {
	 filterKeys.Add("MQ10");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxMQ10)/100.);
	 filterNames.Add(String("MQ10_")+nMaxMQ10);
       }
       if ( nMaxMQ20 < 100 ) {
	 filterKeys.Add("MQ20");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxMQ20)/100.);
	 filterNames.Add(String("MQ20_")+nMaxMQ20);
       }
       if ( nMinFIC > INT_MIN ) {
	 filterKeys.Add("FIC");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinFIC)/100.);
	 filterNames.Add(String("fic")+nMinFIC);
       }
       if ( nMaxLQR < 100 ) {
	 filterKeys.Add("LQR");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxLQR)/100.);
	 filterNames.Add(String("LQR")+nMaxLQR);
       }
       if ( nMaxABE < 100 ) {
	 filterKeys.Add("ABE");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxABE)/100.);
	 filterNames.Add(String("ABE")+nMaxABE);
       }
       if ( nMinABE > -100 ) {
	 filterKeys.Add("ABE");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinABE)/100.);
	 filterNames.Add(String("abe")+nMinABE);
       }
       if ( nMaxMBR < 100 ) {
	 filterKeys.Add("MBR");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxMBR)/100.);
	 filterNames.Add(String("MBR")+nMaxMBR);
       }
       if ( nMinMBR > -100 ) {
	 filterKeys.Add("MBR");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinMBR)/100.);
	 filterNames.Add(String("mbr")+nMinMBR);
       }
       if ( nMaxABZ < INT_MAX ) {
	 filterKeys.Add("ABZ");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxABZ));
	 filterNames.Add(String("ABZ")+nMaxABZ);
       }
       if ( nMinABZ > INT_MIN ) {
	 filterKeys.Add("ABZ");
	 filterMinMax.push_back(true);
	 filterThres.push_back(static_cast<double>(nMinABZ));
	 filterNames.Add(String("abz")+nMinABZ);
       }
       if ( nMaxBCS < INT_MAX ) {
	 filterKeys.Add("BCS");
	 filterMinMax.push_back(false);
	 filterThres.push_back(static_cast<double>(nMaxBCS));
	 filterNames.Add(String("BCS")+nMaxBCS);
       }
       if ( ! sIndelVcf.IsEmpty() ) {
	 pIndelVcf = new VcfFile();
	 pIndelVcf->setSiteOnly(true);
	 pIndelVcf->openForRead(sIndelVcf.c_str(),2);
	 pIndelVcf->iterateMarker();
       }

       Logger::gLogger->writeLog("The following filters are in effect:");
       for(int i=0; i < filterKeys.Length(); ++i) {
	 Logger::gLogger->writeLog("%s : %s %s %.2lf",filterNames[i].c_str(), filterKeys[i].c_str(), filterMinMax[i] ? ">=" : "<=", filterThres[i]);
       }
       if ( nMinQUAL > 0 ) {
	 Logger::gLogger->writeLog("q%d : QUAL >= %d",nMinQUAL,nMinQUAL);
       }
       if ( nWinIndel > 0 ) {
	 Logger::gLogger->writeLog("INDEL%d : INDEL >= %d bp with %s",nWinIndel,nWinIndel,sIndelVcf.c_str());
       }
       if ( ( nWinFFRQ > 0 ) && ( nMaxFFRQ > 0 ) ) {
	 Logger::gLogger->writeLog("FFRQ%d : Flanking %d-mer frequency <= 10^{-%.1lf}",nMaxFFRQ,nWinFFRQ,nMaxFFRQ/10.);
       }
       Logger::gLogger->writeLog("");
     }

     //fprintf(stderr,"foo\n");

     if ( ( bRecipesWriteVcf ) || ( bRecipesWriteBed) || ( bRecipesSubset ) ) {

       // Open input VCF/BED file
       VcfFile* pVcf;
       if ( bVCF ) {
	 pVcf = new VcfFile();
	 pVcf->setUpgrade(bRecipesUpgrade);
	 pVcf->setParseValues(true);
	 pVcf->setParseGenotypes(true);
	 pVcf->openForRead(sInputVcf.c_str(), 1);
	 pVcf->nMinGD = nMinGD;
	 pVcf->nMinGQ = nMinGQ;
       }
       else {
	 BedFile* pBed = new BedFile();
	 pBed->openForRead(sInputBed.c_str(), sInputBim.c_str(), sInputFam.c_str(), sFasta.c_str());
	 pVcf = (VcfFile*) pBed;
       }
       
       // Open output file
       IFILE oFile = NULL, oFamFile = NULL, oBimFile = NULL;
       if ( bRecipesWriteVcf ) {
	 String sOutVcf = sOut;

	 if ( bOutPlain ) {
	   if ( ( sOutVcf.Right(7).Compare(".vcf.gz") != 0 ) && ( sOutVcf.Right(4).Compare(".vcf") != 0 ) ) {
	     sOutVcf += ".vcf";  // append .vcf extension
	   }
	   oFile = ifopen(sOutVcf.c_str(),"wb");
	 }
	 else if ( bOutBgzf || bOutGzip ) {
	   InputFile::ifileCompression cMode = bOutBgzf ? InputFile::BGZF : InputFile::GZIP;
	   if ( sOutVcf.Right(4).Compare(".vcf") == 0 ) {
	     sOutVcf += ".gz";
	   }
	   else if ( sOutVcf.Right(7).Compare(".vcf.gz") != 0 ) {
	     sOutVcf += ".vcf.gz";
	   }
	   oFile = ifopen(sOutVcf.c_str(),"wb",cMode);
	 }
	 else {
	   throw VcfFileException("Cannot recognize output compression option");
	 }

	 if ( oFile == NULL ) {
	   Logger::gLogger->error("Cannot open %s file",sOutVcf.c_str());
	 }
	 else {
	   Logger::gLogger->writeLog("Writing to VCF file %s",sOutVcf.c_str());
	 }

	 pVcf->printVCFHeader(oFile);
       }
       else if ( bRecipesWriteBed ) {
	 if ( bOutPlain ) {
	   oFile = ifopen((sOut+".bed").c_str(),"wb");
	   oBimFile = ifopen((sOut+".bim").c_str(),"wb");
	   oFamFile = ifopen((sOut+".fam").c_str(),"wb");
	 }
	 else if ( bOutBgzf ) {
	   oFile = ifopen((sOut+".bed.gz").c_str(),"wb",InputFile::BGZF);
	   oBimFile = ifopen((sOut+".bim.gz").c_str(),"wb",InputFile::BGZF);
	   oFamFile = ifopen((sOut+".fam.gz").c_str(),"wb",InputFile::BGZF);
	 }
	 else if ( bOutGzip ) {
	   oFile = ifopen((sOut+".bed.gz").c_str(),"wb",InputFile::GZIP);
	   oBimFile = ifopen((sOut+".bim.gz").c_str(),"wb",InputFile::GZIP);
	   oFamFile = ifopen((sOut+".fam.gz").c_str(),"wb",InputFile::GZIP);
	 }
	 else {
	   throw VcfFileException("Cannot recognize output compression option");
	 }
	 
	 if ( ( oFile == NULL ) || ( oBimFile == NULL ) || ( oFamFile == NULL ) ) {
	   Logger::gLogger->error("Cannot open %s.{bim,bed,fam} file",sOut.c_str());
	 }
	 pVcf->printBEDHeader(oFile,oFamFile);
       }

       // identify the list of individuals to be subsetted
       std::vector< std::string > subsetNames;        // list of subset IDs
       std::vector< std::vector<int> > subsetIndices; // per subset list of individuals
       std::vector< IFILE > subsetOutFiles;           // per subset list of output files
       if ( bRecipesSubset ) {
	 String line;
	 IFILE iSubsetFile = ifopen( sInputSubset.c_str(), "rb" );
	 std::map< std::string, int > name2SampleInd;
	 StringArray tok, tok2;
	 std::string ind, subset;
	 int subsetId;

	 // build map to sample name to index
	 for(int i=0; i < (int)pVcf->vpVcfInds.size(); ++i) {
	   name2SampleInd[pVcf->vpVcfInds[i]->sIndID.c_str()] = i;
	 }

	 if ( iSubsetFile == NULL ) {
	   Logger::gLogger->error("Cannot open %s file",sInputSubset.c_str());
	 }
	 while ( line.ReadLine(iSubsetFile) > 0 ) {
	   tok.ReplaceTokens(line,"\t\r\n ");
	   if ( tok.Length() < 2 ) {
	     Logger::gLogger->error("Cannot recognize subset label for in %s ",sInputSubset.c_str());
	   }
	   tok2.ReplaceColumns(tok[1],',');
	   ind = tok[0].c_str();

	   //fprintf(stderr,"%s\t%s\t%s\n",line.c_str(),tok[0].c_str(),tok[1].c_str());

	   // check if sample exists in the VCF
	   if ( name2SampleInd.find(ind) == name2SampleInd.end() ) {
	     Logger::gLogger->warning("Cannot recognize individual ID %s. Skipping..",ind.c_str());
	     continue;
	   }

	   // iterate thru subset names
	   for(int i=0; i < tok2.Length(); ++i) {
	     subset = tok2[i].c_str();
	     subsetId = -1;
	     for(int j=0; j < (int)subsetNames.size(); ++j) {
	       if ( subset.compare(subsetNames[j]) == 0 ) {
		 subsetId = j;
		 break;
	       }
	     }
	     if ( subsetId < 0 ) {
	       subsetNames.push_back(subset);
	       subsetIndices.push_back(std::vector<int>());
	       subsetId = subsetNames.size()-1;
	     }
	     subsetIndices[subsetId].push_back(name2SampleInd[ind]);
	   }
	 }
	 
	 // and create output files
	 for(int i=0; i < (int)subsetNames.size(); ++i) {
	   String sOutVcf = sOut + "." + subsetNames[i].c_str();
	   IFILE f;

	   if ( bOutPlain ) {
	     if ( ( sOutVcf.Right(7).Compare(".vcf.gz") != 0 ) && ( sOutVcf.Right(4).Compare(".vcf") != 0 ) ) {
	       sOutVcf += ".vcf";  // append .vcf extension
	     }
	     f = ifopen(sOutVcf.c_str(),"wb");
	   }
	   else if ( bOutBgzf || bOutGzip ) {
	     InputFile::ifileCompression cMode = bOutBgzf ? InputFile::BGZF : InputFile::GZIP;
	     if ( sOutVcf.Right(4).Compare(".vcf") == 0 ) {
	       sOutVcf += ".gz";
	     }
	     else if ( sOutVcf.Right(7).Compare(".vcf.gz") != 0 ) {
	       sOutVcf += ".vcf.gz";
	     }
	     f = ifopen(sOutVcf.c_str(),"wb",cMode);
	   }
	   else {
	     throw VcfFileException("Cannot recognize output compression option");
	   }

	   if ( f == NULL ) {
	     Logger::gLogger->error("Error in opening output file");
	   }

	   subsetOutFiles.push_back(f);
	   pVcf->printVCFHeaderSubset(f,subsetIndices[i]);
	 }

	 Logger::gLogger->writeLog("Found %d groups for subsetting",subsetNames.size());
	 for(int i=0; i < (int)subsetNames.size(); ++i) {
	   Logger::gLogger->writeLog("%s - %u individuals",subsetNames[i].c_str(), subsetIndices[i].size());
	 }
       }

       // read input files
       for( int cnt = 0; pVcf->iterateMarker(); ++cnt ) {
	 VcfMarker* pMarker = pVcf->getLastMarker();

	 if ( ( nPrintUnit > 0 ) && ( (cnt+1) % nPrintUnit == 0 ) ) {
	   Logger::gLogger->writeLog("Processing %d markers at %s:%d",cnt+1,pMarker->sChrom.c_str(),pMarker->nPos);
	 }

	 // Apply filters
	 if ( bRecipesFilter ) {
	   if ( bKeepFilter ) {
	     if ( ( pMarker->asFilters.Length() == 1 ) && ( ( pMarker->asFilters[0].Compare(".") == 0 ) || ( pMarker->asFilters[0].Compare("PASS") == 0 ) || ( pMarker->asFilters[0].Compare("0") == 0 ) ) ) {
	       pMarker->asFilters.Clear();
	     }
	   }
	   else {
	     pMarker->asFilters.Clear();
	   }

	   // QUAL filter
	   if ( ( nMinQUAL > 0 ) && ( pMarker->fQual < nMinQUAL ) ) {
	     pMarker->asFilters.Add(String("q")+nMinQUAL);
	   }


	   // Indel filter
	   while ( ( pIndelVcf != NULL ) && ( ! pIndelVcf->bEOF ) && 
	      VcfHelper::compareGenomicPos( pIndelVcf->getLastMarker()->sChrom, pIndelVcf->getLastMarker()->nPos, pMarker->sChrom, pMarker->nPos ) < 0 ) {
	     pIndelVcf->iterateMarker();
	     //Logger::gLogger->writeLog("IndelVCF %s:%d",pIndelVcf->getLastMarker()->sChrom.c_str(),pIndelVcf->getLastMarker()->nPos);
	   }
	   
	   if ( ( pIndelVcf != NULL ) && ( !pIndelVcf->bEOF ) && ( nWinIndel > 0 ) ) {
	     int d1 = VcfHelper::compareGenomicPos( pIndelVcf->getLastMarker(0)->sChrom, pIndelVcf->getLastMarker(0)->nPos, pMarker->sChrom, pMarker->nPos );
	     int d2 = ( pIndelVcf->nNumMarkers > 1 ) ? VcfHelper::compareGenomicPos( pMarker->sChrom, pMarker->nPos, pIndelVcf->getLastMarker(1)->sChrom, pIndelVcf->getLastMarker(1)->nPos ) : 1000000;
	     if ( ( d1 < 0 ) || ( d2 < 0 ) ) {
	       Logger::gLogger->warning("%s:%d, d1=%d, d2=%d",pMarker->sChrom.c_str(),pMarker->nPos,d1,d2);
	     }
	     //Logger::gLogger->writeLog("%s:%d, d1=%d, d2=%d",pMarker->sChrom.c_str(),pMarker->nPos,d1,d2);
	     if ( ( d1 < nWinIndel ) || ( d2 < nWinIndel ) ) {
	       pMarker->asFilters.Add(String("INDEL")+nWinIndel);
	     }
	   }

	   // Update filter index if order in the INFO field has been changed
	   if ( ( pVcf->nBuffers == 1 ) && ( pMarker->bPreserved ) ) {
	     // do not update filterIndices
	   }
	   else {
	     filterIndices.resize(filterKeys.Length());
	     for(int i=0; i < filterKeys.Length(); ++i) {
	       filterIndices[i] = pMarker->asInfoKeys.Find(filterKeys[i]);
	     }
	   }

	   // apply standard filters
	   for(int i=0; i < (int)filterIndices.size(); ++i) {
	     if ( filterIndices[i] >= 0 ) {
	       if ( filterMinMax[i] ) { // min
		 if ( atof(pMarker->asInfoValues[filterIndices[i]].c_str()) < filterThres[i] ) {
		   pMarker->asFilters.Add(filterNames[i]);
		 }
	       }
	       else {
		 if ( atof(pMarker->asInfoValues[filterIndices[i]].c_str()) > filterThres[i] ) {
		   pMarker->asFilters.Add(filterNames[i]);
		 }
	       }
	     }
	   }

	   // apply flanking frquency filters
	   if ( ( nWinFFRQ > 0 ) && ( nMaxFFRQ > 0 ) ) {
	     int maxFrq = freqLeft[leftKeys[cnt]];
	     if ( maxFrq < freqRight[rightKeys[cnt]] ) {
	       maxFrq = freqRight[rightKeys[cnt]];
	     }

	     if ( maxFrq > fMaxFFRQ * leftKeys.size() ) {
	       pMarker->asFilters.Add(String("FFRQ")+nMaxFFRQ);
	     }

	     pMarker->asInfoKeys.Add("LFLANK");
	     pMarker->asInfoValues.Add(leftFlanks[cnt].c_str());
	     pMarker->asInfoKeys.Add("RFLANK");
	     pMarker->asInfoValues.Add(rightFlanks[cnt].c_str());
	   }
	 }

	 if ( bRecipesWriteVcf ) {
	   pMarker->printVCFMarker(oFile,false,qGeno);
	 }
	 else if ( bRecipesWriteBed ) {
	   pMarker->printBEDMarker(oFile,oBimFile,false);
	 }

	 if ( bRecipesSubset ) {
	   bool filterPass = true;
	   if ( bFiltOnlySubset ) {
	     if ( ( pMarker->asFilters.Length() > 0 ) && ( pMarker->asFilters[0].Compare("PASS") != 0 ) ) {
	       filterPass = false;
	     }
	   }

	   if ( filterPass ) {
	     for(int i=0; i < (int)subsetNames.size(); ++i) {
	       pMarker->printVCFMarkerSubset(subsetOutFiles[i],subsetIndices[i],bMonoSubset);
	     }
	   }
	 }
       }
       
       if ( oFile != NULL ) {
	 ifclose(oFile);
       }
       if ( bRecipesWriteBed ) {
	 ifclose(oBimFile);
	 //ifclose(oFamFile);
       }
       if ( bRecipesSubset ) {
	 for(int i=0; i < (int)subsetNames.size(); ++i) {
	   ifclose(subsetOutFiles[i]);
	 }
       }
     }
     else {
       Logger::gLogger->error("One of --write-vcf, --write-bed, --subset or --summarize recipes must be provided to process the input file");
     }
   }
   catch (VcfFileException e) {
     Logger::gLogger->error(e.msg.c_str());
   }

   time(&t);
   Logger::gLogger->writeLog("Analysis finished on %s", ctime(&t));

   return 0;
}

     /*
   int nArgStart  = pl.ReadWithTrailer(argc,argv) + 1;
   pl.Status();

   time_t t;
   time(&t);

   printf("Analysis started on %s\n", ctime(&t));
   fflush(stdout);

   int n = argc - nArgStart;
   argv += nArgStart;

   if ( n == 0 )
     error("No input files listed at the end of command line\n");

   if ( bCmdIntersect ) {
     try {
       vcfFile* vcfs = new vcfFile[n];
       vcfRecord* records = new vcfRecord[n];
       int* nChrs = new int[n]();
       bool* bAdvances = new bool[n];

       if ( nIsecWay == 0 ) 
	 nIsecWay = n;

       for (int i = 0; i < n; i++) {
	 vcfs[i].OpenForRead(argv[i]);
	 nChrs[i] = 0;
	 records[i].setPosition(0);
       }

       IFILE outVCF = ifopen(sOutFile.c_str(), "wb");
       //FILE* outVCF = fopen(sOutFile.c_str(), "w");

       // by default, print the meta information from the first file
       for(int i=0; i < vcfs[0].getMetaCount(); ++i) {
	 ifprintf(outVCF,"## %s = %s\n",vcfs[0].getMetaKey(i).c_str(), vcfs[0].getMetaValue(i, "<na>").c_str());
       }
       
       // print the header line
       ifprintf(outVCF,"#CHROM\tPOS\tID\tREF\tALT\tQUAL\tFILTER\tINFO");
       if ( bIsecGeno ) {
	 // not sure how to merge genotypes
	 error("--isec-geno option is not supported yet");
       }
       ifprintf(outVCF,"\n");
       
       // print data line, by taking intersecting lines
       // assume that every chromosome is observed so far
       String currentChrom;
       int minChrom;
       int minPos;
       do {
	 minChrom = INT_MAX;
	 minPos = INT_MAX;

	 // find minimum position
	 for(int i=0; i < n; ++i) {
	   if ( minChrom > nChrs[i] ) {
	     minChrom = nChrs[i];
	     currentChrom = records[i].getChromosome();
	     minPos = records[i].getPosition();
	   }
	   else if ( ( minChrom == nChrs[i] ) && ( minPos > records[i].getPosition() ) ) {
	     minPos = records[i].getPosition();
	   }
	 }
	 
	 // determine which to advance;
	 int nAdvance = 0;
	 for(int i=0; i < n; ++i) {
	   if ( ( minChrom == nChrs[i] ) && ( minPos == records[i].getPosition() ) ) {
	     bAdvances[i] = true;
	     ++nAdvance;
	   }
	   else {
	     bAdvances[i] = false;
	   }
	 }

	 // advance lines to read
	 for(int i=0; i < n; ++i) {
	   if ( bAdvances[i] ) {
	     if ( !vcfs[i].ReadRecord(records[i], false) ) {
	       records[i].setPosition(INT_MAX);
	       nChrs[i] = INT_MAX;
	     }
	     if ( currentChrom.Compare(records[i].getChromosome()) != 0 ) {
	       ++(nChrs[i]);
	     }
	   }
	 }
	 if ( ( nAdvance == n ) && ( currentChrom.Compare(records[0].getChromosome()) != 0 ) ) {
	   currentChrom = records[0].getChromosome();
	 }

	 // determine whether to print the SNP or not
	 if ( nAdvance >= nIsecWay ) {
	   // check consistency of REF and ALT
	   String ref, alt;
	   bool bConsistent = true;
	   String qual;
	   String info;
	   String filter;

	   for(int i=0; i < n; ++i) {
	     if ( bAdvances[i] ) {
	       if ( ref.IsEmpty() ) {
		 ref = records[i].getReferenceAllele().ToUpper();
		 alt = records[i].getAlternateAlleles().ToUpper();
		 qual = records[i].getQuality();
		 info = records[i].getInfo();
		 filter = records[i].getFilter();
	       }
	       else {
		 if ( ref.Compare(records[i].getReferenceAllele().ToUpper()) != 0 ) {
		   bConsistent = false;
		 }
		 if ( alt.Compare(records[i].getAlternateAlleles().ToUpper()) != 0) {
		   bConsistent = false;
		 }
		 info += ";";
		 info += records[i].getInfo();
	       }
	     }
	   }

	   if ( bResetFilter ) {
	     filter = ".";
	   }

	   if ( ( bConsistent || bIsecAllowInconsistency ) && ( minChrom < INT_MAX ) ) {
	     ifprintf(outVCF,"%s\t%d\t.\t%s\t%s\t%s\t%s\t%s\n",
		     currentChrom.c_str(), 
		     minPos,
		     ref.c_str(),
		     alt.c_str(),
		     qual.c_str(),
		     filter.c_str(),
		     info.c_str()
		     );
	   }
	 }
       }
       while ( minChrom < INT_MAX );

       ifclose(outVCF);
       //fclose(outVCF);

       delete [] vcfs;
     }
     catch ( vcfFileException &e ) {
       fprintf(stderr,"Error: %s\n", e.what());
       error("Error in opening VCF file");
     }
   }
   else if ( bCmdFilter ) {
     error("Not implemented yet");
   }
   else {
     error("No command option was given. Doing nothing\n");
     }*/


