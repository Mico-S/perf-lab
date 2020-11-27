//-*-c++-*-
#ifndef _Filter_h_
#define _Filter_h_

using namespace std;

class Filter {
  int divisor;
  int dim;
  int *data;

public:
  Filter(int _dim);
  int get(int r, int c){ return data[ r * dim + c ]; };
  void set(int r, int c, int value) { data[ r * dim + c ] = value; };

  int getDivisor() { return divisor; };
  void setDivisor(int value) { divisor = value; };

  int getSize() { return dim; };
  void info();
};

#endif
