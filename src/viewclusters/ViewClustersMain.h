/* 
 * File:   ViewClustersMain.h
 * Author: peter
 *
 * Created on March 18, 2013, 3:01 PM
 */

#ifndef VIEWCLUSTERSMAIN_H
#define	VIEWCLUSTERSMAIN_H

#include "../Config.h"
#include "../Structs.h"

using namespace std;

class ViewClustersMain {
public:
    ViewClustersMain();
    virtual ~ViewClustersMain();
    
    int main(int argc, char *argv[]);
    static int usage();
private:
    
};

#endif	/* VIEWCLUSTERSMAIN_H */

