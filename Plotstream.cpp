/* File: Plotstream.cxx
 *
 * Implementation class Plotstream
 * A plotstream opens a window displays a data plot, then closes
 * the window.
 *
 * left-clicking the mouse in the window displays the current plot coordinates of
 * the cursor.
 *
 * This file hides all the fiddly bits about plotting a function.
 *
 * Author: 	jlk
 * Version:	1.2
 * Date:	January 2006
 */
#include <ostream>
#include <iomanip>
#include <string>
#include <cfloat>

#include "Plotstream.h"
#include <sstream>
//#include "BGI_util.h"
#include <windowsx.h>

HWND Plotstream::wnd;
HDC Plotstream::dc;


// Constants used exclusively in this file
//static const int topx = 150; // top left corner of window
//static const int topy =  50;
static const int border_height	= 20;
static const int border_width	= 20;
static const int left_border	= 70;
static const int mark_length	= 4;
//static const int max_height		= 600;
//static const int min_height 	= 200;
static const int graph_color    = GREEN; // Default only, can be changed
static const int point_color    = graph_color; // will be XORed
static const int marker_color	= CYAN; // Will give red in XOR mode over white
static const float_t chouia		= 0.0001; // an invisible difference (in drawing terms)
static const float_t epsilon		= DBL_EPSILON * 1000; // Extremely small difference
//static const bool ERASE			= true;

// Mouse events variables used exclusively in this file
//static bool left_clicked = false;
static int cursorX;
static int cursorY;

// dtoa safely converts a double to the equivalent char *
// It is the responsibility of the calling program to delete the returned string
static char * dtoa(float_t val)
{
	ostringstream ostr;
	ostr.precision(3);
	ostr << val;

	char * ret = new char[ostr.str().size() + 1];
	strcpy(ret, ostr.str().c_str());
	return ret;
}


// pow10() does not exist in mingw (redefine as inline wrapper here)
static inline float_t pow10(float_t x) {return pow(10.0,x);}

/* Rounds double value by discarding very small decimals.
 * (necessary to deal with previous small floating point errors)
 * @param val 		value to round
 * @param sigDigits number of significant digits.
 * @param intrep    all significant digits as a long int
 * @return		truncated value as a double (may be inexact)
 */
static float_t floatRound(float_t val, int&sigDigits, int&intrep) {
 int neg;
 int decPos;
 const int n=2;	// wanted significant digits
 char*str=_ecvt(val,n,&decPos,&neg);
 intrep = atoi(str);
 sigDigits=n;
	// find position of dot if any
 if (decPos>=n) { // then it's an integer, return now
  sigDigits = decPos;
  return val;
 }
 val=intrep*pow10(decPos-n);
 if (neg) val=-val;
 return val;
}

/* Local function hidden at file scope
 * Obtains a practical y axis range with "round" numbers at both ends
 * Params hold the min and max values in plot data on entry and
 * pass back the practical values on output.
 */
static void getNearest(Plotdata::Range&range) {
 int sigdigits; // Holds number of significant digits
 int intrep; // the integer representation

 float_t min = range.min, max = range.max, dif;

 min = floatRound(min, sigdigits, intrep);
 max = floatRound(max, sigdigits, intrep);

 dif = max-min - range.delta();

 range.init(floatRound(range.min - dif / 2, sigdigits, intrep),
            floatRound(range.max + dif / 2, sigdigits, intrep));
}

/*
 * Local function hidden at file scope
 * Attempt to guess a convenient divisor for grid along the X axis
 * If 0 is in the range, it should in many cases be on the grid.
 * Otherwise, divide the x range by 10, or 6, or 7, or 5, or 8, or 3,
 * or 4, in this order if divisible, by 10 otherwise.
 */
