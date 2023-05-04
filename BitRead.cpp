
/*
    Implementacia Bitstreamu pre kompresor
    Autor: Martin Matějka <xmatej55@vutbr.cz>
    KKO 2022/23
*/

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include "argparse.cpp"
#include <fstream>
#include <vector>
#include <map>
#include <bitset>

#include <queue>
#include <utility>
#include <algorithm>

#include "defines.h"

#ifndef __BITREAD_CL_
#define __BITREAD_CL_


using namespace std;

class BitRead {

    public:
    
    const uint8_t *ptr;
    const uint8_t *end;
    int bit_index;

    unsigned char act_char;

    bool stop;
    bool last_char;

    BitRead(const uint8_t *p, const uint8_t *e){
        ptr=p;
        end=e;
        bit_index=7;
        stop = false;
        last_char=false;
        act_char=(*ptr);
    }
    
    bool getNextBit(){
        bool res = (act_char>>bit_index)&1;        
        bit_index--; 
        if(bit_index<0){
            getNextChar();
        }
        return res;
    }

    void getNextChar(){
        bit_index = 7;
        ptr++;
        if(ptr == end) stop=true;
        act_char=(*ptr);
    }


};

#endif