#include "pointcloud.h"


Point2D::Point2D(float _x, float _y) {
	x = _x;
	y = _y;
}


PointCloud::PointCloud() {
	nPoint = 1;
	points.push_back(Point2D(0, 0));
	frontier.push_back(false);
}


PointCloud::PointCloud(int _nPpoints, int _rngSeed) {
	nPoint = 1;
	points.push_back(Point2D(0, 0));
	frontier.push_back(false);
}


void PointCloud::insertPoint(float x, float y) {
	points.push_back(Point2D(x, y));
	frontier.push_back(false);
	nPoint++;
}


void PointCloud::popPoint() {
	if (nPoint > 1) {
		points.pop_back();
		frontier.pop_back();
		nPoint--;
	}
}


void PointCloud::clear() {
	points.clear();
	points.push_back(Point2D(0, 0));
	frontier.clear();
	frontier.push_back(false);
	nPoint = 1;
}


void PointCloud::findMax(float &x, float &y) {
	x = points[0].x;
	y = points[0].y;
	for (int i = 1; i < nPoint; i++) {
		if (x < points[i].x) x = points[i].x;
		if (y < points[i].y) y = points[i].y;
	}
}


void PointCloud::findNEFrontier() {
	std::vector<bool> xOrder, yOrder;
	xOrder = frontier;
	yOrder = frontier;
	
	for (int i = 0; i < nPoint; i++) {
		for (int j = 0; j < nPoint; j++) {
			if (i != j) {
				if (points[i].x < points[j].x) {
					xOrder[i] = true;
				}
				if (points[i].y < points[j].y) {
					yOrder[i] = true;
				}
				if (xOrder[i] && yOrder[i]) break;
			}
		}
	}

	for (int i = 0; i < nPoint; i++) {
		frontier[i] = !(xOrder[i] && yOrder[i]);
	}
}


void PointCloud::argSort() {
	//TODO

}


void PointCloud::maximumAreaCoverage() {
	//TODO
	findNEFrontier();
}