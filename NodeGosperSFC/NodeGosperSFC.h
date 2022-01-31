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

#include "SFC.h"

//Used constants
#define F1_SQRT7 0.3779644730092272 // 1 / SQRT(7)
#define F1_3 0.3333333333333333 // 1/3
#define F2_3 0.6666666666666667 // 2/3
#define F1_7 0.1428571428571428 // 1/7
#define SQRT3_3 0.5773502691896257 // SQRT(3) / 3

//Types of Node-Gosper indexation patterns
enum NodeGosperSFC_Type {
	NodeGosperSFC_Center,
	NodeGosperSFC_Precise,
	NodeGosperSFC_Simple,
	NodeGosperSFC_Linear,
	NodeGosperSFC_Snake
};

#define MAX_LEVEL_NUM 21

/*
Node-Gosper SFC class
*/
class NodeGosperSFC : public SFC<2> {
private:
	//Precomputed array of circles inscribed into Gosper islands of different levels
	const REAL norm_insc[MAX_LEVEL_NUM] = { 0.755928946000, 0.755928946000, 0.750121467308, 0.746782631146, 0.746782631146, 0.746577727521, 0.746348363909, 0.746348363909, 0.746344578768, 0.746327538283, 0.746327538283, 0.746327538283, 0.746326555879, 0.746326555879, 0.746326555879, 0.746326510616, 0.746326510616, 0.746326510616, 0.746326508597, 0.746326508597, 0.746326508597 };
	REAL smallHexSize; //Size of hexagons of the deepest level of recursion
	
	uint level; //Index of max. level of recursion (i.e. depth-1)
	PointCloud<2> * pc; //Point cloud object
	NodeGosperSFC_Type type; //Type of indexation pattern

public:
	NodeGosperSFC(uint _level, PointCloud<2> * _pc, NodeGosperSFC_Type _type) : SFC(_pc) {
		//Max. level condition
		if ((_level + 1) > MAX_LEVEL_NUM) {
			cout << "ERROR: Level greater than " << (MAX_LEVEL_NUM - 1) << endl;
			throw 1;
		}

		level = _level;
		pc = _pc;
		type = _type;

		//Computation of the smallHexSize according to BB diagonal which secures that the BB diagonal \
		fits into the circle inscribed into the Gosper island of required level
		const BB * bb = pc->GetBB();
		REAL halfdiag = 0.5f*distance(bb->min, bb->max); //BB diagonal
		REAL s = halfdiag / norm_insc[level];
		smallHexSize = s*pow(F1_SQRT7, level);
	}

	virtual ~NodeGosperSFC() {
		pc = 0;
	}
	/*
	Returns number of bits representing code of one recursive level
	*/
	virtual inline uint GetBitShift() { return 3; }
	/*
	Returns bit mask for reading code of one recursive level
	*/
	virtual inline uint GetBitAndMask() { return 7; }
	/*
	Returns Node-Gosper hash code of a point p depending on the selected type of indexation pattern
	*/
	virtual CODE HashCode(const Point * p);
	/*
	Returns size of the smallest hexagon
	*/
	inline REAL GetCellSize() {
		return smallHexSize;
	}

private:
	/*
	Returns code of a point p using the center indexation pattern (P1)

	reverse - if true it writes the code bits of recursive levels in reverse order, required by HashCodePrecise
	*/
	CODE HashCodeCenter(const Point * p, bool reverse = false);
	/*
	Returns code of a point p using the precise Node-Gosper indexation pattern (P2) \
	-  includes additional transformations for continuous SFC
	*/
	CODE HashCodePrecise(const Point * p);
	/*
	Returns code of a point p using the precise Node-Gosper indexation pattern (P2) \
	-  without additional transformations for continuous SFC
	*/
	CODE HashCodeSimple(const Point * p);
	/*
	Returns code of a point p using the linear indexation pattern (P3)
	*/
	CODE HashCodeLinear(const Point * p);
	/*
	Returns code of a point p using the snake indexation pattern (P4)
	*/
	CODE HashCodeSnake(const Point * p);
};

CODE NodeGosperSFC :: HashCode(const Point * p) {
	switch (type) {
	case NodeGosperSFC_Center:
		return HashCodeCenter(p);
		break;
	case NodeGosperSFC_Precise:
		return HashCodePrecise(p);
		break;
	case NodeGosperSFC_Simple:
		return HashCodeSimple(p);
		break;
	case NodeGosperSFC_Linear:
		return HashCodeLinear(p);
		break;
	case NodeGosperSFC_Snake:
		return HashCodeSnake(p);
		break;
	default:
		return HashCodeCenter(p);
	}
}

