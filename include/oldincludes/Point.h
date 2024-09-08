#ifndef POINT_H
#define POINT_H

#include "stdafx.h"
#include <vector>

using namespace std;

class Point{

public:
	Point();
	~Point();
	void setPointVect(int i, ...);
	vector<int> getPointVect();

private:
	vector<int> pointVect;


};

#endif