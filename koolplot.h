/**
 * Header file to the "koolplot" library
 * 
 * Provides basic, but also very simple-to-use 2D data plotting for
 * C and C++ programs using the WinBGIm graphics library.
 *
 * Example:
 * #include <koolplot.h>
 * int main()
 * {
 * 		Plotdata x(0, 360), y = sin(x * M_PI / 180);
 *		plot(x, y);
 *		return 0;
 * }
 *
 * The "plot" function opens a window and displays the graph. The window
 * is closed when any key is pressed.
 *
 * Author: 	jlk
 * Version:	1.1
 * Date:	July 2005
 */
 
#pragma once

#include "Plotstream.h"

/** Plot x..y data, specify label */
void plot(const Plotdata&x,const Plotdata&y,const char*label=0);

/** Plot x..y data, specify curve colour, then label */
void plot(const Plotdata&x,const Plotdata&y,Color colour,const char*label=0); 

/** Plot x..y data, specify label and curve colour */
inline void plot(const Plotdata&x, const Plotdata&y, const char*l, int c) {plot(x,y,c,l);}


/** Plot f(x) function, specify label (use together with f(x) function) */
void plot(const Plotdata&x,Func&f,const char*label=0); 

/** Plot f(x) function, specify curve colour, then label (use together with f(x) function) */
void plot(const Plotdata&x,Func&f,Color colour,const char *label=0); 

/** Plot f(x) function, specify label and curve colour (use together with f(x) function) */
inline void plot(const Plotdata&x,Func&f,const char *l,Color c) {plot(x,f,c,l);}

/**
 * Installs a user-defined unary function of x as f(x)
 * example: Plotdata x(-6, 6);
 *			f(x) = sinc;	// if sinc is a function defined by the user.
 *			plot(x, f(x));	// Plots the graph of sinc(x) from -6 to 6
 */
Func&f(Plotdata&x);

/**
 * Installs a user-defined binary function of x as f2(x)
 * example: Plotdata x(-270, 270);
 *			f2(x) = tanLessThan; // tanLessThan is a user-defined binary function.
 */
BinFunc&f2(Plotdata&x);

/**
 * Calculates a user-defined binary function of x as y = f2(x, double_value)
 * example: Plotdata x(-270, 270);
 *			f2(x) = tanLessThan; //  tanLessThan -user-defined binary function.
 *			plot(x, f2(x, 10));	 // Plots the graph of tanLessThan(x, 10)
 */
Plotdata f2(const Plotdata&x, float_t op2);

/**
 * Adds next point to plotting data
 * -You add x and y coordinates in 2 separate Plotdatas then call plot(). 
 * plot() will draw the curve joining all the data points. 
 * 
 * @param pd  the Plotdata to add the point to
 * @param val value of the point to add
 */

void point(Plotdata&pd, float_t val);

/**
 * Adds next point to plotting data
 *  
 * Same as above, but will take both axes and coordinates in one call. 
 * 
 * @param xAxis the x Plotdata to add the point to
 * @param xCoord value of the x coordinate to add
 * @param yAxis the y Plotdata to add the point to
 * @param yCoord value of the y coordinate to add
 */
void point(Plotdata&xAxis, Plotdata&yAxis,
           float_t xCoord, float_t yCoord);

/**
 * Insert array of data points into plotting data
 *  
 * @param axis the  Plotdata to add the points to
 * @param array The array of points to insert
 * @param numToInsert the number of points to add from the array.
 */
void insert(Plotdata& axis, const float_t array[], int numToInsert);

/** Sets the bottom left corner of the graph axes 
  * xmin and ymin should be less than or equal to any coordinate
  * on the graph.
  */ 
void axesBotLeft(Plotdata& x, Plotdata& y, 
                 float_t xmin, float_t ymin);

/** Sets the top right corner of the graph axes
  * xmax and ymax should be larger than or equal to any coordinate
  * on the graph.
  */ 
void axesTopRight(Plotdata& x, Plotdata& y, float_t xmax, float_t ymax);

/**
 * Clear previous data from an axis.
 * @param the axis to clear data from
 */
void clear(Plotdata& pd);

/**
 * Breaks the current plot data at this point.
 *
 * Later, when plotting, the last point of the data previously 
 * entered will not be joined to the first point of the next data.
 * This allows plotting more than one function on a single graph
 * using the "point" method of data entry.
 * @param xAxis the x data to break
 * @param yAxis the y data to break
 */
// void breakplot(Plotdata& x, Plotdata& y);

/**
 * Print plotting data on axis "anAxis" to standard output (for debugging)
 */
//void printPlotdata(const Plotdata& anAxis);


/**********************************************************************/
/*________ Maths functions that may be used to define functions ______*/

/*
 * Return new data, the sine of the original data
 * @param pd the original Plotdata
 */
Plotdata sin(const Plotdata& pd);

/**
 * Return new data, the cosine of the original data
 * @param pd the original Plotdata
 */
Plotdata cos(const Plotdata& pd);

/**
 * Return new data, the tan of the original data
 * @param pd the original Plotdata
 */
Plotdata tan(const Plotdata& pd);

/**
 * Return new data, the asin of the original data
 * @param pd the original Plotdata
 */
Plotdata asin(const Plotdata& pd);

/**
 * Return new data, the acos of the original data
 * @param pd the original Plotdata
 */
Plotdata acos(const Plotdata& pd);


/**
 * Return new data, the atan of the original data
 * @param pd the original Plotdata
 */
Plotdata atan(const Plotdata& pd);

/**
 * Return new data, the hyperbolic sine of the original data
 * @param pd the original Plotdata
 */
Plotdata sinh(const Plotdata& pd);

/**
 * Return new data, the hyperbolic cosine of the original data
 * @param pd the original Plotdata
 */
Plotdata cosh(const Plotdata& pd);

/**
 * Return new data, the hyperbolic tan of the original data
 * @param pd the original Plotdata
 */
Plotdata tanh(const Plotdata& pd);

/**
 * Return new data, the square root of the original data
 * @param pd the original Plotdata
 */
Plotdata sqrt(const Plotdata& pd);

/**
 * Return new data, the absolute value of the original data
 * @param pd the original Plotdata
 */
Plotdata fabs(const Plotdata& pd);

/**
 * Return new data, the natural logarithm of the original data
 * @param pd the original Plotdata
 */
Plotdata log(const Plotdata& pd);

/**
 * Return new data, the log (base 10)l of the original data
 * @param pd the original Plotdata
 */
Plotdata log10(const Plotdata& pd);

/**
 * Return new data, the exponential of the original data
 * @param pd the original Plotdata
 */
Plotdata exp(const Plotdata& pd);

/**
 * Return new data, the power "exp" of the original data
 * @param pd  the original Plotdata
 * @param exp value of the exponent to raise the data to.
 */
Plotdata pow(const Plotdata& pd, float_t exp);

