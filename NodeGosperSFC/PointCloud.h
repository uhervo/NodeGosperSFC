#pragma once

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

#include "common.h"

/*
PointCloud class loading / containing data

D - dimension of points
*/
template <uint D> class PointCloud
{
private:
	uint pnum; //Number of points
	Point * data; //Array of points
	BB * bb; //Bounding box

public:
	PointCloud();
	virtual ~PointCloud();
	
	/**
	Loads point dataset

	path - file address
	n - number of points
	*/
	bool LoadDataset(string path, uint n);
	/**
	Returns number of points
	*/
	uint GetPointNum() { return pnum; }
	/**
	Returns bounding box
	*/
	const BB * GetBB() { return bb; }
	/**
	Returns pointer to the array of points
	*/
	const Point * GetArray() { return data; }
	/**
	Returns the i-th point
	*/
	Point operator[] (uint i) { return data[i]; }
};

template <uint D> PointCloud<D>::PointCloud()
{
	pnum = 0;
	data = NULL;
	bb = NULL;
}

template <uint D> PointCloud<D>::~PointCloud()
{
	pnum = 0;
	delete[] data;
	data = NULL;
	delete bb;
	bb = NULL;
}

template <uint D> bool PointCloud<D>::LoadDataset(const string path, const uint n)
{
	pnum = n;
	data = new Point[n];
	bb = new BB();

	ifstream is(path);

	if (!is.is_open()) {
		is.close();
		cout << "File " << path << " cannot be opened." << endl;
		return false;
	}

	//Load points from file, compute BB
	Point * p = data;
	for (uint i = 0; i < n; i++, p++) {
		for (uint d = 0; d < D; d++) {
			is >> p->arr[d];
		}

		if (!is.good()) {
			is.close();
			cout << "Error while reading file " << path << "." << endl;
			return false;
		}

		if (i == 0) {
			bb->min = bb->max = *p;
		}

		for (uint d = 0; d < D; d++) {
			bb->min.arr[d] = (p->arr[d] < bb->min.arr[d] ? p->arr[d] : bb->min.arr[d]);
			bb->max.arr[d] = (p->arr[d] > bb->max.arr[d] ? p->arr[d] : bb->max.arr[d]);
		}
	}

	is.close();

	//Correction of dataset to the square BB
	REAL maxLength = 0.;
	for (uint d = 0; d < D; d++) {
		if ((bb->max.arr[d] - bb->min.arr[d]) > maxLength)
			maxLength = (bb->max.arr[d] - bb->min.arr[d]);
	}
	for (uint d = 0; d < D; d++) {
		bb->max.arr[d] = bb->min.arr[d] + maxLength;
	}

	//Centering of dataset to (0,0)
	p = data;
	for (uint i = 0; i < n; i++, p++) {
		for (uint d = 0; d < D; d++) {
			p->arr[d] -= 0.5f*(bb->max.arr[d] + bb->min.arr[d]);
		}
	}

	REAL subtract;
	for (uint d = 0; d < D; d++) {
		subtract = 0.5f*(bb->max.arr[d] + bb->min.arr[d]);
		bb->max.arr[d] -= subtract;
		bb->min.arr[d] -= subtract;
	}

	//Small shift of bounds to eliminate rounding errors
	for (uint d = 0; d < D; d++) {
		bb->max.arr[d] += 0.001*maxLength;
		bb->min.arr[d] -= 0.001*maxLength;
	}
	
	return true;
}