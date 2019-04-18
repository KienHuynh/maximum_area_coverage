#pragma once
#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <fstream> 

#include <vector>
#include <algorithm>


// Simple class to store a 2D point
class Point2D {
public:	
	float x, y;
	Point2D(float _x, float _y);
};


// Storing the data points and other related meta data
class PointCloud {
public:
	int nPoint;
	std::vector<Point2D> points;
	// Store the sorted indices of the points according to their x-coords
	std::vector<int> sortedIdX;
	// Store the sorted indices of the points according to their y-coords
	std::vector<int> sortedIdY;

	// Default constructor, create an empty point cloud
	PointCloud();
	// This constructor creates n random data points using the given random seed
	PointCloud(int _nPoint, int _rngSeed);

	// Sort the points to get their indices
	// The sorted indices will be stored in sortedIdX and sortedIdY
	void argSort();
};


#endif