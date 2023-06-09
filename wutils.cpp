#include "Plotstream.h"
#include <math.h>

//bool std::isfinite(double v) {return !!_finite(v);}
//double std::round(double v) {return ::floor(v+0.5);}
//double std::trunc(double v) {return (double)(int)v;}
//double std::remquo(double a,double b,int*c) {
// if (c) *c=int(a/b);
// return fmod(a,b);
//}

LRESULT CALLBACK PlotWindowProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
 Plotstream*ps=(Plotstream*)GetWindowLongPtr(wnd,0);
 switch (msg) {
  case WM_NCCREATE: {
   CREATESTRUCT*cs=(CREATESTRUCT*)lParam;
   ps=(Plotstream*)cs->lpCreateParams;
   SetWindowLongPtr(wnd,0,(LPARAM)ps);
   ps->wnd=wnd;
  }break;
  case WM_PAINT: {
   PAINTSTRUCT s;
   BeginPaint(wnd,&s);
   ps->dc=s.hdc;
   ps->onPaint();
   EndPaint(wnd,&s);
  }return 0;
  case WM_PRINTCLIENT: {
   ps->dc=(HDC)wParam;
   ps->onPaint();
  }return 0;
  case WM_KEYDOWN: switch (wParam) {
   case VK_ESCAPE: DestroyWindow(wnd); break;
   case VK_SPACE: PostQuitMessage(2); break;
  }break;
  case WM_LBUTTONDOWN: PostQuitMessage(1); break;
  case WM_CLOSE: DestroyWindow(wnd); break;
  case WM_DESTROY: PostQuitMessage(0); break;
  case WM_NCDESTROY: ps->wnd=0; break;
 }
 return DefWindowProc(wnd,msg,wParam,lParam);
}

static struct initializer{
 initializer();
}initializer;

initializer::initializer() {
 WNDCLASS wc;
 memset(&wc,0,sizeof wc);
 wc.style=CS_HREDRAW|CS_VREDRAW/*|CS_DBLCLKS*/;
 wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
 wc.hCursor=LoadCursor(0,IDC_ARROW);
 wc.hIcon=LoadIcon(0,IDI_APPLICATION);
 wc.cbWndExtra=sizeof(void*);
 wc.lpfnWndProc=PlotWindowProc;
 wc.lpszClassName="koolplot";
 RegisterClass(&wc);
}
