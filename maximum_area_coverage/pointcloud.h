#ifndef POINTCLOUD_H
#define POINTCLOUD_H

#include <fstream> 

#include <vector>
#include <algorithm>
#include <math.h>


// Simple class to store a 2D point
class Point2D {
public:
	float x, y;
	Point2D();
	Point2D(float _x, float _y);

	// Compute the (absolute) rectangular area between this point and p
	float area(Point2D p);
};


// Simple class to store a rectangle
class Rectangle2D {
public:
	Point2D botLeft, topRight;
	bool instanced;

	Rectangle2D();
	Rectangle2D(Point2D _botLeft, Point2D _topRight);

	void set(Point2D _botLeft, Point2D _topRight);
	
	// Compute the area of a rect
	float area();

	// Check if this rect intersect with another
	// I'm walking on a thin line of floating point error here
	// Probably
	bool intersectCheck(Rectangle2D r);
	bool intersectCheck(std::vector<Rectangle2D> rv);
};


// Storing the data points and other related meta data
class PointCloud {
public:
	// The points
	std::vector<Point2D> points;
	// The rectangles associated with the points
	std::vector<Rectangle2D> rects;
	// The upper boundary
	std::vector<Point2D> upperBoundary;
	std::vector<float> gridX;
	std::vector<float> gridY;
	// The same variables to store the result
	std::vector<Point2D> resultPoints;
	std::vector<Rectangle2D> resultRects;

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

	// Binary insert to insert a 2D point into a vector w.r.t their x-coord
	void binaryInsert(std::vector<Point2D> &v, Point2D p, int lo, int hi);

	// Enumerate the grid points
	std::vector<Point2D> enumerateGrid(std::vector<Point2D> points);

	// Remove a point with particular coord
	void removePoint(std::vector<Point2D> &v, Point2D p);

	// Find the North-East frontier
	void findNEFrontier();

	// Update the upper boundary with new points by walking along the edges of the rectangles
	void upperBoundaryUpdate(std::vector<Point2D> &upperBoundary);

	// Using greedy to find the solution from a particular frontier and an upper boundary
	void greedySearch(std::vector<Point2D> &upperBoundary);

	// The main algorithm
	void maximumAreaCoverage();
};


#endif