static int getXDivisor(float_t lo, float_t hi, int plotWidth) {
 int sigdigits, pquo, div = 0;
 int intVal;
 float_t delta = (hi - lo) / plotWidth; // Ignore 1 pixel inaccuracies
 float_t range = hi - lo;

 floatRound(range, sigdigits, intVal);

	// If 0 is on the axis attempt to place it on the grid
 if (lo < 0 && hi > 0) // 0 is part of the range
	{
		if(fabsl(lo) < hi)
			div = int(round(hi / fabsl(lo)));
		else
			div = int(round(fabs(lo) / hi));

		if (div > 0 && div <= 9)
        {
			div += 1;

            // If too small number of divisions, try multiples
            if(div == 2) // See if divisible by 12, 10, 6, 8, or 4
            {
        		if (remquo(intVal, 12, & pquo) < delta)
        			div = 12;
        	   	else if (remquo(intVal, 10, & pquo) < delta)
        			div = 10;
        		else if (remquo(intVal, 6, & pquo) < delta)
        			div = 6;
                else if (remquo(intVal, 8, & pquo) < delta)
        			div = 8;
        		else if (remquo(intVal, 4, & pquo) < delta)
        			div = 4;
            }
            else if (div == 3) // See if divisible by 12, 9 or 6
            {
        		if (remquo(intVal, 12, & pquo) < delta)
        			div = 12;
        	   	else if (remquo(intVal, 9, & pquo) < delta)
        			div = 9;
        		else if (remquo(intVal, 6, & pquo) < delta)
        			div = 6;
            }
            return div;
        }
	}

	// If not, then attempt a reasonable division of the range.
	// Try dividing by  12, or 10, or 5, or 6, or 8,  or 3, or 4
	else
	{
		if (remquo(intVal, 12, & pquo) < delta)
			div = 12;
	   	else if (remquo(intVal, 10, & pquo) < delta)
			div = 10;
		else if (remquo(intVal, 7, & pquo) < delta)
			div = 7;
    	else if (remquo(intVal, 6, & pquo) < delta)
			div = 6;
	    else if (remquo(intVal, 5, & pquo) < delta)
			div = 5;
		else if (remquo(intVal, 8, & pquo) < delta)
			div = 8;
		else if (remquo(intVal, 3, & pquo) < delta)
			div = 3;
		else if (remquo(intVal, 4, & pquo) < delta)
			div = 4;
		else // We are running out of common divisors
			div = 10;
	}
	return div;
}

/*
 * Attempt to guess a convenient divisor for grid along the Y axis
 * If 0 is in the range, it should in many cases be on the grid.
 * Otherwise, divide the y range by 5 or 3 or 4 in this order if
 * divisible, by 5 otherwise.
 */
static int getYDivisor(float_t lo, float_t hi, int plotHeight) {
 int sigdigits, pquo, div = 0;
 int intVal=0;
 float_t delta = (hi - lo) / plotHeight; // Ignore 1 pixel inaccuracies

	// If 0 is on the axis attempt to place it on the grid
 if (lo < 0 && hi > 0) {// 0 is part of the range
  if (fabsl(lo) < hi) div = int(round(hi / fabsl(lo)));
  else div = int(round(fabs(lo) / hi));
  if (div > 0 && div < 6) {
   div += 1;
            // If too small number of divisions, try multiples
   if(div == 2) {// See if divisible by 6, 4
    if (remquo(intVal, 6, & pquo) < delta) div = 6;
    else if (remquo(intVal, 4, & pquo) < delta) div = 4;
   }else if (div == 3) { // See if divisible 6
    if (remquo(intVal, 6, & pquo) < delta) div = 6;
   }
   return div;
  }
 }
	// if not, then attempt a reasonable division of the range
 float_t range = hi - lo;
 floatRound(range, sigdigits, intVal);
	// If 0 is in the middle try dividing by 4
 if (div == 1) {
  if (remquo(intVal, 4, & pquo) < delta) div = 4;
  else div = 2; // We are running out of common divisors
 }else{	// else try dividing by  3, or 4, or 5, or 2
  if (remquo(intVal, 5, & pquo) < delta) div = 5;
  if (remquo(intVal, 3, & pquo) < delta) div = 3;
  else if (remquo(intVal, 4, & pquo) < delta) div = 4;
  else if (remquo(intVal, 2, & pquo) < delta) div = 2;
  else div = 5;// We are running out of common divisors
 }
 return div;
}
#if 0
/*
 * This handler will be triggered on left mouse click.
 * It will record the event and current mouse coordinates in the window
 */
static void click_handler(int x, int y)
{
    left_clicked = true;

	cursorX = x;
	cursorY = y;
}

/*
 * Local function hidden at file scope
 * Delete a rectangle with top left corner x1, y1 and bottom right x2, y2
 */
static void delRectangle(int x1, int y1, int x2, int y2)
{
	int poly[8] = {x1, y1, x2, y1, x2, y2, x1, y2};
//	int bkColour = getbkcolor();
//	setfillstyle(EMPTY_FILL, bkColour );
//	setcolor(bkColour);
//	fillpoly(4, poly);
}
#endif

/************************* CLASS FUNCTIONS ***************************/

