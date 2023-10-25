#include <stdio.h>  // Standard input and output
#include <errno.h>  // Access to errno and Exxx macros
#include <stdint.h> // Extra fixed-width data types
#include <string.h> // String utilities
#include <err.h>    // Convenience functions for error reporting (non-standard)

static char const b64_alphabet[] =
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  "abcdefghijklmnopqrstuvwxyz"
  "0123456789"
  "+/";


  void data_doer(FILE* file){

    int wrap_count = 0;
    char* newline = "\n";

    for (;;) {                                          //INFINITE LOOP

        if(wrap_count >= 76){
          fwrite(newline, sizeof(char), 1, stdout);
          wrap_count = 0;
        }

        uint8_t input_bytes[3] = {0};
        size_t n_read;

        n_read = fread(input_bytes, sizeof(uint8_t), 3, file);

        if (n_read != 0) {

            if (n_read == 3)
            {
                int alph_ind[4];
                alph_ind[0] = input_bytes[0] >> 2;
                alph_ind[1] = (input_bytes[0] << 4 | input_bytes[1] >> 4) & 0x3Fu;
                alph_ind[2] = (input_bytes[1] << 2 | input_bytes[2] >> 6) & 0x3Fu;
                alph_ind[3] = input_bytes[2] & 0x3Fu;

                char output[4];
                output[0] = b64_alphabet[alph_ind[0]];
                output[1] = b64_alphabet[alph_ind[1]];
                output[2] = b64_alphabet[alph_ind[2]];
                output[3] = b64_alphabet[alph_ind[3]];

              fwrite(output, sizeof(char), 4, stdout);
              wrap_count += 4;
            }
            else if (n_read < 3) {

                if (n_read == 2){

                  int alph_ind[3];
                  alph_ind[0] = input_bytes[0] >> 2;
                  alph_ind[1] = (input_bytes[0] << 4 | input_bytes[1] >> 4) & 0x3Fu;
                  alph_ind[2] = input_bytes[1] << 2 & 0x3Cu;

                  char output[4];
                  output[0] = b64_alphabet[alph_ind[0]];
                  output[1] = b64_alphabet[alph_ind[1]];
                  output[2] = b64_alphabet[alph_ind[2]];
                  output[3] = '=';

                  fwrite(output, sizeof(char), 4, stdout);
                  wrap_count += 4;
                }
                else if (n_read == 1){

                  int alph_ind[2];
                  alph_ind[0] = input_bytes[0] >> 2;
                  alph_ind[1] = input_bytes[0] << 4 & 0x30u;

                  char output[4];
                  output[0] = b64_alphabet[alph_ind[0]];
                  output[1] = b64_alphabet[alph_ind[1]];
                  output[2] = '=';
                  output[3] = '=';

                  fwrite(output, sizeof(char), 4, stdout);
                  wrap_count += 4;
                }
              }
          }
        if (feof(file) && (wrap_count > 0)) {
          fwrite(newline, sizeof(char), 1, stdout);
          break; // End of file
        }
        else if(feof(file) && (wrap_count == 0)) {
          break; // End of file
        }
        if (ferror(file)) {
            err(1, "Read error"); // Read error
        }
    /*
    if (... != stdin) fclose(...); // close opened files;
    // any other cleanup tasks?
    */
    }
    fclose(file);
}



int main(int argc, char *argv[])
{

    if (argc > 2) {
        //fprintf(stderr, "Usage: %s [FILE]\n", argv[0]);
        errx(1, "Too many arguments");
        }

    else if (argc == 2 && strcmp(argv[1], "-")) {
        FILE* file = fopen(argv[1], "r");          // CREATING A FILE STREAM

        if (file == NULL) {                        // FILE NAME DOES NOT EXIST ERROR
            err(1, "Failed to open the file (Specified file does not exist)"); // Read error
            return 1;
        }
        data_doer(file);
    }
    else {  // CREATING A FILE STREAM
        data_doer(stdin);
    }
}

//0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ
