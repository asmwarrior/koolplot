/* File: Plotstream.h
 *
 * Class Plotstream
 * A plotstream opens a window, displays a data plot, then closes
 * the window when the user presses a key.
 *
 * Author: 	jlk
 * Version:	1.1
 * Date:	July 2005
 */
#pragma once

#include "PlotData.h"

enum Rounding{DOWN,ANY,UP};

struct Rect:public RECT{
 LONG width() const{return right-left;}
 LONG height() const{return bottom-top;}
};

class Plotstream{
public:
// Plotstream(const char * title = "2D Plot", int width = 640, int height = 0);
 Plotstream(const char*title=0);
 ~Plotstream();
 void addplot(const Plotdata&x, const Plotdata&y, Color colour = GREEN);
 void show(const char*title=0);
 void plot(const Plotdata&x, const Plotdata&y, Color colour = GREEN);
 static HWND wnd;
 static HDC dc;
 void onPaint();
 struct attrib{
  Color colour;
  char penwidth;
  char penstyle;
  char fillto;
  char drawstyle;
 };
 struct xytrace{
  const Plotdata*x;
  const Plotdata*y;
  attrib a;
 };
private:
 Rect rcPlot;
// int winWidth, winHeight;
// int plotWidth, plotHeight;
 Plotdata::Range xr,yr;	// range of plotdata
 float_t x_range, y_range; // Ranges of x values and y values
 float_t x_scale, y_scale; // Scales of graph drawing to screen pixels
 bool plotStarted;	// True while plotting is going on
 bool marked; 		// True when a marker is visible
 int lastX;
 int lastY; 		// Location of last marker drawn
 Color colour;	// Current drawing colour
 Color lastColour;	// Previous drawing colour
	   
	/* Convert graph x value to screen coordinate */
 int X(float_t x) const;
	/* Convert graph y value to screen coordinate */
 int Y(float_t y) const;
	/* Convert screen coordinate to graph x value */
 float_t plotX(int screenX) const;
	/* Convert screen coordinate to graph y value */
 float_t plotY(int screenY) const;
	/* Round(est) double within one pixel in X */
 Rounding nearRoundX(float_t & val, Rounding direction = ANY) const;
	/* Round(est) double within one pixel in Y */
 Rounding nearRoundY(float_t & val, Rounding direction = ANY) const;
	   /* True if values given are within x and y graph ranges */
 bool withinRange(float_t xVal, float_t yVal) const;
	/* Draws the axes */
 void drawAxes();
	/* Watches the mouse and prints cursor coords if clicked */
// bool watchMouse();
	// Draw a marker at the current cursor position. Erase if erase is true
// void drawMarker(bool erase = false);
    // Draw a single point at the requested coordinates
 void drawSinglePoint(float_t xCoord, float_t yCoord) const;
    // Set new foreground colour, store last colour
// void setFgColor(Color fgColour);
    // Reset foreground colour to last colour
// void resetFgColor();
    // Check whether the stream holds a command at this point (colour change
    // or single point), and handle the command if it does.
// void handleCommand(dataIterator&x_it,dataIterator&y_it);
 
 void drawPointShape(int x, int y) const;
 void drawMarkShape(int x, int y) const;
 void moveto(int x, int y) const {MoveToEx(dc,x,y,0);}
 void lineto(int x, int y) const {LineTo(dc,x,y);}
// void linerel(int x, int y) const {POINT pt;GetCurrentPositionEx(dc,&pt);LineTo(dc,x+pt.x,y+pt.y);}
 void outtextxy(int x, int y,const char*s) const {TextOut(dc,x,y,s,strlen(s));}
 struct gdiobj{
  HPEN penPlot;
  HBRUSH brFill;
 };
 struct marker_t{
  float_t x,y;
 };
 struct internal_xytrace{
  xytrace t;
  std::vector<marker_t>markers;
  gdiobj g;
 };
 std::vector<internal_xytrace> traces;
	/* Draw the data */
 void drawFunc(internal_xytrace&t);
};