Plotstream::Plotstream(const char*title)
:plotStarted(false) {
 if (!wnd) wnd=CreateWindow("koolplot",title,WS_OVERLAPPEDWINDOW|WS_VISIBLE,
   CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
   0,0,0,this);
 else{
  SetWindowLongPtr(wnd,0,(LONG_PTR)this);	// repoint Windows object data
  if (title) SetWindowText(wnd,title);
 }
}

void Plotstream::addplot(const Plotdata&x, const Plotdata&y, Color color) {
 size_t i=traces.size();
 traces.resize(i+1);
 internal_xytrace&t=traces[i];
 t.t.x=&x;
 t.t.y=&y;
 t.t.a.colour=color;
 t.t.a.drawstyle=0;
 t.t.a.penstyle=PS_SOLID;
 t.t.a.penwidth=1;
 t.g.penPlot=CreatePen(t.t.a.penstyle,t.t.a.penwidth,t.t.a.colour);
}

void Plotstream::show(const char*title) {
 if (title) SetWindowText(wnd,title);
 InvalidateRect(wnd,0,TRUE);
 MSG Msg;
 while (GetMessage(&Msg,0,0,0)) {	// mouse click, space bar, or Alt+F4 generates WM_QUIT
  TranslateMessage(&Msg);
  DispatchMessage(&Msg);
 }
}

void Plotstream::plot(const Plotdata&x, const Plotdata&y, Color color) {
 addplot(x,y,color);
 show();
}

Plotstream::~Plotstream() {
 //std::vector<internal_xytrace>::iterator t;
 for (auto t=traces.begin(); t!=traces.end(); t++) {
  DeleteObject(t->g.penPlot);
  t->markers.clear();
 }
 traces.clear();
}

void Plotstream::onPaint() {
 RECT r;
 GetClientRect(wnd,&r);
 //internal_xytrace*t;

 for (auto t=traces.begin(); t!=traces.end(); t++) {
	// Need 2 points minimum to do a plot
  if (t->t.x->size() < 2) break;
	// Need as many y values as x values to do a plot
  if (t->t.x->size() > t->t.y->size()) break;
	// Store the hi and lo points of the axes
  Plotdata::rangeXY(*t->t.x,*t->t.y,xr,yr);
//  Plotdata::maxXY(*t->t.x,*t->t.y,hi_x,hi_y);
 }
    // Set Y values to the nearest "round" numbers
 getNearest(yr);

 x_range = xr.delta();
 y_range = -yr.delta();	// negative!

 SetRect(&rcPlot,left_border,border_height,r.right-border_width,r.bottom-border_height);

 x_scale = x_range / (rcPlot.right-rcPlot.left);
 y_scale = y_range / (rcPlot.bottom-rcPlot.top);

 drawAxes();

 for (auto t=traces.begin(); t!=traces.end(); t++) drawFunc(*t);
}

/* Convert graph x value to screen coordinate */
int Plotstream::X(float_t x) const{ return int((x - xr.min) / x_scale + rcPlot.left);}
/* Convert graph y value to screen coordinate */
int Plotstream::Y(float_t y) const{ return int((y - yr.max) / y_scale + rcPlot.top);}

/* Convert screen coordinate to graph x value */
float_t Plotstream::plotX(int screenX) const{ return (screenX - rcPlot.left) * x_scale + xr.min;}
/* Convert screen coordinate to graph y value */
float_t Plotstream::plotY(int screenY) const{ return (screenY - rcPlot.top) * y_scale + yr.max;}

/* @return  true if given location is within x and y axes ranges */
bool Plotstream::withinRange(float_t x, float_t y) const{
 return xr.inside(x) && yr.inside(y);
/*
 return	x + x_scale >= lo_x && x - x_scale <= hi_x
    &&	y + y_scale >= lo_y && y - y_scale <= hi_y;
*/
}

/* Round(est) double within one pixel in X
 *
 * @param theVal the value to be rounded, passed back rounded to caller
 * @param direction the requested direction of rounding UP, DOWN or ANY
 * @return the direction the rounding took place.
 */
