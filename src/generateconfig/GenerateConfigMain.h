/* 
 * File:   GenerateConfigMain.h
 * Author: peter
 *
 * Created on April 25, 2013, 3:11 PM
 */

#ifndef GENERATECONFIGMAIN_H
#define	GENERATECONFIGMAIN_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

class GenerateConfigMain {
public:
    GenerateConfigMain();
    virtual ~GenerateConfigMain();
    
    int main(int argc, char *argv[]);
    static int usage();
private:

};

#endif	/* GENERATECONFIGMAIN_H */

