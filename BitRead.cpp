
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

    BitRead(const uint8_t *p, const uint8_t *e){
        ptr=p;
        end=e;
        bit_index=7;
        stop = false;
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

    void printSome(){
        for(int i=0; i<30; i++){
            if(i%6 == 0) cout << endl;
            print_num_of_bits((*ptr),8); cout << " ";
           
            ptr++;

        }
        
    }

    void getNextChar(){
        bit_index = 7;
        ptr++;
        if(ptr == end) stop=true;
        act_char=(*ptr);
    }

    void print_num_of_bits(int num, int k){
        for(int i=k-1; i >= 0; i--){
            if(num & (1<<i)){
                cout << "1";
            }
            else{
                cout << "0";
            }
        }
        
    }



};

#endif