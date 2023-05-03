#include <iostream>
#include <sys/types.h>

// CLASSES
#include "argparse.cpp"
#include "static.cpp"


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

    if(args.compress_mode) huff_static.encode_input();
    if(args.decompress_mode) huff_static.decode_input();
    //huff_static.print_huff_codes();
   // if(huff_static.error) cout << "ENDED WITH ERR "<< huff_static.err_code<<endl; 
    return 0;
}