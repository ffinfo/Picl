/* 
 * File:   ReadNameConverter.cpp
 * Author: peter
 * 
 * Created on February 18, 2013, 11:15 AM
 */

#include "ReadNameConverter.h"
#include "limits.h"
#include <iostream>
#include <stdexcept>

ReadNameConverter::ReadNameConverter(unsigned int* NameType, vector<ReplaceNameArg>* NameTrim) {
    this->NameType = NameType;
    this->NameReplace = NameTrim;
}

ReadNameConverter::ReadNameConverter() {
    //NameTrim = new vector<string>;
    NameReplace = new vector<ReplaceNameArg>;
}

ReadNameConverter::~ReadNameConverter() {
}

unsigned long int ReadNameConverter::ConvertNameToInt(string &Name) {
    string ReadGroup;
    return ConvertNameToInt(Name, ReadGroup);
}

bool ReadNameConverter::TrimName(string& name) {
    bool found = false;
    for (ReplaceNameArg arg : *NameReplace) {
        unsigned int it = name.find(arg.Trim);
        if (it < name.size()) {
            name = name.substr(0, it) + arg.Replace + name.substr(it + arg.Trim.size(), name.size());
            found = true;
        }
    }
    return found;
}

unsigned long int ReadNameConverter::ConvertNameToInt(string &Name, string &ReadGroup) { 
    switch (*NameType) {
        case Solid5: return ConvertSolid5(Name);
        case Solid4: return ConvertSolid4(Name);
        case Solid4_2: return ConvertSolid4_2(Name);
        case Solid5_2: return ConvertSolid5_2(Name, ReadGroup);
        case Ilumnia: return ConvertIlumnia(Name, ReadGroup);
        case Simulated: return ConvertSimulated(Name);
        default: return 7;
    }
    cerr << "No NameType found while Convert Name To Int" << endl;
    exit(1);
}

bool ReadNameConverter::AddReadGroup(string &ReadGroup) {
    try {
        ReadGroups.at(ReadGroup);
    } catch (std::out_of_range e) {
        ReadGroups[ReadGroup] = ReadGroups.size();
        return true;
    }
    return false;
}

unsigned long int ReadNameConverter::ConvertSolid5(string &Name) { // Based on: lane#_(samplename):####_####_####
    unsigned int lane, IdNumber[] = {0,0,0};
    
    istringstream buffer ( Name.substr(Name.find("lane")+4,1) );
    buffer >> lane;
    
    string NameID = Name.substr(Name.find(':') + 1, Name.size());
    
    istringstream bn1 (NameID.substr(0, NameID.find('_')));
    bn1 >> IdNumber[0];
    NameID = NameID.substr(NameID.find('_') + 1, NameID.length());
        
    istringstream bn2 (NameID.substr(0, NameID.find('_')));
    bn2 >> IdNumber[1];
    NameID = NameID.substr(NameID.find('_') + 1, NameID.length());
        
    istringstream bn3 (NameID);
    bn3 >> IdNumber[2];
    
    unsigned long int id = (lane * 1000000000000);
    id += ((unsigned long int)IdNumber[0] * 100000000);
    id += ((unsigned long int)IdNumber[1] * 10000);
    id += ((unsigned long int)IdNumber[2]);
        
    return id;
}

unsigned long int ReadNameConverter::ConvertSolid4(string &Name) { // Based on: (samplename):####_####_####
    unsigned int IdNumber[] = {0,0,0};
    
    string NameID = Name.substr(Name.find(':') + 1, Name.size());
    
    istringstream bn1 (NameID.substr(0, NameID.find('_')));
    bn1 >> IdNumber[0];
    NameID = NameID.substr(NameID.find('_') + 1, NameID.length());
        
    istringstream bn2 (NameID.substr(0, NameID.find('_')));
    bn2 >> IdNumber[1];
    NameID = NameID.substr(NameID.find('_') + 1, NameID.length());
        
    istringstream bn3 (NameID);
    bn3 >> IdNumber[2];
    
    unsigned long int id = ((unsigned long int)IdNumber[0] * 100000000);
    id += ((unsigned long int)IdNumber[1] * 10000);
    id += ((unsigned long int)IdNumber[2]);
        
    return id;
}