Rounding Plotstream::nearRoundX(float_t & val, Rounding direction) const
{
	int valSigdigits;
	int upSigdigits;
	int downSigdigits;
	int intrep;
	float_t theVal, upVal, downVal;

	theVal = floatRound(val, valSigdigits, intrep);

	if (direction == UP) // If a higher or equal value has been requested
	{
		upVal = floatRound(val + x_scale, upSigdigits, intrep);
		if (theVal < 0 && upVal > 0) // Takes care of 0
		{
			val = 0;
			return UP;
		}
		else if (fabs(trunc(theVal)) < fabs(trunc(upVal))) // takes care of integers
		{
			val = trunc(upVal);
			return UP;
		}
		else if (valSigdigits > upSigdigits)
		{
			val = upVal;
			return UP;
		}
		else
			return ANY;
	}
	else if (direction == DOWN) // If lower or equal value is requested
	{
		downVal = floatRound(val - x_scale, downSigdigits, intrep);
		if (theVal > 0 && downVal < 0) // Takes care of 0
		{
			val = 0;
			return ANY;
		}
		else if (fabs(trunc(theVal)) > fabs(trunc(downVal))) // takes care of integers
		{
			val = trunc(theVal);
			return ANY;
		}
	    else if (valSigdigits > downSigdigits)
		{
			val = downVal;
			return DOWN;
		}
		else
			return ANY;
	}
	else // May round in any direction
	{
		upVal = floatRound(val + x_scale, upSigdigits, intrep);
		downVal = floatRound(val - x_scale, downSigdigits, intrep);
		if (theVal < 0 && upVal > 0)  // Takes care of 0
		{
			val = 0;
			return ANY;
		}
		else if (theVal > 0 && downVal < 0) // Takes care of 0
		{
			val = 0;
			return ANY;
		}
	   	else if (fabs(trunc(theVal)) < fabs(trunc(upVal)))
		{
			val = trunc(upVal);
			return UP;
		}
		else if (fabs(trunc(theVal)) > fabs(trunc(downVal))) // takes care of integers
		{
			val = trunc(theVal);
			return ANY;
		}

		int minimum = min(valSigdigits, upSigdigits);
		minimum = min (minimum, downSigdigits);

		if(minimum ==  valSigdigits)
		{
			val = theVal;
			return ANY;
		}
		else if (minimum == upSigdigits)
		{
			val = upVal;
			return UP;
		}
		else
		{
			val = downVal;
			return DOWN;
		}
	}
}

/* Round(est) double within one pixel in Y */
Rounding Plotstream::nearRoundY(float_t & val, Rounding direction) const
{
	int valSigdigits;
	int upSigdigits;
	int downSigdigits;
	int intrep;
	float_t theVal, upVal, downVal;

	theVal = floatRound(val, valSigdigits, intrep);

	if (direction == UP) // If a higher or equal value has been requested
	{
		upVal = floatRound(val + y_scale, upSigdigits, intrep);
		if (theVal < 0 && upVal > 0) // Takes care of 0
		{
			val = 0;
			return ANY;
		}
		else if (fabs(trunc(theVal)) < fabs(trunc(upVal))) // takes care of integers
		{
			val = trunc(upVal);
			return UP;
		}
		else if (valSigdigits > upSigdigits)
		{
			val = upVal;
			return UP;
		}
		else
			return ANY;
	}
	else if (direction == DOWN) // If lower or equal value is requested
	{
		downVal = floatRound(val - y_scale, downSigdigits, intrep);
		if (theVal > 0 && downVal < 0) // Takes care of 0
		{
			val = 0;
			return DOWN;
		}
		else if (fabs(trunc(theVal)) > fabs(trunc(downVal))) // takes care of integers
		{
			val = trunc(theVal);
			return ANY;
		}
	    else if (valSigdigits > downSigdigits)
		{
			val = downVal;
			return DOWN;
		}
		else
			return ANY;
	}
	else // May round in any direction
	{
		upVal = floatRound(val + y_scale, upSigdigits, intrep);
		downVal = floatRound(val - y_scale, downSigdigits, intrep);
		if (theVal < 0 && upVal > 0)  // Takes care of 0
		{
			val = 0;
			return ANY;
		}
		if (theVal > 0 && downVal < 0) // Takes care of 0
		{
			val = 0;
			return DOWN;
		}
	   	   else if (fabs(trunc(theVal)) < fabs(trunc(upVal)))
		{
			val = trunc(upVal);
			return UP;
		}
		else if (fabs(trunc(theVal)) > fabs(trunc(downVal))) // takes care of integers
		{
			val = trunc(theVal);
			return ANY;
		}

		int minimum = min(valSigdigits, upSigdigits);
		minimum = min (minimum, downSigdigits);

		if(minimum ==  valSigdigits)
		{
			val = theVal;
			return ANY;
		}
		else if (minimum == upSigdigits)
		{
			val = upVal;
			return UP;
		}
		else
		{
			val = downVal;
			return DOWN;
		}
	}
}

