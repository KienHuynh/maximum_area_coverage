#include "pointcloud.h"


Point2D::Point2D() {
	x = -1;
	y = -1;
}


Point2D::Point2D(float _x, float _y) {
	x = _x;
	y = _y;
}


float Point2D::area(Point2D p) {
	return abs((p.x - x)*(p.y - y));
}


Rectangle2D::Rectangle2D() {
	botLeft = Point2D(-1, -1);
	topRight = Point2D(-1, -1);
	instanced = false;
}


Rectangle2D::Rectangle2D(Point2D _botLeft, Point2D _topRight) {
	botLeft = _botLeft;
	topRight = _topRight;
	instanced = false;
}


void Rectangle2D::set(Point2D _botLeft, Point2D _topRight) {
	botLeft = _botLeft;
	topRight = _topRight;
}


bool Rectangle2D::intersectCheck(Rectangle2D r) {
	if (r.botLeft.x >= this->topRight.x) return false;
	if (r.botLeft.y >= this->topRight.y) return false;
	if (r.topRight.x <= this->botLeft.x) return false;
	if (r.topRight.y <= this->botLeft.y) return false;
	return true;
}


bool Rectangle2D::intersectCheck(std::vector<Rectangle2D> rv) {
	if (rv.size() == 0) return false;
	for (int i = 0; i < rv.size(); i++) {
		if (this->intersectCheck(rv[i])) return true;
	}
	return false;
}


float Rectangle2D::area() {
	return std::abs((topRight.y - botLeft.y)*(topRight.x - botLeft.x));
}


PointCloud::PointCloud() {
	points.push_back(Point2D(0, 0));
	rects.push_back(Rectangle2D(points[0], points[0]));
	upperBoundary.push_back(Point2D(1, 1));
	gridX.push_back(1);
	gridY.push_back(1);
	frontier.push_back(false);
}


PointCloud::PointCloud(int _nPpoints, int _rngSeed) {
	points.push_back(Point2D(0, 0));
	rects.push_back(Rectangle2D(points[0], points[0]));
	upperBoundary.push_back(Point2D(1, 1));
	gridX.push_back(1);
	gridY.push_back(1);
	frontier.push_back(false);
}


void PointCloud::insertPoint(float x, float y) {
	points.push_back(Point2D(x, y));
	frontier.push_back(false);
	rects.push_back(Rectangle2D(points[points.size() - 1], points[points.size() - 1]));
}


void PointCloud::popPoint() {
	if (points.size() > 1) {
		points.pop_back();
		frontier.pop_back();
	}
}


void PointCloud::clear() {
	points.clear();
	points.push_back(Point2D(0, 0));
	frontier.clear();
	resultPoints.clear();
	resultRects.clear();
	upperBoundary.clear();
	upperBoundary.push_back(Point2D(1, 1));
	frontier.push_back(false);
}


void PointCloud::findMax(float &x, float &y) {
	x = points[0].x;
	y = points[0].y;
	for (int i = 1; i < points.size(); i++) {
		if (x < points[i].x) x = points[i].x;
		if (y < points[i].y) y = points[i].y;
	}
}


void PointCloud::findNEFrontier() {
	std::vector<bool> xOrder, yOrder;
	xOrder = frontier;
	yOrder = frontier;

	for (int i = 0; i < points.size(); i++) {
		bool isFrontier = true;
		for (int j = 0; j < points.size(); j++) {
			if (i != j) {
				if (points[i].x < points[j].x && points[i].y < points[j].y) {
					isFrontier = false;
					break;
				}
			}
		}
		frontier[i] = isFrontier;
	}

	/*for (int i = 0; i < nPoint; i++) {
		frontier[i] = !(xOrder[i] && yOrder[i]);
	}*/
}


void PointCloud::argSort() {
	//TODO

}


void PointCloud::binaryInsert(std::vector<Point2D> &v, Point2D p, int lo, int hi) {
	if (hi <= lo) {
		if (p.x > v[lo].x) {
			v.insert(v.begin() + lo + 1, p);
		}
		else {
			v.insert(v.begin() + lo, p);
		}
		return;
	}

	int mid = (lo + hi) / 2;

	if (p.x == v[mid].x) {
		if (p.y < v[mid].y) v.insert(v.begin() + mid + 1, p);
		else if (p.y > v[mid].y) v.insert(v.begin() + mid, p);
		return;
	}

	if (p.x > v[mid].x)
		return binaryInsert(v, p, mid + 1, hi);
	return binaryInsert(v, p, lo, mid - 1);
}


void PointCloud::removePoint(std::vector<Point2D> &v, Point2D p) {
	for (int i = 0; i < v.size(); i++) {
		if (v[i].x == p.x && v[i].y == p.y) {
			v.erase(v.begin() + i);
			break;
		}
	}
}


