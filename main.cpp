/*
    Implementacia kompresie pomocou Huffmanovho kódovania
    Autor: Martin Matějka <xmatej55@vutbr.cz>
    KKO 2022/23
*/

#include <iostream>
#include <sys/types.h>

// CLASSES
#include "argparse.cpp"
#include "static.cpp"
#include "adaptive.cpp"

int read_compressMode(ArgParser *args){
    std::string filename = args->input_file;
    fstream file(filename, ios::in | ios::out | ios::binary);

    file.seekg(4);
    char x;
    file.read((&x), 1);
    file.close();
    if(x & 1){
        return COMPRESS_ADAPTIVE;
    }
    
    return COMPRESS_STATIC;



}

int main(int argc, char** argv){
    ArgParser args = ArgParser(argc,argv);
    
    if(args.i_flag == false || args.o_flag == false){
        cout << "You need to specify -i input and -o output file"<<endl;
        return 0;
    }
    if(args.compress_mode == false && args.decompress_mode == false){
        cout << "You need to specify -c / -d mode"<<endl;
        return 0;
    }

    if(args.compress_mode && args.image_width == 0) {
        cout << "You need to specitfy -w width" <<endl;
        return 0;
    }
    
    HuffStatic huff_static = HuffStatic(&args);
    HuffAdaptive huff_adaptive = HuffAdaptive(&args);

    if(args.decompress_mode){
        
        if( read_compressMode(&args) == COMPRESS_ADAPTIVE){
            huff_adaptive.decode_input();
        }
        else{
            huff_static.decode_input();
        }
        
    }else{
        if(args.adaptive_scanning){
            huff_adaptive.encode_input();
        }
        else{
            huff_static.encode_input();
        }
    }

    return 0;
}