/* Draw the axes */
void Plotstream::drawAxes() {
 int xDivs;				// number of x divisions
 int yDivs;				// number of y divisions
// int divLength;	 	 	// length (in pixels) of a division
 int sigdigits;
 int intVal;
	// draw the rectangle
 HPEN penRect=CreatePen(PS_SOLID,1,DARKGRAY);
 HPEN penGrid=CreatePen(PS_DOT,0,LIGHTGRAY);
 HPEN penMark=CreatePen(PS_SOLID,0,DARKGRAY);
 HPEN open=SelectPen(dc,penRect);
 Rectangle(dc,rcPlot.left-1,   // -1 to fix small discrepancy on screen
		rcPlot.top-1, // Probably due to line width.
		rcPlot.right,
		rcPlot.bottom);

	// Attempt to guess a reasonable number of grid divisions for x and y
 xDivs = getXDivisor(xr.min,xr.max, rcPlot.right-rcPlot.left);
	// If y axis is large, divide in the same manner as x axis
 /*if (winHeight > winWidth * 3 / 5.0) yDivs = getXDivisor(lo_y, hi_y, plotHeight);
 else*/ yDivs = getYDivisor(yr.min,yr.max, rcPlot.bottom-rcPlot.top);

	// draw the grid
 SelectPen(dc,penGrid);
 int i;
	// Horizontal grid
 for (i = yDivs - 1; i > 0; i--) {
  int y=rcPlot.top + MulDiv(rcPlot.height(),i,yDivs);
  moveto(rcPlot.left,y);
  lineto(rcPlot.right,y);
 }
	// Vertical grid
 for (i = xDivs - 1; i > 0; i--) {
  int x=rcPlot.left + MulDiv(rcPlot.width(),i,xDivs);
  moveto(x,rcPlot.top);
  lineto(x,rcPlot.bottom);
 }
	// Draw Axes markers
 SelectPen(dc,penMark);
	// Y axis
 for (i = yDivs - 1; i > 0; i--) {
  int y=rcPlot.top + MulDiv(rcPlot.height(),i,yDivs);
  moveto(rcPlot.left,y);
  lineto(rcPlot.left+mark_length,y);
  moveto(rcPlot.right-mark_length,y);
  lineto(rcPlot.right,y);
 }
	// X axis
 for (i = xDivs - 1; i > 0; i--) {
  int x=rcPlot.left + MulDiv(rcPlot.width(),i,xDivs);
  moveto(x,rcPlot.bottom-mark_length);
  lineto(x,rcPlot.bottom);
  moveto(x,rcPlot.top);
  lineto(x,rcPlot.top+mark_length);
 }
	// Number the axes
 SetTextColor(dc,BLACK);
 SetBkMode(dc,TRANSPARENT);
	// Y axis
 HFONT fntY=CreateFont(16,0,0,0,0,0,0,0,0,0,0,0,0,"Arial");
 HFONT ofnt=SelectFont(dc,fntY);
 SetTextAlign(dc,TA_RIGHT|TA_TOP);
 SIZE sz;
 GetTextExtentPoint32(dc,"0",1,&sz);
// divLength = int(rcPlot.height() / yDivs);
 float_t divVal = floatRound(-yr.delta() / yDivs, sigdigits, intVal);

 for (i = 0;  i <= yDivs; i++) {
  int y=rcPlot.top + MulDiv(rcPlot.height(),i,yDivs)-sz.cy/2;
  float_t val = floatRound(yr.max + divVal * i, sigdigits, intVal);
  if (fabs(val) < chouia * yr.delta()) val = 0;
  char * asciival = dtoa(val);
  outtextxy(rcPlot.left-sz.cx,y,asciival);
  delete [] asciival;
 }
	// X axis
 SetTextAlign(dc,TA_CENTER|TA_TOP);
 divVal = floatRound(xr.delta() / xDivs, sigdigits, intVal);
 for (i = 0;  i <= xDivs; i++) {
  int x=rcPlot.left + MulDiv(rcPlot.width(),i,xDivs);
  float_t val = floatRound(xr.min + divVal * i, sigdigits, intVal);
  if (fabs(val) < chouia * xr.delta()) val = 0;
  char * asciival = dtoa(val);
  outtextxy(x,rcPlot.bottom+sz.cy/4, asciival);
  delete[] asciival;
 }
 SelectFont(dc,ofnt);
 DeleteFont(fntY);
 SelectPen(dc,open);
 DeletePen(penMark);
 DeletePen(penGrid);
 DeletePen(penRect);
}

