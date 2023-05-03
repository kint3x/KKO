#ifndef __DEFINES__
#define __DEFINES__

#include <sys/types.h>

#define FILE_LD_ERR 1
#define INVALID_WIDTH_ERR 2
#define ERR_NOT_MATCHED 3

#define HEADER_BYTE_SIZE 264

   // Compares two intervals according to starting times.
    bool compareInterval(const std::pair<int,int> i1, const std::pair<int,int> i2)
    {
        if(i1.second == i2.second) return i1.first > i2.first;
        return (i1.second > i2.second);
    }

    // comparator function to make min heap
    struct greaters{
        bool operator()(const std::pair<int, int> a ,const std::pair<int, int> b) const{
            return a.first>b.first;
        }
    };

    union Data {
        uint64_t t64;
        uint32_t t32;
        uint8_t t8;
    };

    struct Buffer{
        Data data;
        int valid_bits;
    };

#define ERR_CHCK if(error){ return;}

#endif