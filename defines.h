#ifndef __DEFINES__
#define __DEFINES__

#include <sys/types.h>

#define FILE_LD_ERR 1
#define INVALID_WIDTH_ERR 2
#define ERR_NOT_MATCHED 3
#define ERR_BAD_SIZE 4
#define ERR_WR_ERROR 5

#define HEADER_SIZE_BYTES 261
#define HEADER_ADAPTIVE_SIZE_BYTES 5

#define COMPRESS_ADAPTIVE 1
#define COMPRESS_STATIC 0

#define ERR_CHECK(x) do { \
  int retval = (x); \
  if (retval != 0) { \
    fprintf(stderr, "Runtime error: %s returned %d at %s:%d", #x, retval, __FILE__, __LINE__); \
    return 0; \
  } \
} while (0)

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