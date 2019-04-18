#include "pointcloud.h"


Point2D::Point2D(float _x, float _y) {
	x = _x;
	y = _y;
}


PointCloud::PointCloud() {
	nPoint = 0;
	sortedIdX.push_back(1);
}


PointCloud::PointCloud(int _nPpoints, int _rngSeed) {
	//TODO
}


void PointCloud::argSort() {
	//TODO
}