unsigned long int ReadNameConverter::ConvertSolid4_3(string &Name, string &ReadGroup) { // Based on: (samplename):####_####_#### + Readgroup
    unsigned int IdNumber[] = {0,0,0};
    
    string NameID = Name.substr(Name.find(':') + 1, Name.size());
    
    istringstream bn1 (NameID.substr(0, NameID.find('_')));
    bn1 >> IdNumber[0];
    NameID = NameID.substr(NameID.find('_') + 1, NameID.length());
        
    istringstream bn2 (NameID.substr(0, NameID.find('_')));
    bn2 >> IdNumber[1];
    NameID = NameID.substr(NameID.find('_') + 1, NameID.length());
        
    istringstream bn3 (NameID);
    bn3 >> IdNumber[2];
    
    unsigned long int id = ((unsigned long int)IdNumber[0] * 10000000000);
    id += ((unsigned long int)IdNumber[1] * 1000000);
    id += ((unsigned long int)IdNumber[2] * 100);
    id += ReadGroups[ReadGroup];
        
    return id;
}

unsigned long int ReadNameConverter::ConvertSolid4_2(string &Name) { // Based on: ####_####_####
    int IdNumber[] = {0,0,0};
    
    string NameID = Name;
    
    //istringstream bn1 (NameID.substr(0, NameID.find('_')));
    //bn1 >> IdNumber[0];
    IdNumber[0] = atoi(NameID.substr(0, NameID.find('_')).c_str());
    NameID = NameID.substr(NameID.find('_') + 1, NameID.length());
        
    //istringstream bn2 (NameID.substr(0, NameID.find('_')));
    //bn2 >> IdNumber[1];
    IdNumber[1] = atoi(NameID.substr(0, NameID.find('_')).c_str());
    NameID = NameID.substr(NameID.find('_') + 1, NameID.length());
        
    //istringstream bn3 (NameID);
    //bn3 >> IdNumber[2];
    IdNumber[2] = atoi(NameID.c_str());
    
    unsigned long int id = ((unsigned long int)IdNumber[0] * 100000000);
    id += ((unsigned long int)IdNumber[1] * 10000);
    id += ((unsigned long int)IdNumber[2]);
        
    return id;
}

unsigned long int ReadNameConverter::ConvertSolid5_2(string &Name, string &ReadGroup) { // Based on: (samplename)_####_######### + ReadGroup
    unsigned int it = Name.find_last_of("_");
    unsigned long int ID = atoi(Name.substr(it + 1, Name.size()).c_str());
    Name = Name.substr(0, it);
    it = Name.find_last_of("_");
    unsigned long int plane = atoi(Name.substr(it + 1, Name.size()).c_str());
    Name = Name.substr(0, it);
    
    TrimName(ReadGroup);
        
    return (ReadGroups[ReadGroup] + (plane * 100) + (ID * 1000000));
}

unsigned long int ReadNameConverter::ConvertIlumnia(string& Name, string& ReadGroup) {
    //int it = Name.find_last_of(".");
    unsigned long int ID = atoi(Name.substr(Name.find_last_of(".") + 1, Name.size()).c_str());
    
    TrimName(ReadGroup);
    
    return (ReadGroups[ReadGroup] + (ID * 1000000000));
}

unsigned long int ReadNameConverter::ConvertSimulated(string& Name) { // ref-id
    unsigned int split = Name.find_first_of('-');
    string ref = Name.substr(0, split);
    unsigned long int id = atoi(Name.substr(split+1, Name.size()).c_str());
    //cout << "Ref: " << ref << "  ID: " << id << endl;
    
    unsigned short int hash = 0;
    //int offset = 'a' - 1;
    for(string::const_iterator it=ref.begin(); it!=ref.end(); ++it) {
        hash = hash << 1 | (*it);
    }
    //cout << hash << endl;
    
    return hash + (id * USHRT_MAX);
}