#include "pointcloud.h"


Point2D::Point2D(float _x, float _y) {
	x = _x;
	y = _y;
}


PointCloud::PointCloud() {
	nPoint = 1;
	points.push_back(Point2D(0, 0));
}


PointCloud::PointCloud(int _nPpoints, int _rngSeed) {
	nPoint = 1;
	points.push_back(Point2D(0, 0));
}


void PointCloud::insertPoint(float x, float y) {
	points.push_back(Point2D(x, y));
	nPoint++;
}


void PointCloud::popPoint() {
	if (nPoint > 1) {
		points.pop_back();
		nPoint--;
	}
}


void PointCloud::clear() {
	points.clear();
	points.push_back(Point2D(0, 0));
	nPoint = 1;
}


void PointCloud::argSort() {
	//TODO
}


void PointCloud::maximumAreaCoverage() {
	//TODO
}