/* 
 * File:   ReadNameConverter.h
 * Author: peter
 *
 * Created on February 18, 2013, 11:15 AM
 */

#ifndef READNAMECONVERTER_H
#define	READNAMECONVERTER_H

//#include "Config.h"
#include <sstream>
#include <map>
#include <vector>

using namespace std;

class ReadNameConverter {
public:
    struct ReplaceNameArg {
        string Trim;
        string Replace;
    };
    
    ReadNameConverter();
    ReadNameConverter(unsigned int* NameType, vector<ReplaceNameArg>* NameTrim);
    virtual ~ReadNameConverter();
    
    unsigned long int ConvertNameToInt(string &Name);
    unsigned long int ConvertNameToInt(string &Name, string &ReadGroup);
    bool AddReadGroup(string &ReadGroup);
    bool TrimName(string &name);
    void ReplaceName(string &name);
    
    map<string,int> ReadGroups;
    //vector<string>* NameTrim;
    vector<ReplaceNameArg>* NameReplace;
    
    static const unsigned int Solid5 = 1;
    static const unsigned int Solid4 = 2;
    static const unsigned int Solid4_2 = 3;
    static const unsigned int Solid5_2 = 4;
    static const unsigned int Ilumnia = 5;
    static const unsigned int Simulated = 6;
private:
    unsigned int* NameType;
    unsigned long int ConvertSolid5(string &Name);
    unsigned long int ConvertSolid4(string &Name);
    unsigned long int ConvertSolid4_2(string &Name);
    unsigned long int ConvertSolid4_3(string &Name, string &ReadGroup);
    unsigned long int ConvertSolid5_2(string &Name, string &ReadGroup);
    unsigned long int ConvertIlumnia(string &Name, string &ReadGroup);
    unsigned long int ConvertSimulated(string &Name);
};

#endif	/* READNAMECONVERTER_H */