CODE NodeGosperSFC :: HashCodeCenter(const Point * p, bool reverse)
{
	Point3D dc; //Decimal cube coordinates
	Int3D ic; //Integer cube coordinates

	int sign; //Sign of decimal residue
	int dominant; //Index of dominant axis
	CODE hexc = 0; //Final hash code
	CODE mini; //Hexagon index according to the center pattern

	//Array for mapping points onto the center pattern index
	const int arr[2][3] = { { 5, 1, 3 },{ 2, 4, 6 } };

	//Localization of a point p in the deepest hexagonal grid
	dc.x = (p->x * SQRT3_3 - p->y * F1_3) / smallHexSize;
	dc.z = p->y * F2_3 / smallHexSize;
	dc.y = -dc.x - dc.z;

	ic.x = RND(dc.x);
	ic.y = RND(dc.y);
	ic.z = RND(dc.z);

	dc.x = fabs(ic.x - dc.x);
	dc.y = fabs(ic.y - dc.y);
	dc.z = fabs(ic.z - dc.z);

	if (dc.x > dc.y && dc.x > dc.z)
		ic.x = -ic.y - ic.z; //x is the greatest
	else if (dc.y > dc.z)
		ic.y = -ic.x - ic.z; //y is the greatest
	else
		ic.z = -ic.x - ic.y; //z is the greatest

	//Loop through the hierarchy in the bottom-up manner
	for (int l = 0; l <= level; l++) {
		if (reverse)
			hexc <<= 3;
		
		//Transformation between hierarchical levels
		dc.x = ic.x + ic.x - ic.z;
		dc.x *= F1_7;
		dc.z = ic.x + ic.z + ic.z + ic.z;
		dc.z *= F1_7;
		dc.y = -dc.x - dc.z;

		//Round to integers
		ic.x = RND(dc.x);
		ic.y = RND(dc.y);
		ic.z = RND(dc.z);

		//Decimal residues
		dc.x = dc.x - ic.x;
		dc.y = dc.y - ic.y;
		dc.z = dc.z - ic.z;

		//Compute index of dominant axis in arr
		dominant = (fabs(dc.z) > fabs(dc.x)) + (fabs(dc.z) > fabs(dc.y));
		dominant = (dominant == 2 ? dominant : (fabs(dc.y) > fabs(dc.x)));

		//Is the sign negative or positive?
		sign = (dc.arr[dominant] < 0.f);

		//Center hexagon
		if (fabs(dc.arr[dominant]) < 0.00001f) {
			mini = 0;
		}
		else {
			//Get pattern index for the dominant axis and sign
			mini = arr[sign][dominant];
		}

		if (reverse)
			hexc |= (mini); //Mapping code indices (bottom-up)
		else
			hexc |= (mini) << (l + l + l); //Mapping code indices (top-down)
	}

	return hexc;
}

CODE NodeGosperSFC::HashCodeSimple(const Point * p)
{
	//Transformation table for the simple pattern (P2)
	const int idTrans[7] = { 4, 0, 1, 2, 3, 6, 5 };

	//Transform the indexation of the center pattern to the simple pattern
	CODE center = HashCodeCenter(p);
	CODE hexc = 0U;
	CODE id;
	for (int l = 0; l <= level; l++) {
		id = center & GetBitAndMask();
		center >>= GetBitShift();
		id = idTrans[id];
		hexc |= id << (l + l + l);
	}

	return hexc;
}
CODE NodeGosperSFC::HashCodeLinear(const Point * p)
{
	//Transformation table for the linear pattern (P3)
	const int idTrans[7] = { 3, 2, 0, 1, 4, 6, 5 };

	//Transform the indexation of the center pattern to the linear pattern
	CODE center = HashCodeCenter(p);
	CODE hexc = 0;
	CODE id;
	for (int l = 0; l <= level; l++) {
		id = center & GetBitAndMask();
		center >>= GetBitShift();
		id = idTrans[id];
		hexc |= id << (l + l + l);
	}
	return hexc;
}
CODE NodeGosperSFC::HashCodeSnake(const Point * p)
{
	//Transformation table for the linear pattern (P4)
	const int idTrans[7] = { 3, 4, 0, 1, 2, 6, 5 };

	//Transform the indexation of the center pattern to the snake pattern
	CODE center = HashCodeCenter(p);
	CODE hexc = 0;
	CODE id;
	for (int l = 0; l <= level; l++) {
		id = center & GetBitAndMask();
		center >>= GetBitShift();
		id = idTrans[id];
		hexc |= id << (l + l + l);
	}
	return hexc;
}

CODE NodeGosperSFC::HashCodePrecise(const Point * p)
{
	//Transformation table for the simple pattern (P2)
	const int idTrans[7] = { 4, 0, 1, 2, 3, 6, 5 };
	CODE center = HashCodeCenter(p, true); //Center code

	//Transform the indexation of the center pattern to the precise Node-Gosper indexation using the simple pattern
	CODE hexc = 0; //Final hash code
	int mini = 0; //Hexagon index in a pattern
	char rotDir = 0; // Index rotation: -1 (-120), 0, 1 (+120)
	char btf = 0; //Passage order: 0 - F (forward), 1 - B (backward)
	
	//Loop through the hierarchy in the top-down manner
	for (int l = 0; l <= level; l++) {
		//Unmask center pattern index
		hexc <<= GetBitShift();
		mini = center & GetBitAndMask();
		center >>= GetBitShift();

		//Rotate index
		if (mini && rotDir) {
			mini = mini + rotDir + rotDir;
			if (mini < 1) mini += 6;
			else if (mini > 6) mini -= 6;
		}

		//Transform the center pattern index to the simple pattern index
		mini = idTrans[mini];

		//Compute index rotation
		if ((mini == 0 || mini == 3) && --rotDir < -1) {
			rotDir = 1;
		}
		else if (mini == 5 && ++rotDir > 1) {
			rotDir = -1;
		}

		//Is it backward indexation?
		hexc |= (btf ? 6 - mini : mini);
		//Compute passage order
		if (mini == 0 || mini == 4 || mini == 5)
			btf = !btf;
	}

	return hexc;
}