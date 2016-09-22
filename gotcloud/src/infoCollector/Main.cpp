////////////////////////////////////////////////////////////////////// 
// mergeFilterStats/Main.cpp 
// (c) 2010 Hyun Min Kang
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
// Sunday February 13, 2011

#include <vector>

#include "Parameters.h"
#include "InputFile.h"
#include "Error.h"
#include "Logger.h"
#include "VariantStatizer.h"
#include "StringArray.h"

Logger* Logger::gLogger = NULL;

int main(int argc, char** argv) {
  printf("variantStatizer 1.0.0 -- Merge VCF INFO stats\n"
	 "(c) 2010 Hyun Min Kang\n\n");

  String sAnchorVcf;
  String sPrefix;
  String sSuffix;
  String sListf;
  String sOutVcf;
  int listSkip = 2;
  bool bVerbose = true;

  ParameterList pl;

  BEGIN_LONG_PARAMETERS(longParameters)
    LONG_PARAMETER_GROUP("Input Options")
    LONG_STRINGPARAMETER("anchor",&sAnchorVcf)
    LONG_STRINGPARAMETER("prefix",&sPrefix)
    LONG_STRINGPARAMETER("suffix",&sSuffix)
    LONG_STRINGPARAMETER("list",&sListf)

    LONG_PARAMETER_GROUP("Output Options")
    LONG_STRINGPARAMETER("outvcf",&sOutVcf)
    LONG_PARAMETER("verbose",&bVerbose)

    LONG_PARAMETER_GROUP("Input file formats")
    LONG_INTPARAMETER("skipList",&listSkip)

    BEGIN_LEGACY_PARAMETERS()
    LONG_STRINGPARAMETER("index",&sListf)
    LONG_INTPARAMETER("skipIndex",&listSkip)
  END_LONG_PARAMETERS();

  pl.Add(new LongParameters("Available Options", longParameters));
  pl.Read(argc, argv);
  pl.Status();
  
  // create objects for logging
  if ( sOutVcf.IsEmpty() ) {
    error("ERROR: --outvcf is empty\n");
    //abort();
  }

  Logger::gLogger = new Logger((sOutVcf+".log").c_str(), bVerbose);
  
  time_t t;
  time(&t);
  Logger::gLogger->writeLog("Analysis started on %s", ctime(&t));
  
  ////////////////////////////////////////////////////////////
  // check the compatibility of arguments
  ///////////////////////////////////////////////////////////
  // Check the sanity of input file arguments
  ///////////////////////////////////////////////////////////
  if ( sAnchorVcf.IsEmpty() || sPrefix.IsEmpty() || sSuffix.IsEmpty() || sListf.IsEmpty() ) {
    Logger::gLogger->error("All the --anchor, --prefix, --suffix, --list options must be present");
  }

  // Read list file and list the file names to read
  IFILE listFile = ifopen( sListf.c_str(), "rb" );
  String line;
  std::vector<std::string> inputVcfs;
  while( line.ReadLine(listFile) > 0 ) {
    //fprintf(stderr,"line = %s",line.c_str());
    StringArray tok;
    tok.ReplaceTokens(line," \t\r\n");
    if(tok.Length() == 0)
    {
        // Skip blank line.
        continue;
    }
    if ( tok.Length() < listSkip + 1 ) {
      Logger::gLogger->error("Cannot recognize %s in the list file",line.c_str());
    }
    for(int i=listSkip; i < tok.Length(); ++i) {
      std::string s(sPrefix.c_str());
      StringArray paths;
      paths.ReplaceColumns(tok[i],'/');
      s += paths[paths.Length()-1].c_str();
      s += sSuffix.c_str();
      inputVcfs.push_back(s);
    }
  }

  int nVcfs = inputVcfs.size();
  Logger::gLogger->writeLog("Merging statistics from the following %d files..",nVcfs);
  for(int i=0; i < (int)inputVcfs.size(); ++i) {
    Logger::gLogger->writeLog("%s",inputVcfs[i].c_str());
  }

  // Read anchor VCF files
  VariantStatizer fStat;
  fStat.loadAnchorVcf(sAnchorVcf.c_str());

  // Add VCF file to add in multi-threaded manner
  for(int i=0; i < nVcfs; ++i) {
    fStat.appendStatVcf(inputVcfs[i].c_str());
  }

  // Re-read anchor VCF and print out merged VCF
  fStat.writeMergedVcf(sOutVcf.c_str());
  return 0;
}
