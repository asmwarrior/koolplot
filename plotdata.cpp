/**
 * Implementation of Plotdata class
 * 
 * Stores data for one 2D plotting axis
 *
 * Provides operators and mathematical functions to map a Plotdata
 * object to another using unary and binary functions.
 *
 * Author: 	jlk
 * Version:	1.1
 * Date:	July 2005
 *
 * This file is in the public domain and can be used without any
 * restriction.
200101	Removed dead code around color change
 */

#include <functional>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <limits>

#include "PlotData.h" 

/* 
 * Constructors
 */

Plotdata::Plotdata(float_t lo, float_t hi, Grain grain)
: userFunction(0), userBinFunction(0)
{
	plotRange(lo, hi, grain);
}

Plotdata::Plotdata(const float_t*array, int dataSize)
: data(array, array + dataSize), userFunction(0), userBinFunction(0)
{}

/*  
 * Member Functions
 */
// Insert dataSize elements from array into Plotdata
void Plotdata::insert(const float_t array[], int dataSize)
{
    // Append, rather than insert at the start
    copy(array, array + dataSize, back_inserter(data));
    // data = vector<double>(array, array + dataSize);
}

 
void Plotdata::plotRange(float_t lo,float_t hi,size_t numpoints,LogSpace isLog)
{
	int divisions = numpoints - 1;
	float_t rangeSize, shift;

	if (divisions < 2) divisions = 49; // Guarantees a smooth enough curve
	
	if(lo > hi)
		swap(lo, hi);
		
	// If space is logarithmic do nothing if hi and lo are of opposite signs
	if(isLog && lo < 0 && hi > 0)
		return;
	
	rangeSize = hi - lo;
	
	// if hi and lo are too close to each other do nothing
	if(rangeSize < numeric_limits<float>::epsilon())
		return;
		
	   
	data.clear(); 
	
	if( !isLog)
	{
		data.push_back(lo);

		// distance between 2 points
		float_t dist = rangeSize / divisions; 
		 
		for (int i = divisions; i > 1; i--)
			data.push_back(lo += dist);
	}
	else // Logarithmic range
	{
		// Neither lo nor hi may be 0. Move away if too close
		// Dirty kludge! Avoids crash but introduces inaccuracies.
		if (fabsl(lo) < numeric_limits<float>::epsilon())
		{
			shift = rangeSize / 1000;
			if ( lo < 0 )
				shift = - shift;
			while(fabsl(lo) < numeric_limits<float>::epsilon())	   
				lo += shift;
		}
		if (fabsl(hi) < numeric_limits<float>::epsilon())
		{
			shift = - rangeSize / 1000;
			if ( hi > 0 )
				shift = - shift;
			while(fabsl(hi) < numeric_limits<float>::epsilon())	   	      	  	  	  
				hi += shift;
		}
		
		data.push_back(lo);
	      	   
	   	float_t mult = logl(hi/lo) / divisions;
		for (int i = 1; i < divisions; i++)
		{
			data.push_back(lo * expl(i * mult));
		}
	}
	
	data.push_back(hi);
}

Plotdata Plotdata::operator + (const Plotdata & other) const
{
    int thisSize = data.size(),
        otherSize = other.data.size();
    
	// Size of returned data is that of the smallest of the two
	size_t len = thisSize < otherSize ? thisSize : otherSize;
	
	vector<float_t> vec(len);
	transform(data.begin(),
		 	  data.begin() + len, 
		 	  other.data.begin(),
			  vec.begin(),
		 	  plus<float_t>());
	return Plotdata(vec);
}

Plotdata Plotdata::operator + (float_t val) const
{
	vector<float_t> vec(data.size());
	transform(data.begin(),
		 	  data.end(), 
			  vec.begin(),
		 	  bind2nd(plus<float_t>(), val));
	return Plotdata(vec);
}

Plotdata operator + (float_t op1, const Plotdata & pd)
{
	vector<float_t> vec(pd.data.size());
	transform(pd.data.begin(),
		 	  pd.data.end(), 
			  vec.begin(),
		 	  bind2nd(plus<float_t>(), op1));
	return Plotdata(vec);
}
	   
Plotdata Plotdata::operator - (const Plotdata & other) const
{
	// Size of returned data is that of the smallest of the two
	size_t len = size() > other.size() ? size() : other.size();
	
	vector<float_t> vec(len);
	transform(data.begin(),
		 	  data.begin() + len, 
 	  	      other.data.begin(),
			  vec.begin(),
		 	  minus<float_t>());
	return Plotdata(vec);
}

Plotdata Plotdata::operator - (float_t val) const
{
	vector<float_t> vec(data.size());
	transform(data.begin(),
		 	  data.end(), 
			  vec.begin(),
		 	  bind2nd(minus<float_t>(), val));
	return Plotdata(vec);
}

Plotdata operator - (float_t op1, const Plotdata & pd)
{
	vector<float_t> vec(pd.data.size());
	transform(pd.data.begin(),
		 	  pd.data.end(), 
			  vec.begin(),
		 	  bind2nd(minus<float_t>(), op1));
	return Plotdata(vec);
}

Plotdata Plotdata::operator * (const Plotdata & other) const
{
	// Size of returned data is that of the smallest of the two
	size_t len = size() > other.size() ? size() : other.size();
	
	vector<float_t> vec(len);
	transform(data.begin(),
		 	  data.begin() + len, 
	   	      other.data.begin(),
			  vec.begin(),
		 	  multiplies<float_t>());
	return Plotdata(vec);
}

