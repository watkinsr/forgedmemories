#include "Point.h"
#include <stdarg.h>


Point::Point(){

}

Point::~Point(){


}

void Point::setPointVect(int i, ...){

	double total = 0; /* initialize total */ 
	int j; 
	va_list ap; /* for storing information needed by va_start */ 

	va_start( ap, i ); /* initialize ap for use in va_arg and va_end */ 

	/* process variable length argument list */ 
	for ( j = 1; j <= i; j++ ) { 
		
		total = va_arg( ap, int ); 
		pointVect.push_back(total);
	} /* end for */ 

	va_end( ap ); /* end the va_start */ 
	

}

vector<int> Point::getPointVect(){

	return pointVect;

}
