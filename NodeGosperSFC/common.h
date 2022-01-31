#pragma once

#include <fstream>
#include <string>
#include <iostream>
#include <cmath>

using namespace std;

// Copyright (c) 2019 Vojtech Uher, VSB - Technical University of Ostrava
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights  to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

// This software corresponds to our academic research. If you use this implementation 
// cite our corresponding academic paper as:
//
//	V. Uher, P. Gajdos, V. Snasel, Y.-C. Lai, and M. Radecky. Hierarchical Hexagonal 
//	Clustering and Indexing. Symmetry-Basel, 11(6) : 731, Jun 2019.
//

typedef unsigned int uint;
typedef double REAL;
typedef unsigned long long CODE;
typedef long long Int;

//Point structures
struct Point2D
{
	union {
		struct {
			REAL x, y;
		};
		REAL arr[2];
	};
	Point2D(REAL _x, REAL _y) :x(_x), y(_y) {}
	Point2D() {}
};
struct Point3D
{
	union {
		struct {
			REAL x, y, z;
		};
		REAL arr[3];
	};
	Point3D(REAL _x, REAL _y, REAL _z) :x(_x), y(_y), z(_z) {}
	Point3D() {}
};
struct Int2D {
	union {
		struct {
			Int x, y; //q, r
		};
		Int arr[2];
	};
	Int2D(Int _x, Int _y) :x(_x), y(_y) {}
	Int2D() {}
};
struct Int3D {
	union {
		struct {
			Int x, y, z;
		};
		Int arr[3];
	};
	Int3D(Int _x, Int _y, Int _z) :x(_x), y(_y), z(_z) {}
	Int3D() {}
};

typedef Point2D Point;
typedef Int2D IntN;

//Bounding box
struct BB
{
	Point min, max;
};

#define SQR(x)((x)*(x)) //Square
#define RND(x)( ((x)<0.0) ? ((int)((x) - 0.5)) : ((int)((x) + 0.5)) ) //Round

inline REAL distance(const Point2D & v0, const Point2D & v1) {
	return sqrt(SQR(v0.x - v1.x) + SQR(v0.y - v1.y));
}