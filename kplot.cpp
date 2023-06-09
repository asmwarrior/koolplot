/* Commented examples of graph drawing with
 * the koolplot library, version 1.1.
 * Date:	July 2005

The original one required a BGI (Borland Graphics Interface?) library to run.
I removed all dependencies as good as I could and changed to Win32 functions.

Unlike all other my projects, this relies on MSVCR90.DLL and MSVCP90.DLL,
due to heavy use of std::string and std::vector. It is built with MSVC2008.

Koolplot isn't cool, sorry. While it looks easy to use (looks like Matlab?),
it is almost impossible to integrate into existing Win32 projects.
Scale division and space distribution isn't really solved.
No support for multiple axes, and the support for multiple traces is weird.

I have removed the special-code color-change multi-trace option
and decided to allow multiple traces to be 揳dded?to one plot,
IMHO much more comprehensible.

heha, August 2019 

Changed to be compileable with good old MSVC6: No "for each".
Compiled with "ddk2003", special depency is now to MSVCP60.DLL.

heha, January 2020
 */
#define _USE_MATH_DEFINES
#include <cmath>
#include "koolplot.h"

const float_t DEG_TO_RAD = M_PI / 180;

/*
 * Sinc: user-defined unary function y = sin(PI + x) / (PI * x),
 * except for x = 0 when y = 1.
 * Sinc is an important function in science and engineering.
 */
float_t sinc(float_t x) {
	if (fabs(x) < 0.0001)
		return 1.0;
	else 
		return sin(M_PI * x) / (M_PI * x);
}

/*
 * tanLessThan: User-defined binary function.
 *
 * Returns tan(x) values below max.
 *
 * - Any value of tan(x) above "max" is returned as NOPLOT (a special 
 *   value known as NaN (not a number) in C).
 * - This is done because tan x grows very fast towards infinity (or minus
 *   infinity) at some values. Of course infinity cannot be plotted!
 * - Koolplot never attempts to draw NOPLOT values.
 */
float_t tanLessThan(float_t x, float_t max) {	 
 float_t y = tan(x * DEG_TO_RAD);
 if (fabs(y) > max) return NOPLOT; // This point will not be on the graph
 else return y;
}

// This is a console application.
// Every plot() creates an extra window with the plot,
// and waits for closing by clicking onto close system button or hitting Alt+F4.
// In opposite to original, the window is created with Windows default position
// and size, and its size can be changed.

int main() {	/* give range of X axis, then define Y as x^2 + 3x + 3 */
 Plotdata x(-5.0, 2.0),  y = x*x + 3*x + 3;
 plot(x, y); 	/* Plot the graph */

/**********************************************************/
/* Points may be entered one at a time into the axes data */
/* .. a "point-by-point" data entry method.               */

/* Erase previous data in axes X and Y so they can be re-used.*/	
 clear(x);
 clear(y); 
	
/* 
 * Enter each curve point as a coordinate pair using the Plotdata 
 * insertion operator "<<" ( or use the "point" function instead).
 */
 for (int i = -180 ; i <= 180; i++) {
  float_t y_value = i / 180.0 + cos(DEG_TO_RAD * i);
  x << i;
  y << y_value;
/* "<<" inserts a new value into the plot data.
 * You could instead use the point function, with
 * point(x, y, x_value, y_value); if you prefer.
 */
 }
	
/* Graph drawn in blue colour (Each data point is joined to
 * the preceding and following points, forming the curve.). 
 */
 plot(x, y, BLUE);
		

/**********************************************/	
/* Plot function y = sin(x) between 0 and 360 */
 x = Plotdata(0.0, 360.0); 
 y = sin(x * DEG_TO_RAD);
/* Give the function name to be printed as the window label */
 plot(x, y, CRIMSON, "sin(x)");


/*****************************************************************/
/* Plot user-defined unary function f(x) = sinc between -6 and 6 */
 x = Plotdata(-6.0, 6.0);
 f(x) = sinc;  	  /* sinc is defined at the top of this file. */
 plot(x, f(x), "sinc(x)");
	
/*****************************************************************/      
/* Plot user-defined binary function y = tanLessThan(x, max)     */
/* Read tanLessThan code to see how values > max are not plotted */
 x = Plotdata(-270.0, 270.0); 
 f2(x) = tanLessThan;
	/* Do not plot y values greater than 20 */
 plot(x, f2(x, 20), "tan(x)", REDRED);


/*********************************************************************/
/* Plot 2 functions on same graph (could be any number of functions) */
/* Define Y as two functions */
 {Plotstream ps;
  clear(x); /* Re-use old X, but change its range to -75..245 degrees  */
  x << Plotdata(-80.0, 255.0);
  y = f2(x, 3); /* First function */
  ps.addplot(x,y);
	/* Define second function (2cos(2x)) on same range "point-by-point"  */
  Plotdata z(0);
  for(int i = -80; i <= 255; i++) {
   z << 2 * cos(2 * i * DEG_TO_RAD);
  }
  ps.addplot(x,z,BLUEBLUE);
  ps.show("tan(x) and 2cos(2x)");
 }// invoke destructors
	
 {Plotstream ps;
  x = Plotdata(-315.0, 45.0);
  y = sin(x*DEG_TO_RAD);
  Plotdata z = cos(x*DEG_TO_RAD),
  p = sin(2*(x-45)*DEG_TO_RAD),
  q = cos(2*x*DEG_TO_RAD);
  ps.addplot(x,y,COLOR(0,160,0));
  ps.addplot(x,z,CRIMSON);
  ps.addplot(x,p,DARKORANGE);
  ps.addplot(x,q,BLUEBLUE);
  ps.show("sin(x) -green-, cos(x) -red-, sin(2x - 45) -orange-, cos(2x) -blue-");
 }// invoke destructors
}
