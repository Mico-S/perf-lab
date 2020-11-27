#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "Filter.h"

using namespace std;

#include "rdtsc.h"

//
// Forward declare the functions
//
Filter * readFilter(string filename);
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

class Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    for (int i=0; i < size; i++) {
      for (int j=0; j < size; j++) {
	int value;
	input >> value;
	filter -> set(i,j,value);
      }
    }
    return filter;
  } else {
    cerr << "Bad input in readFilter:" << filename << endl;
    exit(-1);
  }
}


double
applyFilter(class Filter *filter, cs1300bmp *input, cs1300bmp *output)
{

  long long cycStart, cycStop;

  cycStart = rdtscll();

  output -> width = input -> width;
  output -> height = input -> height;
  // Step 2 in ChangesMade.txt
  // Stored filter->getSize() and filter->getDivisor() inside of local variables so for loops
  // don't call functions every iteration (code motion)
  
  // int getSizeVar = filter -> getSize();
  int getDivisorVar = filter -> getDivisor();
    
  // Step 3 in ChangesMade.txt
  // Stored (input -> width) - 1 and (input -> height) - 1 inside of local variables so first 2
  // for loops don't have to calculate this everytime (code motion)
  int inputWidth = (input -> width) - 1;
  int inputHeight = (input -> height) - 1;
    
//   int outputColorArrElement = 0;
    
  // Step 6 in ChangesMade.txt
  // Stored the repeated value calls from "filter->get(i, j)" into a local array
  int localFilterGetArr[3][3];
  for(int i = 0; i < 3; i++) {
      for(int j = 0; j < 3; j++) {
          localFilterGetArr[i][j] = filter -> get(i, j);
      }
  }
  
  // Step 7 in ChangesMade.txt
  int outputColorArrElementPlane0;
  int outputColorArrElementPlane1;
  int outputColorArrElementPlane2;
  
  // Step 8 in ChangesMade.txt
  int localFilterGetArrElement;
  int rowplusiminus1;
  int colplusjminus1;
    
  int colplusj;
  int outputColorArrElementPlane0colj;
  int outputColorArrElementPlane1colj;
  int outputColorArrElementPlane2colj;
  int colplus1;

  // Step 5 in ChangesMade.txt
  // Swapped the row and col variables so that arrays are accessed in row-major
  // order like they are stored in memory
//   for(int col = 1; col < inputWidth; col = col + 1) {
//     for(int row = 1; row < inputHeight; row = row + 1) {
  #pragma omp parallel for
  for(short row = 1; row < inputHeight; row++) {
    for(short col = 1; col < inputWidth; col = col + 2) {
  // Step 7 in ChangesMade.txt
//       for(int plane = 0; plane < 3; plane++) {

    // Step 4 in ChangesMade.txt
    // Created local accumulator "outputColorArrElement". 
//         outputColorArrElement = output -> color[plane][row][col];
//         output -> color[plane][row][col] = 0;
//         outputColorArrElement = 0;
    // Step 7 in ChangesMade.txt
       outputColorArrElementPlane0 = 0;
       outputColorArrElementPlane1 = 0;
       outputColorArrElementPlane2 = 0;
        
    // Step 9 in ChangesMade.txt
       outputColorArrElementPlane0colj = 0;
       outputColorArrElementPlane1colj = 0;
       outputColorArrElementPlane2colj = 0;
          
    // Step 5 in ChangesMade.txt
    // Swapped the row and col (i and j) variables so that the arrays are accessed
    // in row-major order like they are stored in memory
//         for (int j = 0; j < getSizeVar; j++) {
//           for (int i = 0; i < getSizeVar; i++) {
        for (short i = 0; i < 3; i++) {
          // Step 8 in ChangesMade.txt
          rowplusiminus1 = row + i - 1;
          for (short j = 0; j < 3; j++) {	
            // Step 6 in ChangesMade.txt
//             outputColorArrElement = outputColorArrElement + (input -> color[plane][row + i - 1][col + j - 1] * filter -> get(i, j) );
//             outputColorArrElement = outputColorArrElement + (input -> color[plane][row + i - 1][col + j - 1] * localFilterGetArr[i][j] );
            // Step 8 in ChangesMade.txt
            localFilterGetArrElement = localFilterGetArr[i][j];
            colplusjminus1 = col + j - 1;
              
            colplusj = col + j;
            
            // Step 7 in ChangesMade.txt
//             outputColorArrElementPlane0 = outputColorArrElementPlane0 + (input -> color[0][row + i - 1][col + j - 1] * localFilterGetArrElement );
//             outputColorArrElementPlane1 = outputColorArrElementPlane1 + (input -> color[1][row + i - 1][col + j - 1] * localFilterGetArrElement );
//             outputColorArrElementPlane2 = outputColorArrElementPlane2 + (input -> color[2][row + i - 1][col + j - 1] * localFilterGetArrElement );
            outputColorArrElementPlane0 = outputColorArrElementPlane0 + (input -> color[0][rowplusiminus1][colplusjminus1] * localFilterGetArrElement );
            // Step 9 in ChangesMade.txt
            outputColorArrElementPlane0colj = outputColorArrElementPlane0colj + (input -> color[0][rowplusiminus1][colplusj] * localFilterGetArrElement );
            outputColorArrElementPlane1 = outputColorArrElementPlane1 + (input -> color[1][rowplusiminus1][colplusjminus1] * localFilterGetArrElement );
            // Step 9 in ChangesMade.txt
            outputColorArrElementPlane1colj = outputColorArrElementPlane1colj + (input -> color[1][rowplusiminus1][colplusj] * localFilterGetArrElement );
            outputColorArrElementPlane2 = outputColorArrElementPlane2 + (input -> color[2][rowplusiminus1][colplusjminus1] * localFilterGetArrElement );
            // Step 9 in ChangesMade.txt
            outputColorArrElementPlane2colj = outputColorArrElementPlane2colj + (input -> color[2][rowplusiminus1][colplusj] * localFilterGetArrElement );
          }
        }

//         outputColorArrElement = 	
//           outputColorArrElement / getDivisorVar;
        
        // Step 9 in ChangesMade.txt
        if(getDivisorVar != 1) {
            outputColorArrElementPlane0 = outputColorArrElementPlane0 / getDivisorVar;
            outputColorArrElementPlane1 = outputColorArrElementPlane1 / getDivisorVar;
            outputColorArrElementPlane2 = outputColorArrElementPlane2 / getDivisorVar;

            outputColorArrElementPlane0colj = outputColorArrElementPlane0colj / getDivisorVar;
            outputColorArrElementPlane1colj = outputColorArrElementPlane1colj / getDivisorVar;
            outputColorArrElementPlane2colj = outputColorArrElementPlane2colj / getDivisorVar;
        }

//         if ( outputColorArrElement  < 0 ) {
//           outputColorArrElement = 0;
//         }
        // Step 7 in ChangesMade.txt
        if ( outputColorArrElementPlane0  < 0 ) {
          outputColorArrElementPlane0 = 0;
        }
        if ( outputColorArrElementPlane1  < 0 ) {
          outputColorArrElementPlane1 = 0;
        }
        if ( outputColorArrElementPlane2  < 0 ) {
          outputColorArrElementPlane2 = 0;
        }
        
        if ( outputColorArrElementPlane0colj  < 0 ) {
          outputColorArrElementPlane0colj = 0;
        }
        if ( outputColorArrElementPlane1colj  < 0 ) {
          outputColorArrElementPlane1colj = 0;
        }
        if ( outputColorArrElementPlane2colj  < 0 ) {
          outputColorArrElementPlane2colj = 0;
        }

//         if ( outputColorArrElement  > 255 ) { 
//           outputColorArrElement = 255;
//         }
        // Step 7 in ChangesMade.txt
        if ( outputColorArrElementPlane0  > 255 ) { 
          outputColorArrElementPlane0 = 255;
        }
        if ( outputColorArrElementPlane1  > 255 ) { 
          outputColorArrElementPlane1 = 255;
        }
        if ( outputColorArrElementPlane2  > 255 ) { 
          outputColorArrElementPlane2 = 255;
        }
        
        if ( outputColorArrElementPlane0colj  > 255 ) {
          outputColorArrElementPlane0colj = 255;
        }
        if ( outputColorArrElementPlane1colj  > 255 ) {
          outputColorArrElementPlane1colj = 255;
        }
        if ( outputColorArrElementPlane2colj  > 255 ) {
          outputColorArrElementPlane2colj = 255;
        }
          
        colplus1 = col + 1;
//         output -> color[plane][row][col] = outputColorArrElement;
        // Step 7 in ChangesMade.txt
        output -> color[0][row][col] = outputColorArrElementPlane0;
        output -> color[0][row][colplus1] = outputColorArrElementPlane0colj;
        output -> color[1][row][col] = outputColorArrElementPlane1;
        output -> color[1][row][colplus1] = outputColorArrElementPlane1colj;
        output -> color[2][row][col] = outputColorArrElementPlane2;
        output -> color[2][row][colplus1] = outputColorArrElementPlane2colj;
//       }
    }
  }

  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}