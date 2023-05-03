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
#include "BitRead.cpp"

#include "defines.h"

#ifndef HUFF_STATIC
#define HUFF_STATIC

using namespace std;

class HuffStatic {

    public:

    std::vector<uint8_t> data;
    std::vector<std::vector<uint8_t>> blocks;
    
    std::map<uint8_t,uint32_t> frequencies;
    std::map<uint8_t,uint32_t> huff_codes;
    std::vector<int> bitlen;
    uint32_t bytes_loaded;

    uint32_t width;
    uint32_t height;
    char *f_inputname; 
    char *f_outputname; 

    bool error;
    int err_code;

    ArgParser *arguments;

    HuffStatic(ArgParser args){
        f_inputname = args.input_file;
        f_outputname = args.output_file;
        width = args.image_width;
        arguments = &args;
        bitlen.resize(256);
        error=false;
    }

    int ReadFileData(){
        std::string filename = f_inputname;

        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            error = true;
            err_code = FILE_LD_ERR;
            return FILE_LD_ERR;
        }

        std::streampos size = file.tellg();
        data.resize(size);
        file.seekg(0, std::ios::beg);

        if(arguments->compress_mode){
            if (size % width != 0) {
                std::cerr << "Invalid width: " << width << std::endl;
                error = true;
                err_code = INVALID_WIDTH_ERR;
                return INVALID_WIDTH_ERR;
            }
        
            height = size / width;
            std::cout << "Loaded image with width " << width << " and height " << height << std::endl; 
        }

