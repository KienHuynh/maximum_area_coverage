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
	// Flag that marks if a point is in the frontier or not
	std::vector<bool> frontier;

	// Default constructor, create an empty point cloud
	PointCloud();
	// This constructor creates n random data points using the given random seed
	PointCloud(int _nPoint, int _rngSeed);

	// Auxilary functions
	// Insert a point to the cloud
	void insertPoint(float x, float y);
	// Remove the last point, note that it does not remove the first point (0,0)
	void popPoint();
	// Clear the cloud
	void clear();


	// Find max x and y of the points
	void findMax(float &x, float &y);

	// Sort the points to get their indices
	// The sorted indices will be stored in sortedIdX and sortedIdY
	void argSort();

	// Find the North-East frontier
	void findNEFrontier();

	// The main algorithm
	void maximumAreaCoverage();
};


#endif