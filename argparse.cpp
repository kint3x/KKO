
#include <iostream>
#include <cstring>

#ifndef _ARGPARSER
#define _ARGPARSER

class ArgParser {

    public:

    bool compress_mode = false;
    bool decompress_mode = false;
    bool model_activation = false;
    bool adaptive_scanning = false;
    char* input_file = nullptr;
    char* output_file = nullptr;
    int image_width = 0;

    bool i_flag = false;
    bool o_flag = false;

    ArgParser(int argc, char** argv){
        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-c") == 0) {
                compress_mode = true;
            }
            else if (strcmp(argv[i], "-d") == 0) {
                decompress_mode = true;
            }
            else if (strcmp(argv[i], "-m") == 0) {
                model_activation = true;
            }
            else if (strcmp(argv[i], "-a") == 0) {
                adaptive_scanning = true;
            }
            else if (strcmp(argv[i], "-i") == 0 && i+1 < argc) {
                i_flag = true;
                input_file = argv[++i];
            }
            else if (strcmp(argv[i], "-o") == 0 && i+1 < argc) {
                o_flag = true;
                output_file = argv[++i];
            }
            else if (strcmp(argv[i], "-w") == 0 && i+1 < argc) {
                image_width = std::stoi(argv[++i]);
            }
            else if (strcmp(argv[i], "-h") == 0) {
                std::cout << "-c: enable compression mode" << std::endl;
                std::cout << "-d: enable decompression mode" << std::endl;
                std::cout << "-m: enable model activation for preprocessing input data" << std::endl;
                std::cout << "-a: enable adaptive scanning mode for image processing" << std::endl;
                std::cout << "-i <ifile>: specify input file name for compression or decompression" << std::endl;
                std::cout << "-o <ofile>: specify output file name for compression or decompression" << std::endl;
                std::cout << "-w <width_value>: specify image width (required for compression)" << std::endl;
                std::cout << "-h: display help message" << std::endl;
            }
            else {
                std::cerr << "Error: unknown option " << argv[i] << std::endl;
            }
        }
    }
};

#endif