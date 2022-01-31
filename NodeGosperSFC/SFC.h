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

#include "PointCloud.h"
#include "Sorting.h"

/*
Basic SFC abstract class

D - dimension of points
*/
template <uint D> class SFC
{
private:
	uint * indices; //Array of point indices
	CODE * codes; //Array of point codes
protected:
	PointCloud<D> * pc; //Point cloud object
public:
	SFC(PointCloud<D> * _pc);
	virtual ~SFC();

	/*
	Returns number of points
	*/
	uint GetPointNum() { return pc->GetPointNum(); }
	/*
	Returns array of indices
	*/
	uint * GetIndices() { return indices; }
	/*
	Returns array of codes
	*/
	CODE * GetCodes() { return codes; }
	/*
	Returns bounding box
	*/
	const BB * GetBB() { return pc->GetBB(); }
	/*
	Returns SFC hash code of a point p

	p - point being hashed
	*/
	virtual CODE HashCode(const Point * p) = 0;
	/*
	Sorts point indices by codes
	*/
	virtual void SortSFC();
	/*
	Returns the i-th point along SFC
	*/
	virtual const Point * GetSFCPoint(uint i) { return (pc->GetArray() + indices[i]); }
	/*
	Constructs SFC
	*/
	virtual void ConstructSFC();
	/*
	Returns number of bits representing a code index on one recursive level
	*/
	virtual inline uint GetBitShift() = 0;
	/*
	Returns bit mask for reading a code index on one recursive level
	*/
	virtual inline uint GetBitAndMask() = 0;
};

template <uint D> SFC<D>::SFC(PointCloud<D> * _pc)
{
	pc = _pc;
	indices = new uint[GetPointNum()];
	codes = new CODE[GetPointNum()];
}

template <uint D> SFC<D>::~SFC()
{
	pc = NULL;
	delete[] indices;
	indices = NULL;
	delete[] codes;
	codes = NULL;
}

template <uint D> void SFC<D>::SortSFC()
{
	Sorting<CODE, uint>::quickSort(codes, indices, 0, ((int)GetPointNum()) - 1);
}

template <uint D> void SFC<D>::ConstructSFC()
{
	const Point * pts = pc->GetArray();
	CODE * cds = codes;
	uint * idxs = indices;
	for (uint i = 0; i < GetPointNum(); i++, idxs++, pts++, cds++) {
		*idxs = i;
		*cds = HashCode(pts);
	}

	SortSFC();
}