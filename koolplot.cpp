/*
 * Implementation of the koolplot 2D plotting library
 *
 * Author: 	jlk
 * Version:	1.1
 * Date:	July 2005
 * 
 * This file is in the public domain and can be used without
 * any restriction.
 */
#include <cmath>

#include "Koolplot.h"

// ----------- more function prototypes -------  ----------------------
// May be placed in the header file for user program access if they are
// deemed useful at some stage.

/*
 * Defines a plot data range with data spread along the range
 * If the range is in logarithmic space, min and max must have
 * the same sign and must not be 0.
 *
 * @param min lowest point in the range
 * @param max highest point in the range
 * @param numpoints number of points in the range
 *                  (or one of EXTRA_FINE, FINE, COARSE, or GROSS).
 * @param space LOG for logarithmic space, LIN for linear space 
 */
void plotRange(Plotdata&pd,float_t min,float_t max,size_t numpoints=FINE,bool logspace=false);

// -----------------------------------------------------------------
// --------------------- Start Code --------------------------------

/*
 * Add a point to a plot data
 * @param pd  the Plotdata to add the point to
 * @param val value of the point to add
 */
void point(Plotdata&pd, float_t val) {pd.point(val);}

/*
 * Adds next point to plotting data
 *  
 * Same as above, but will take both axes and coordinates in one call. 
 * 
 * @param xAxis the x Plotdata to add the point to
 * @param xCoord value of the x coordinate to add
 * @param yAxis the y Plotdata to add the point to
 * @param yCoord value of the y coordinate to add
 */
void point(Plotdata&xAxis, Plotdata&yAxis,float_t xCoord, float_t yCoord) {
 xAxis.point(xCoord);
 yAxis.point(yCoord);
}

/*
 * Insert array of data points into plotting data
 *  
 * @param axis the  plotdata to add the points to
 * @param array The array of points to insert
 * @param numToInsert the number of points to add from the array.
 */
void insert(Plotdata& axis, const float_t array[], int numToInsert) {
 axis.insert(array, numToInsert);
}

/*
 * Clear data from a Plotdata object
 */
void clear(Plotdata & pd)
{
	pd.clear();
}
/*
 * Plot x..y data, specify label
 */
void plot(const Plotdata&x,const Plotdata&y,const char*label) {
 Plotstream ps(label);
 ps.plot(x, y);
} 

/*
 * Plot x..y data, specify colour, then label.
 */
void plot(const Plotdata&x,const Plotdata&y,Color colour,const char*label) {
 Plotstream ps(label);
 ps.plot(x, y, colour);
} 

/**
 * Plot f(x) function, specify label (use together with f(x) function)
 */
void plot(const Plotdata&x,Func&aFunction,const char*label) {
 if (!aFunction) return;
 Plotstream ps(label);
 ps.plot(x, x.doFunc(aFunction));
} 
/*
 * Plot f(x) function, specify curve colour
 */
void plot(const Plotdata&x,Func&aFunction,Color colour,const char*label) {
 if (!aFunction) return;
 Plotstream ps(label);
 ps.plot(x, x.doFunc(aFunction), colour);
} 

/**
 * Access a user-defined unary function of x as f(x)
 * example: Plotdata x(-6, 6);
 *			f(x) = sinc;	// if sinc is a function defined by the user.
 *			plot(x, f(x));	// Plots the graph of sinc(x) from -6 to 6
 */
Func&f(Plotdata&x) {
 return x.userfunc();
}

/*
 * Access a user-defined binary function of x as f2(x)
 * example: Plotdata x(-270, 270);
 *			f2(x) = tanLessThan; // Install user-defined binary function.
 */
BinFunc&f2(Plotdata&x) {
 return x.userBinfunc();
}

/*
 * Calculates a user-defined binary function of x as y = f2(x, double_value)
 * example: Plotdata x(-270, 270);
 *			f2(x) = tanLessThan; 
 *			plot(x, f2(x, 10));	 // Plots the graph of tanLessThan(x, 10)
 */
Plotdata f2(const Plotdata&x, float_t op2) {
 return x.doBinFunc(op2);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~ MATH.H FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * Return new data, the sine of the original data
 */
Plotdata sin(const Plotdata&pd) {return pd.doFunc(sin);}

/*
 * Return new data, the cosine of the original data
 */
Plotdata cos(const Plotdata&pd) {return pd.doFunc(cos);}

/*
 * Return new data, the tan of the original data
 */
Plotdata tan(const Plotdata&pd) {return pd.doFunc(tan);}

/*
 * Return new data, the asin of the original data
 */
Plotdata asin(const Plotdata&pd) {return pd.doFunc(asin);}

/*
 * Return new data, the acos of the original data
 */
Plotdata acos(const Plotdata&pd) {return pd.doFunc(acos);}

/*
 * Return new data, the atan of original the data
 */
Plotdata atan(const Plotdata&pd) {return pd.doFunc(atan);}

/*
 * Return new data, the hyperbolic sine of the original data
 */
Plotdata sinh(const Plotdata&pd) {return pd.doFunc(sinh);}

/*
 * Return new data, the hyperbolic cosine of the original data
 */
Plotdata cosh(const Plotdata&pd) {return pd.doFunc(cosh);}

/*
 * Return new data, the hyperbolic tan of the original data
 */
Plotdata tanh(const Plotdata&pd) {return pd.doFunc(tanh);}

/*
 * Return new data, the square root of the original data
 */
Plotdata sqrt(const Plotdata&pd) {return pd.doFunc(sqrt);}

/*
 * Return new data, the absolute value of the original data
 */
Plotdata fabs(const Plotdata&pd) {return pd.doFunc(fabs);}

/*
 * Return new data, the natural logarithm of the original data
 */
Plotdata log(const Plotdata&pd) {return pd.doFunc(log);}

/*
 * Return new data, the log (base 10)l of the original data
 */
Plotdata log10(const Plotdata&pd) {return pd.doFunc(log10);}

/*
 * Return new data, the exponential of the original data
 */
Plotdata exp(const Plotdata&pd) {return pd.doFunc(exp);}

/*
 * Return new data, the power "exp" of the original data
 */
Plotdata pow(const Plotdata&pd, float_t exp) {return pd.doBinFunc(pow,exp);}