        return 0;
    }

    void count_freq(){
        const uint8_t *ptr=data.data();
        const uint8_t *end = data.data() + data.size();
        while(ptr != end){
            frequencies[*ptr]++;
            ptr++;
        }
    }

    void write_map(){
        for (auto const& x : frequencies)
        {
            std::cout << int (x.first)  // string (key)
                    << ':' 
                    << x.second // string's value 
                    << std::endl;
        }
    }

    
    void split_image_into_blocks(int block_size) {
        int num_blocks_x = width / block_size;
        int num_blocks_y = height / block_size;
        const uint8_t *fdata = data.data();
        
        for (int by = 0; by < num_blocks_y; by++) {
            for (int bx = 0; bx < num_blocks_x; bx++) {
                std::vector<uint8_t> block(block_size * block_size);
                for (int y = 0; y < block_size; y++) {
                    for (int x = 0; x < block_size; x++) {
                        int index = ((by * block_size + y) * width + bx * block_size + x);
                        block[y * block_size + x] = fdata[index];
                    }
                }
                blocks.push_back(block);
            }
        }
    }

    void HClen() {
        std::vector<int> freq;
        std::vector<int> hr(256*2,0);

        for(uint8_t i=0; i<255; i++){
            freq.push_back(frequencies[i]);
        }
        freq.push_back(frequencies[255]);

        //cout << "Freq:" << endl;
        //print_int_vector(freq);

        int m = 256;
        std::vector<std::pair<int, int>> h;

        // init heap
        for (int i = 0; i < m; i++) {
            h.emplace_back(freq[i], m + i);
        }
        std::make_heap(h.begin(), h.end(), greaters());

        while (m>1){  
            pop_heap(h.begin(),h.end(), greaters());
            std::pair<int,int> a = h.back();
            h.pop_back();

            pop_heap(h.begin(),h.end(),greaters());
            std::pair<int,int> b = h.back();
            h.pop_back();

            m--;
            hr[a.second] = m; 
            hr[b.second] = m;

            a.first += b.first;
            a.second = m;
            h.push_back(a);
            push_heap(h.begin(),h.end(),greaters());
        }

        for(int i=0; i<256; i++){
            int j = hr[256+i];
            int l = 1;
            while(j>1){
                j=hr[j];
                l+=1;
            }
            bitlen[i] = l;
        }
        
        //cout << "Lens:" << endl;
        //print_int_vector(bitlen);
        return;    
    }


    void huff_codes_gen(){
        // vector of val/len 
        std::vector<std::pair<int,int>> vals;

        for(int i=0; i<256; i++){
            std::pair<int,int> a;
            a.first = i; a.second= bitlen[i];
            vals.push_back(a);
        }
        sort(vals.begin(), vals.end(), compareInterval);
        //cout<<endl;
        int delta_l;
        int last_len = vals.front().second;
        int last_num = 0;
        
        huff_codes[vals.front().first]=(uint32_t) 0;

        for (auto it = (begin(vals)+1); it != end (vals); ++it) {
            delta_l = last_len - it->second;
            last_len = it->second;
            int curr_num = (last_num+1)>>delta_l;
            
            last_num=curr_num;
            huff_codes[it->first]=(uint32_t) curr_num;
        }

    }


    void writing_file(){
        ofstream fout;

        fout.open(f_outputname, ios::binary | ios::out);
        
        if (!fout.is_open()) {
            std::cerr << "Failed to open file: " << f_outputname << std::endl;
            error = true;
            err_code = FILE_LD_ERR;
            return;
        }

        fout.write((char *) &width, sizeof(width));
        fout.write((char *) &height, sizeof(height));
        for(int i=0; i<256; i++){
            uint8_t b = (uint8_t ) bitlen[i];
            fout.write((char *) &b, sizeof(b));
        }
        
        const uint8_t *ptr = data.data();
        const uint8_t *end = data.data() + data.size();
        
        Buffer buffer;
        buffer.data.t64 = 0;
        buffer.valid_bits = 0;
        
        while(ptr != end){
            int w_len = bitlen[*ptr];
            uint32_t loaded = huff_codes[*ptr];

            //cout << "Loading " << (int) loaded << " of len "<< w_len<<endl;
            buffer.valid_bits += w_len;
            buffer.data.t64 = buffer.data.t64 << (uint64_t) w_len;
            //cout<< "Before: "; print_bin(buffer.data.t64,64);
            buffer.data.t64 = buffer.data.t64 | (uint64_t) loaded;
            //cout << "After : "; print_bin(buffer.data.t64,64);

            while(buffer.valid_bits >= 8){
                uint64_t tmp=0;
                int tmp_bits=0;

                if(buffer.valid_bits>8){
                    tmp_bits = buffer.valid_bits-8;
                    tmp = buffer.data.t64 & ((1U << tmp_bits) - 1U);
                    buffer.data.t64 = (buffer.data.t64 >> tmp_bits);
                }
                //cout << "Write:"; print_bin( (uint64_t) buffer.data.t8,8);
                fout.write((char *) &(buffer.data.t8), 1);

                buffer.valid_bits -= 8;
                if(tmp_bits > 0){
                    buffer.data.t64 = tmp;
                }
                else{
                    buffer.data.t64 = buffer.data.t64 >> 8;
                }
                
            }
            //cout << (int) buffer.valid_bits<< endl;
            ptr++;
        }


        fout.close();

    }
    uint8_t *decode_header(uint8_t *ptr){
        width = *((uint32_t *) ptr); ptr+=sizeof(uint32_t);
        height = *((uint32_t *) ptr); ptr+=sizeof(uint32_t);
        
        for(int i=0; i<256;i++){
            bitlen[i] = *((uint8_t *) ptr);
            ptr+=sizeof(uint8_t);
        }
        return ptr;
    }

    
    void encode_input(){
        ReadFileData();
        split_image_into_blocks(4);
        count_freq();
        HClen();
        huff_codes_gen();
        writing_file();

        
    }

    void decode_input(){
        ReadFileData();
        ERR_CHCK;

        const uint8_t *ptr = data.data();
        decode_header(data.data());
        const uint8_t *end = data.data() + data.size();

        cout << "Width :" << width<<endl;
        cout << "Heigth:" << height<<endl;
        
        huff_codes_gen();
  
        //vector[len] = pair < code, value
        std::vector< std::vector < std::pair<uint32_t,uint8_t>>> lenmaps(256);
        for(int i=0; i<256;i++){
            int len = bitlen[i];
            std::pair<uint32_t,uint8_t> pair;
            pair.first = huff_codes[i]; pair.second = (uint8_t) i;
            lenmaps[len].push_back(pair);
        } 

        ofstream fout;
        fout.open(f_outputname, ios::binary | ios::out);
       
        if (!fout.is_open()) {
            std::cerr << "Failed to open file: " << f_outputname << std::endl;
            error = true;
            err_code = FILE_LD_ERR;
            return;
        }
        
        BitRead bitread(ptr+HEADER_BYTE_SIZE, end);
        int putchar=0;
        uint64_t buffer=0;
        int f_len = 0;
        const int expected_size = (int) height * width;

       while(putchar < expected_size){
            f_len++;
            bool nextBit= bitread.getNextBit();
            buffer = (buffer<<1) | nextBit;
            
            //if(nextBit) cout << "1"; else cout <<"0";
            

            uint32_t finding = (uint32_t) buffer;
            
            auto it = std::find_if(lenmaps[f_len].begin(), lenmaps[f_len].end(),
                        [&](const std::pair<uint32_t,uint8_t>& p) {
                            return p.first == finding;
                        });
            
            if (it != lenmaps[f_len].end()){
                fout.write((char *) &(it->second), sizeof(it->second));
                f_len=0; 
                buffer=0;
                putchar++;
                //cout<< "Matching "<< (int) finding <<":"; print_bin(finding,f_len); 
                //cout << "With:    "; print_bin(it->first,32);

            }

            if( f_len == 32) {
                cout << "ERR TOO LONG WIHTOUT FINDING";
                error = true; err_code =ERR_NOT_MATCHED;
                return;
            }
            if(bitread.stop) { break; }

        }

        fout.close();    

    }


    ~HuffStatic(){
      
    }

    void print_huff_codes(){
        for( int i=0; i<256; i++){
            if(bitlen[i]>12) continue;
            cout << i << ":"; print_num_of_bits(huff_codes[i],bitlen[i]) ;cout << "\n"; 
        }
    }

    void print_bitlens(){
        for( int i=0; i<256; i++){
            
            cout << i << ":" <<(int)((uint8_t) bitlen[i]); cout << "\n"; 
            
        }
    }
        
    void print_int_vector(const std::vector<int>& v) {
        std::cout << "[";
        for (auto it = v.begin(); it != v.end(); ++it) {
            std::cout << *it;
            if (it != v.end() - 1) {
                std::cout << ",";
            }
        }
        std::cout << "]";
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

    void print_bin(uint64_t n,int k){
        for(int i=k-1; i >= 0; i--){
            if( n & ((uint64_t)1 << i) ){
                cout << "1";
            }
            else cout << "0";

            if(i%4 == 0) cout << " ";
        }
        cout << endl;
    }



};
#endif