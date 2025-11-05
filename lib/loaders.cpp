#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "loaders.h"


// Loads the content of a GLSL Shader file into a char* variable
char* get_shader_content(const char* fileName)
{
    long size = 0;

    /* Read File to get size */
    FILE *fp = fopen(fileName, "rb");
    if(fp == NULL) {
        printf("Fatal Error: Failed to load shader at path: %s\n", fileName);
        exit(1);
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp)+1;
    fclose(fp);

    /* Read File for Content */
    fp = fopen(fileName, "r");
    const auto shaderContent = static_cast<char *>(memset(malloc(size), '\0', size));
    fread(shaderContent, 1, size-1, fp);
    fclose(fp);

    return shaderContent;
}

// reads something that is not really a csv, because it has no line endings
FloatData read_csv(const char* filename) {

  FILE* fptr = fopen(filename, "r");

  int read = 0;
  int number_of_floats = 0;
  while ((read = getc(fptr)) != EOF) {
    if (read == ',') {
      number_of_floats++;
      }
    }
    
  number_of_floats++;
  const size_t number = number_of_floats;
  auto* floats = (float *)malloc(sizeof(float)*number);

  size_t float_cursor = 0;
  char number_string[10] = { 0 }; // it wont be longer than this
  size_t digit_cursor = 0;
  FILE* fptr2 = fopen(filename, "r");

  while ((read = getc(fptr2)) != EOF) {
    
    if (read != ',') {
        // add chars to number string
        const char t = read;
        number_string[digit_cursor] = t;
        digit_cursor++;
    } else {
        // convert the number string into a float
        floats[float_cursor] = atof(number_string);
        // zero the number string
        memset(number_string,0,strlen(number_string));
        // reset digit cusor
        digit_cursor = 0;
        // move onto the next float
        float_cursor++; 
    }
   } 

    // at EOF add the last float in
    floats[float_cursor] = atof(number_string);

    return (FloatData){
        .data = floats,
        .count = number
    };

}