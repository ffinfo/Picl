/* 
 * File:   CoverageFile.cpp
 * Author: peter
 * 
 * Created on April 2, 2013, 3:19 PM
 */

#include <iostream>
#include <fstream>
#include <algorithm>

#include "CoverageFile.h"

CoverageFile::CoverageFile() {
}

CoverageFile::CoverageFile(string filename) {
    this->filename = filename;
    fstream file;
    file.open(filename, ios::binary | ios::in);
    if (not file.is_open()) {
        cerr << "[Error] Could not open file: " << filename << endl;
        return;
    }
    file.read((char*) & binsize, sizeof(unsigned int));
    long begin = file.tellg();
    file.seekg(0, ios::end);
    long end = file.tellg();
    binCount = (end - begin) / sizeof(CoverageFileRead);
    file.close();
}

CoverageFile::~CoverageFile() {
}

unsigned int CoverageFile::getBinsize() {
    return binsize;
}

vector<CoverageFileRead> CoverageFile::getAllReads() {
    return getRegion(0, (binsize * binCount));
}

unsigned int CoverageFile::getBin(unsigned int position) {
    return floor((double)position / (double)binsize);
}

unsigned int CoverageFile::getStartPositionOfBin(unsigned int bin) {
    return (bin * binsize);
}

unsigned int CoverageFile::getBinCount() {
    return binCount;
}

vector<CoverageFileRead> CoverageFile::getRegion(unsigned int begin, unsigned int end) {
    vector<CoverageFileRead> reads;
    
    fstream file;
    file.open(filename, ios::binary | ios::in);
    if (not file.is_open()) {
        cerr << "[Error] Could not open file: " << filename << endl;
        return reads;
    }
    
    unsigned int startBin = floor((double)begin / (double)binsize);
    file.seekg(sizeof(binsize) + (sizeof(CoverageFileRead) * startBin));
    unsigned int t = startBin;
    while (not file.eof()) {
        CoverageFileRead read;
        file.read((char*) & read, sizeof(CoverageFileRead));
        if (begin != end and (t * binsize) > end) break;
        reads.push_back(read);
        t++;
    }
    file.close();
    reads.shrink_to_fit();
    
    return reads;
}

CoverageFileRead CoverageFile::getBinOnPosition(unsigned int position) {
    for (CoverageFileRead read : getRegion(position, position)) return read;
    cerr << "[Warning] position " << position << " not found in " << filename << endl;
    CoverageFileRead read;
    return read;
}

void CoverageFile::setBinOnPosition(unsigned int position, CoverageFileRead &coverageFileRead) {
    fstream file;
    file.open(filename, ios::binary | ios::out);
    unsigned int bin = floor((double)position / (double)binsize);
    file.seekp(sizeof(binsize) + (sizeof(CoverageFileRead) * bin));
    file.write((char*) & coverageFileRead, sizeof(CoverageFileRead));
    file.close();
}