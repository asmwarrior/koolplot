/**
 * Class Plotdata
 *
 * Stores data for one 2D plotting axis
 *
 * Provides operators and mathematical functions to map a Plotdata
 * object to another using unary and binary functions.
 *
 * Provides insertion and extraction of special commands into and from
 * Plotdata pairs as static operations.
 * Available commands are:
 *      colourChange (to change the foreground colour of graphs)
 *      singlePoint  (to draw a single point of data as a special marker)
 *
 * Author: 	jlk
 * Version:	1.1
 * Date:	July 2005
 */

#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include <limits>

// BEGIN Windows-specific, MSVC2008 specific
#include <windows.h>
typedef COLORREF Color;	// central switch between GDI and GDIplus
#define COLOR(r,g,b) RGB(r,g,b)

const Color BLACK	=RGB( 0, 0, 0 );
const Color BLUE	=RGB( 0, 0, 128);
const Color GREEN	=RGB( 0, 128, 0 );
const Color CYAN	=RGB( 0, 128, 128 );
const Color RED		=RGB( 128, 0, 0 );
const Color MAGENTA	=RGB( 128, 0, 128 );
const Color BROWN	=RGB( 128, 128, 0 );
const Color LIGHTGRAY	=RGB( 192, 192, 192 );
const Color DARKGRAY	=RGB( 128, 128, 128 );
const Color LIGHTBLUE	=RGB( 128, 128, 255 );
const Color LIGHTGREEN	=RGB( 128, 255, 128 );
const Color LIGHTCYAN	=RGB( 128, 255, 255 );
const Color LIGHTRED	=RGB( 255, 128, 128 );
const Color LIGHTMAGENTA=RGB( 255, 128, 255 );
const Color YELLOW	=RGB( 255, 255, 0 );
const Color WHITE	=RGB( 255, 255, 255 );

const Color CRIMSON		=COLOR(220, 20, 60);
const Color CHOCOLATE		=COLOR(210, 105, 30);
const Color GOLDENROD		=COLOR(218, 165, 32);
const Color DARKORANGE		=COLOR(255, 140, 0);
const Color REDRED		=COLOR(255, 0, 0);
const Color DARKSLATEGRAY	=COLOR(47, 79, 79);
const Color DEEPPINK		=COLOR(255, 20, 147);
const Color TURQUOISE		=COLOR(64, 224, 208);
const Color DARKVIOLET		=COLOR(148, 0, 211);
const Color BEIGE		=COLOR(245, 245, 220);
const Color MEDIUMSEAGREEN	=COLOR(60, 179, 113);
const Color LIMEGREEN		=COLOR(50, 205, 50);
const Color DARKGREEN		=COLOR(0, 100, 0);
const Color MEDIUMBLUE		=COLOR(0, 0, 205);
const Color BLUEBLUE		=COLOR(0, 0, 255);
const Color DODGERBLUE		=COLOR(30, 144, 255);

/* -------------------------------------------------------- */
//typedef double float_t;

//namespace std{
// bool isfinite(float_t);
// float_t round(float_t);
// float_t remquo(float_t,float_t,int*);
// float_t trunc(float_t);
//}
// END Windows-specific, MSVC2008 specific

using namespace std;

// Unary function pointer type accepted by Plotdata (double parameter)
typedef float_t (*Func)(float_t operand);

// Binary function pointer type accepted by Plotdata
typedef float_t (*BinFunc)(float_t operand1, float_t operand2);

/** When true defines a range to be logarithmic */
typedef bool LogSpace;

/** Define data iterator */
typedef vector<float_t>::const_iterator  dataIterator;

/** Remove case sensitivity for Plotdata class */
class Plotdata;

/** #define Plotdata & as a type (avoid using confusing '&' symbols in C) */

/** Number of points in a plot */
enum Grain
{
	EXTRA_FINE = 901,
	FINE = 501,
	MEDIUM = 301,
	COARSE = 201,
	GROSS = 101
};

const float_t NOPLOT = numeric_limits<float_t>::quiet_NaN(); // Any point of this value will not be plotted

//const int NOCOLOR      = -1;
//const int RESETCOLOR   = -2;

class Plotdata{
    // --------------------------------------------------------------
    //     OPERATORS

 friend ostream & operator << (ostream & out, const Plotdata & pd);
 friend istream & operator >> (istream & in, Plotdata & pd);

public:

 Plotdata operator +  (const Plotdata &) const;
 Plotdata operator +  (float_t) const;
 Plotdata operator -  (const Plotdata &) const;
 Plotdata operator -  (float_t) const;
 Plotdata operator *  (const Plotdata &) const;
 Plotdata operator *  (float_t) const;
 Plotdata operator /  (const Plotdata &) const;
 Plotdata operator /  (float_t) const;
 Plotdata operator ^  (float_t) const;
 Plotdata operator -  ( ) const;
 Plotdata & operator << (const Plotdata &); // concatenate
 Plotdata & operator << (float_t); // add a double to the data

    // --------------------------------------------------------------
    // Constructors
 inline Plotdata(): data(MEDIUM), userFunction(0),userBinFunction(0){}
 Plotdata(float_t min, float_t max, Grain grain=MEDIUM);
 Plotdata(const float_t*array, int dataSize);
 inline Plotdata(size_t s): data(s), userFunction(0),userBinFunction(0){}
 inline Plotdata(vector<float_t> d): data(d),userFunction(0), userBinFunction(0){};
    // Member Functions
 void insert(const float_t array[], int dataSize);
 inline size_t size() const {return data.size();}
 inline void point(float_t p) {data.push_back(p);}
 void plotRange(float_t min,float_t max,size_t numPoints,bool isLog = false);

 inline void clear() {data.clear();}
 inline const vector<float_t> & getData() const{return data;}
 inline Func & userfunc() { return userFunction; }
 inline BinFunc & userBinfunc() { return userBinFunction; }

 Plotdata doFunc(Func aFunction) const;
 Plotdata doBinFunc(BinFunc aFunction, float_t operand2) const;
 Plotdata doBinFunc(float_t operand2) const;

// A 揜ange?class to find min/max of data and to test insideness
 struct Range{
  float_t min,max;
// Initializers, looking like constructors, to setup an object
  void init(float_t a,float_t b) {min=a; max=b;}
  void init(float_t a) {init(a,a);}
  void init() {init(numeric_limits<float_t>::infinity(),-numeric_limits<float_t>::infinity());}
// Expanders, expanding a range by a data item or a range
  bool expand(const Range&r) {return expand(r.min,r.max);}
  bool expand(float_t a) {return expand(a,a);}
  bool expand(float_t a, float_t b) {
   bool r=false;
   if (min>a) {min=a; r=true;}
   if (max<b) {max=b; r=true;}
   return r;
  }
// Get difference between min and max
  float_t delta() {return max-min;}
// check whether item is between min and max (both inclusive)
  bool inside(float_t x) const {return min<=x && x<=max;}
 };

    // --------------------------------------------------------------
    // Class (static) functions
    // Retrieves the range of each of x and y in a Plotdata pair
    // Values corresponding to a NAN in the other Plotdata are not included
 static void rangeXY(const Plotdata&, const Plotdata&,
                    Range&, Range&);

 vector<float_t> data;
private:
 Func userFunction;		// Any user-designed unary function
 BinFunc userBinFunction;	// Any user-designed binary function
};

Plotdata operator + (float_t op1, const Plotdata & pd);
Plotdata operator - (float_t op1, const Plotdata & pd);
Plotdata operator * (float_t op1, const Plotdata & pd);
Plotdata operator / (float_t op1, const Plotdata & pd);
