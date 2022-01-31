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

/*
Quicksort
*/

template <typename T, class S> class Sorting
{
public:

	//COMMON
	static int partition(T* v1, int left, int right);
	static void quickSort(T *v1, int left, int right);
	static void quickSort(T *v1, S *v2, int left, int right);
	static void quickSort(T *v1, S **v2, int left, int right, const unsigned int v2length);
};

//COMMON
template<typename T, class S> int Sorting<T, S>::partition(T *v1, int left, int right)
{
	T tmpT;
	int tmpI;

	T x = v1[right];
	int i = left - 1;
	for (int j = left; j < right; j++)
	{
		if (v1[j] <= x)
		{
			i++;
			tmpT = v1[i];
			v1[i] = v1[j];
			v1[j] = tmpT;
		}
	}

	tmpI = i + 1;
	tmpT = v1[tmpI];
	v1[tmpI] = v1[right];
	v1[right] = tmpT;
	return i + 1;
}

template<typename T, class S> void Sorting<T, S>::quickSort(T *v1, int left, int right)
{
	////WITH PARTITION

	//if(left < right)
	//{
	//	int pivot = partition(v1,left , right);
	//	quickSort(v1, left, pivot-1);
	//	quickSort(v1, pivot+1 , right);
	//}

	//WITHOUT PARTITION
	int i = left, j = right;
	T tmp;
	T pivot = v1[(left + right) / 2];

	while (i <= j)
	{
		while (v1[i] < pivot)
			i++;
		while (v1[j] > pivot)
			j--;
		if (i <= j)
		{
			tmp = v1[i];
			v1[i] = v1[j];
			v1[j] = tmp;
			i++;
			j--;
		}
	}

	if (left < j)
		quickSort(v1, left, j);
	if (i < right)
		quickSort(v1, i, right);
}

template<typename T, class S> void Sorting<T, S>::quickSort(T *v1, S *v2, int left, int right)
{
	//if(left < right)
	//{
	//	int pivot = partition(v1,v2, left , right);
	//	quickSort(v1,v2, left, pivot-1);
	//	quickSort(v1,v2, pivot+1 , right);
	//}

	//WITHOUT PARTITION
	int i = left, j = right;
	T tmp;
	S tmpS;
	T pivot = v1[(left + right) / 2];

	while (i <= j)
	{
		while (v1[i] < pivot)
			i++;
		while (v1[j] > pivot)
			j--;
		if (i <= j)
		{
			tmp = v1[i];
			v1[i] = v1[j];
			v1[j] = tmp;

			tmpS = v2[i];
			v2[i] = v2[j];
			v2[j] = tmpS;

			i++;
			j--;
		}
	}

	if (left < j)
		quickSort(v1, v2, left, j);
	if (i < right)
		quickSort(v1, v2, i, right);
}

template<typename T, class S> void Sorting<T, S>::quickSort(T *v1, S **v2, int left, int right, const unsigned int v2length)
{
	//WITHOUT PARTITION
	int i = left, j = right;
	unsigned int k;
	T tmp;
	S tmpS;
	T pivot = v1[(left + right) / 2];

	while (i <= j)
	{
		while (v1[i] < pivot)
			i++;
		while (v1[j] > pivot)
			j--;
		if (i <= j)
		{
			tmp = v1[i];
			v1[i] = v1[j];
			v1[j] = tmp;

			for (k = 0; k<v2length; k++)
			{
				tmpS = v2[k][i];
				v2[k][i] = v2[k][j];
				v2[k][j] = tmpS;
			}

			i++;
			j--;
		}
	}

	if (left < j)
		quickSort(v1, v2, left, j, v2length);
	if (i < right)
		quickSort(v1, v2, i, right, v2length);
}
