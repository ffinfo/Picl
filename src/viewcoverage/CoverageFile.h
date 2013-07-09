/* 
 * File:   CoverageFile.h
 * Author: peter
 *
 * Created on April 2, 2013, 3:19 PM
 */

#ifndef COVERAGEFILE_H
#define	COVERAGEFILE_H

#include "../Structs.h"
#include <cstdlib>
#include <string>

class CoverageFile {
public:
    CoverageFile();
    CoverageFile(string filename);
    virtual ~CoverageFile();
    
    vector<CoverageFileRead> getAllReads();
    vector<CoverageFileRead> getRegion(unsigned int begin, unsigned int end);
    CoverageFileRead getBinOnPosition(unsigned int position);
    void setBinOnPosition(unsigned int position, CoverageFileRead &coverageFileRead);
    unsigned int getBinsize();
    unsigned int getBin(unsigned int position);
    unsigned int getStartPositionOfBin(unsigned int bin);
    unsigned int getBinCount();
private:
    unsigned int binCount = 0;
    unsigned int binsize;
    string filename;
};

#endif	/* COVERAGEFILE_H */