std::vector<Point2D> PointCloud::enumerateGrid(std::vector<Point2D> points) {
	std::vector<Point2D> grid;
	for (int i = 0; i < points.size(); i++) {
		for (int j = 0; j < points.size(); j++) {
			grid.push_back(Point2D(points[i].x, points[j].y));
		}
	}
	return grid;
}


void PointCloud::upperBoundaryUpdate(std::vector<Point2D> &upperBoundary) {
	// First round, go from top left to bottom right
	Point2D latest = upperBoundary[0];
	std::vector<Point2D> newPoints;
	std::vector<int> indices;
	for (int i = 1; i < upperBoundary.size(); i++) {
		if (latest.x < upperBoundary[i].x) {
			if (i + 1 < upperBoundary.size()) {
				if (upperBoundary[i].y > latest.y && upperBoundary[i + 1].y < latest.y) {
					indices.push_back(i);
					newPoints.push_back(Point2D(upperBoundary[i].x, latest.y));
				}
			}
			
			if (latest.y > upperBoundary[i].y) latest = upperBoundary[i];
		}
		else {
			latest = upperBoundary[i];
		}
	}
	for (int i = indices.size() - 1; i >= 0; i--) {
		upperBoundary.insert(upperBoundary.begin() + indices[i], newPoints[i]);
	}

	newPoints.clear();
	indices.clear();
	// Second round, go from bottom right to top left
	latest = upperBoundary[upperBoundary.size()-1];
	for (int i = upperBoundary.size()-2; i >= 0; i--) {
		if (latest.y < upperBoundary[i].y) {
			if (i - 1 >= 0) {
				if (upperBoundary[i].x > latest.x && upperBoundary[i - 1].x < latest.x) {
					indices.push_back(i);
					newPoints.push_back(Point2D(latest.x, upperBoundary[i].y));
				}
			}

			if (latest.x > upperBoundary[i].x) latest = upperBoundary[i];
		}
		else {
			latest = upperBoundary[i];
		}
	}
	for (int i = 0; i < indices.size(); i++) {
		upperBoundary.insert(upperBoundary.begin() + indices[i], newPoints[i]);
	}

}


void PointCloud::greedySearch(std::vector<Point2D> &upperBoundary) {

	std::vector<Point2D> frontierPoints;
	int s = points.size();
	for (int i = s - 1; i >= 0; i--) {
		if (frontier[i]) {
			frontierPoints.push_back(points[i]);
			points.erase(points.begin() + i);
		}
	}

	while (frontierPoints.size() > 0) {
		int bestJ = 0, bestK = 0;
		float bestArea = 0;
		Rectangle2D bestRect(Point2D(0, 0), Point2D(0, 0));
		std::vector<Point2D> grid = enumerateGrid(upperBoundary);

		for (int j = 0; j < frontierPoints.size(); j++) {
			
			for (int k = 0; k < grid.size(); k++) {
				// Check if point k is to the right of j
				if (grid[k].x <= frontierPoints[j].x) continue;

				// Check if point k is above point j
				if (grid[k].y <= frontierPoints[j].y) continue;

				//if (upperBoundary[k].y > lowestY) break;
				Rectangle2D tmpRect = Rectangle2D(frontierPoints[j], grid[k]);

				float areaJK = frontierPoints[j].area(grid[k]);
				if (areaJK > bestArea && !tmpRect.intersectCheck(resultRects)) {
					bestJ = j;
					bestK = k;
					bestArea = areaJK;
					bestRect = tmpRect;
				}

				//if (upperBoundary[k].y < lowestY) lowestY = upperBoundary[k].y;
			}
		}
		resultPoints.push_back(frontierPoints[bestJ]);
		resultRects.push_back(bestRect);

		frontierPoints.erase(frontierPoints.begin() + bestJ);

		// Update the upper boundary
		//binaryInsert(upperBoundary, Point2D(bestRect.botLeft.x, bestRect.topRight.y), 0, upperBoundary.size() - 1);
		//binaryInsert(upperBoundary, Point2D(bestRect.botLeft.x, bestRect.botLeft.y), 0, upperBoundary.size() - 1);
		//binaryInsert(upperBoundary, Point2D(bestRect.topRight.x, bestRect.botLeft.y), 0, upperBoundary.size() - 1);
		upperBoundary.push_back(Point2D(bestRect.botLeft.x, bestRect.topRight.y));
		upperBoundary.push_back(Point2D(bestRect.botLeft.x, bestRect.botLeft.y));
		upperBoundary.push_back(Point2D(bestRect.topRight.x, bestRect.botLeft.y));
		removePoint(upperBoundary, Point2D(bestRect.topRight.x, bestRect.topRight.y));
		//binaryInsert(upperBoundary, Point2D(bestRect.topRight.x, bestRect.topRight.y), 0, upperBoundary.size() - 1);
		//upperBoundaryUpdate(upperBoundary);
	}
}


void PointCloud::maximumAreaCoverage() {
	// First iteration
	findNEFrontier();
	greedySearch(upperBoundary);
}