void Plotstream::drawFunc(internal_xytrace&t) {
 vector<float_t>::const_iterator x_it  = t.t.x->getData().begin();
 vector<float_t>::const_iterator y_it  = t.t.y->getData().begin();
 vector<float_t>::const_iterator x_end = t.t.x->getData().end();

 HPEN open=SelectPen(dc,t.g.penPlot);

 if (isfinite(*y_it) && isfinite(*x_it)) {
  moveto(X(*x_it), Y(*y_it));
  plotStarted = true;
 }else{
    // Else if both x and y are NAN, then it could be either a color change
    // request or a single point drawing request.
// if (!isfinite(*y_it) && !isfinite(*x_it)
// && x_it+1 != x_end && x_it+2 != x_end) handleCommand(x_it, y_it);
 }

 while (++x_it != x_end) {
  if (isfinite(*(++y_it)) && isfinite(*x_it)) {
   if (plotStarted) lineto(X(*x_it), Y(*y_it));
   else{
    moveto(X(*x_it), Y(*y_it));
    plotStarted = true;
   }
  }else{
   plotStarted = false;
            // If both x and y are NAN, then it could be either a colour
            // change request or a single point drawing request.
//   if (!isfinite(*y_it) && !isfinite(*x_it)
//   && x_it+1 != x_end && x_it+2 != x_end) handleCommand(x_it, y_it);
  }
 }
 //marker_t*marker;
 for (auto marker=t.markers.begin(); marker!=t.markers.end(); marker++) {
  drawPointShape(X(marker->x),Y(marker->y));
 }
 SelectPen(dc,open);
}

// draws the marker shape in X and Y.
void Plotstream::drawMarkShape(int x, int y) const{
	// Horz
 moveto(x - 4, y); lineto(x + 4, y); 		// centre line
 moveto(x - 5, y - 1);	lineto(x - 3, y - 1);
 moveto(x - 5, y + 1);	lineto(x - 3, y + 1);	// side lines
 moveto(x + 5, y - 1);	lineto(x + 3, y - 1);
 moveto(x + 5, y + 1);	lineto(x + 3, y + 1);
	// Vert
 moveto(x	, y - 4); 	lineto(x	, y + 4);	// centre line
 moveto(x - 1, y - 5);	lineto(x - 1, y - 3);
 moveto(x + 1, y - 5);	lineto(x + 1, y - 3);	// side lines
 moveto(x - 1, y + 5);	lineto(x - 1, y + 3);
 moveto(x + 1, y + 5);	lineto(x + 1, y + 3);
}
#if 0
// Draw a marker at the current cursor position
// Will erase only if erase is true
void Plotstream::drawMarker(bool erase) {
	/* select XOR drawing mode and marker colour */
 int orop=SetROP2(dc,R2_XORPEN);
 HPEN penMark=CreatePen(PS_SOLID,0,marker_color);
 HPEN open=SelectPen(dc,penMark);

 if (marked) { // then erase existing marker
  drawMarkShape(lastX, lastY);
  marked = false;
 }
 if (!erase) {
  drawMarkShape(lastX=cursorX, lastY=cursorY);
  marked = true;
 }
	/* Restore drawing mode */
 SelectPen(dc,open);
 DeletePen(penMark);
 SetROP2(dc,orop);
}
#endif
// draws the point shape in X and Y.
void Plotstream::drawPointShape(int x, int y) const {
	// Horz
 moveto(x - 1, y - 2); 	lineto(x + 1, y - 2);   // top line
 moveto(x - 1, y + 2);	lineto(x + 1, y + 2);	// bottom line
	// Vert
 moveto(x - 2, y - 1); 	lineto(x - 2, y + 1);	// left line
 moveto(x + 2, y - 1);	lineto(x + 2, y + 1);	// right line
}

// Draw a single point at the given coordinates
void Plotstream::drawSinglePoint(float_t xCoord, float_t yCoord) const{
 drawPointShape(X(xCoord) + 1, Y(yCoord));
}
#if 0
// Set new foreground colour, remember last colour
void Plotstream::setFgColor(Color fgColour) {
 lastColour = colour;
 colour = fgColour;
//    setcolor(fgColour);
}

// Reset foreground colour to last used colour
void Plotstream::resetFgColor() {
 setFgColor(lastColour);
}
#endif