Plotdata Plotdata::operator * (float_t val) const
{
	vector<float_t> vec(data.size());
	transform(data.begin(),
		 	  data.end(), 
			  vec.begin(),
		 	  bind2nd(multiplies<float_t>(), val));
	return Plotdata(vec);
}

Plotdata operator * (float_t op1, const Plotdata & pd)
{
	vector<float_t> vec(pd.data.size());
	transform(pd.data.begin(),
		 	  pd.data.end(), 
			  vec.begin(),
		 	  bind2nd(multiplies<float_t>(), op1));
	return Plotdata(vec);
}

 
Plotdata Plotdata::operator / (const Plotdata & other) const
{
	// Size of returned data is that of the smallest of the two
	size_t len = size() > other.size() ? size() : other.size();
	
	vector<float_t> vec(len);
	transform(data.begin(),
		 	  data.begin() + len, 
			  other.data.begin(),
			  vec.begin(),
		 	  divides<float_t>());
	return Plotdata(vec);
}

Plotdata Plotdata::operator / (float_t val) const
{
	vector<float_t> vec(data.size());
	transform(data.begin(),
		 	  data.end(), 
			  vec.begin(),
		 	  bind2nd(divides<float_t>(), val));
	return Plotdata(vec);
}

Plotdata Plotdata::operator ^ (float_t val) const
{
	vector<float_t> vec(data.size());
	transform(data.begin(),
		 	  data.end(), 
			  vec.begin(),
		 	  bind2nd(ptr_fun(powl), val));
	return Plotdata(vec);
}


Plotdata operator / (float_t op1, const Plotdata & pd)
{
	vector<float_t> vec(pd.data.size());
	transform(pd.data.begin(),
		 	  pd.data.end(), 
			  vec.begin(),
		 	  bind2nd(divides<float_t>(), op1));
	return Plotdata(vec);
}


Plotdata Plotdata::operator - ( ) const
{
	vector<float_t> vec(data);
	transform(vec.begin(),
		 	  vec.end(), 
		 	  vec.begin(),
		 	  negate<float_t>());
	return Plotdata(vec);
}

// Concatenation operator
Plotdata & Plotdata::operator << (const Plotdata & toadd)
{
	data.insert(data.end(), toadd.data.begin(), toadd.data.end());
	
	return *this; // This makes the << operator transitive
}

// add a double to the data
Plotdata & Plotdata::operator << (float_t toadd)
{
	data.push_back(toadd);
	return *this;
}

// Functor class is_finite: Returns true if val is neither
// NaN (NOPLOT) nor infinite. A wrapper around isfinite(), necessary as
// isfinite() is a macro and cannot be used as a predicate.
class is_finite
{
public:
    bool operator() (float_t val) const
    {
		return isfinite(val);   
    }
};

/**
 * Class (static) function
 * Retrieves the maximums of each of x and y in a Plotdata pair
 * Values corresponding to a NOPLOT in the other Plotdata are not included;
 * Assign their respective maximums to xMax and yMax; NOPLOT if either does
 * not have a valid maximum.
 */
void Plotdata::rangeXY(const Plotdata&x,const Plotdata&y,Range&xr,Range&yr) {
 xr.init(); yr.init();
    // Only search if both vectors contain some values
 if (x.data.size() && y.data.size()){
  dataIterator xIter = x.data.begin();
  dataIterator yIter = y.data.begin();
  dataIterator xEnd  = x.data.end();
  dataIterator yEnd  = y.data.end();
        
  for (; xIter != xEnd && yIter != yEnd; xIter++, yIter++) {
   if (isfinite(*xIter) && isfinite(*yIter)) {
    xr.expand(*xIter);
    yr.expand(*yIter);
   }
  }
 }
}

// Return new plot data with unary function applied to each 
// original data elements values
Plotdata Plotdata::doFunc(Func aFunction) const{
 Plotdata ret(size()); 
 transform(data.begin(), data.end(), ret.data.begin(), aFunction);
 return ret;
}	 

// Return new plot data with binary function applied to each original
// data elements values and op2 as the second function param
Plotdata Plotdata::doBinFunc(BinFunc aFunc, float_t op2) const{
 Plotdata ret(size()); 
 transform(data.begin(), data.end(), ret.data.begin(),
 bind2nd(ptr_fun(aFunc), op2));
 return ret;
}	 

// Return new plot data with user binary function applied to each
// original data elements values and op2 as the second function param
Plotdata Plotdata::doBinFunc(float_t op2) const{
 Plotdata ret(size()); 
 transform(data.begin(), data.end(), ret.data.begin(),
   bind2nd(ptr_fun(userBinFunction), op2));
 return ret;
}	 


ostream& operator<< (ostream&out, const Plotdata&pd){
 out << endl << pd.data.size() << endl;
 copy(pd.data.begin(), pd.data.end(), ostream_iterator<float_t>(out, " "));
 return out;
}

istream& operator>> (istream&in, Plotdata&pd){
 int size;
 float_t val;
 in >> size;
 for(int i = size; i > 0; i--){
  in >> val;
  pd.data.push_back(val);
 }
 return in;
}
