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


bool Point2D::compareX(Point2D a, Point2D b) {
	if (a.x < b.x || (a.x == b.x && a.y < b.y)) return true;
	else if (a == b) return false;
	return false;
}


bool Point2D::compareY(Point2D a, Point2D b) {
	if (a.y < b.y || (a.y == b.y && a.x < b.x)) return true;
	else if (a == b) return false;
	return false;
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


Polygon2D::Polygon2D() {

}


void Polygon2D::set(Rectangle2D r) {
	points.push_back(r.botLeft);
	points.push_back(Point2D(r.topRight.x, r.botLeft.y));
	points.push_back(r.topRight);
	points.push_back(Point2D(r.botLeft.x, r.topRight.y));
}


Polygon2D::Polygon2D(Rectangle2D r) {
	set(r);
}


Polygon2D Polygon2D::rect2Poly(Rectangle2D r) {
	Polygon2D p(r);
	return p;
}


Position Polygon2D::edgeTouch(Point2D a, Point2D b, Point2D c, Point2D d) {
	// TODO: check special case where b == c

	if (a.x == b.x) {
		if (c.x == d.x && a.x == c.x) {
			if (d.y - c.y > 0) return left;
			else return right;
		}
		else {
			// ab is vertical but cd is horizontal
			return nope;
		}
	}
	else { // a.y == b.y
		if (c.y == d.y && a.y == c.y) {
			if (d.x - c.x > 0) return above;
			else return below;
		}
		else {
			return nope;
		}
	}
	return nope;
}


bool Polygon2D::edgeAlign(Point2D a, Point2D b, Point2D c, Point2D d) {
	if (b.x == c.x && b.y == c.y) {
		if (a.x == b.x) { // Vertical
			if ((b.y - a.y) * (d.y - c.y) > 0) return true;
			else return false;
		}
		else { // Horizontal
			if ((b.x - a.x) * (d.x - c.x) > 0) return true;
			else return false;
		}
	}
	return false;
}


void Polygon2D::merge(Polygon2D p) {
	if (points.size() == 0) {
		points = p.points;
		return;
	}
	// Remove same points between the 2 polygons
	std::vector<int> marker;
	for (int i = 0; i < points.size(); i++) {
		for (int j = 0; j < p.points.size(); j++) {
			if (points[i] == p.points[j]) {
				marker.push_back(i);
				p.points.erase(p.points.begin() + j);
				break;
			}
		}
	}

	for (int i = marker.size()-1; i >= 0; i--) {
		points.erase(points.begin() + marker[i]);
	}

	for (int i = 0; i < p.points.size(); i++) {
		points.push_back(p.points[i]);
	}

	std::vector<Point2D> sortedX = points, sortedY = points;
	std::map<Point2D, Point2D> edgeH, edgeV;

	std::sort(sortedX.begin(), sortedX.end(), Point2D::compareX);
	std::sort(sortedY.begin(), sortedY.end(), Point2D::compareY);

	// Enumerate the horizontal edges
	for (int i = 0; i < points.size(); ) {
		float currY = sortedY[i].y;
		
		for (; i < points.size() && sortedY[i].y == currY; i = i + 2) {
			edgeH.insert(std::pair<Point2D, Point2D>(sortedY[i], sortedY[i+1]));
			edgeH.insert(std::pair<Point2D, Point2D>(sortedY[i+1], sortedY[i]));
		}
	}

	// Enumerate the vertical edges
	for (int i = 0; i < points.size(); ) {
		float currX = sortedX[i].x;

		for (; i < points.size() && sortedX[i].x == currX; i = i + 2) {
			edgeV.insert(std::pair<Point2D, Point2D>(sortedX[i], sortedX[i + 1]));
			edgeV.insert(std::pair<Point2D, Point2D>(sortedX[i + 1], sortedX[i]));
		}
	}

	std::vector<std::vector<Point2D>> allPoly;
	
	while (edgeH.size() > 0) {
		std::vector<Point2D> poly;
		poly.push_back(edgeH.begin()->first);
		edgeH.erase(edgeH.begin());
		
		for (int i = 0; ; i++) {
			Point2D latest = poly[poly.size() - 1];
			Point2D next;
			if (i % 2 == 0) {
				next = edgeV[latest];
				edgeV.erase(latest);
			}
			else {
				next = edgeH[latest];
				edgeH.erase(latest);
			}
			poly.push_back(next);
			if (poly[0] == poly[poly.size() - 1]) {
				poly.pop_back();
				break;
			}
		}
		for (int i = 0; i < poly.size(); i++) {
			edgeH.erase(poly[i]);
			edgeV.erase(poly[i]);
		}

		allPoly.push_back(poly);
	}
	
	bool stop = false;
	for (int i = 0; i < allPoly.size() && !stop; i++) {
		Point2D tmp = Point2D(1, 1);
		for (int j = 0; j < allPoly[i].size(); j++) {
			if (allPoly[i][j] == tmp) {
				points = allPoly[i];
				stop = true;
				break;
			}
		}
	}
}


void Polygon2D::merge(Rectangle2D r) {
	Polygon2D p(r);
	merge(p);
}


void Polygon2D::clear() {
	points.clear();
}


PointCloud::PointCloud() {
	points.push_back(Point2D(0, 0));
	rects.push_back(Rectangle2D(points[0], points[0]));
	upperBoundary.push_back(Point2D(1, 1));
	frontier.push_back(false);
}


PointCloud::PointCloud(int _nPpoints, int _rngSeed) {
	points.push_back(Point2D(0, 0));
	rects.push_back(Rectangle2D(points[0], points[0]));
	upperBoundary.push_back(Point2D(1, 1));
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
	boundingPoly.clear();
	
}


void PointCloud::randomGen(int n) {
	clear();
	for (int i = 0; i < n; i++) {
		float x = 0;
		float y = 0;
		while (x == 0 && y == 0) {
			x = ((float) (rand() % 10000)) / 10000.0;
			y = ((float) (rand() % 10000)) / 10000.0;
		}
		insertPoint(x, y);
	}
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


void PointCloud::loadFromFile(const char* fname) {
	std::ifstream myfile;
	myfile.open(fname, std::ios::in);
	int nPoints = 0, *conComSizes = NULL, dump;

	myfile >> nPoints;
	for (int i = 0; i < nPoints; i++) {
		float x, y;
		myfile >> x >> y;
		insertPoint(x, y);
	}

	myfile.close();
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


std::set<Point2D> PointCloud::enumerateGrid(std::vector<Point2D> points) {
	std::set<Point2D> grid;
	for (int i = 0; i < points.size(); i++) {
		for (int j = 0; j < points.size(); j++) {
			grid.insert(Point2D(points[i].x, points[j].y));
		}
	}
	return grid;
}


void PointCloud::removeHoles(std::vector<Point2D> &upperBoundary) {
	// Find the outline of the rectangle group
	// Starting from (1,1) because it will always be there.
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
		int bestJ = 0;
		float bestArea = 0;
		Rectangle2D bestRect(Point2D(0, 0), Point2D(0, 0));
		std::set<Point2D> grid = enumerateGrid(upperBoundary);

		for (int j = 0; j < frontierPoints.size(); j++) {
			
			for (std::set<Point2D>::iterator it = grid.begin(); it != grid.end(); it++) {
				// Check if point k is to the right of j
				if (it->x <= frontierPoints[j].x) continue;

				// Check if point k is above point j
				if (it->y <= frontierPoints[j].y) continue;

				//if (upperBoundary[k].y > lowestY) break;
				Rectangle2D tmpRect = Rectangle2D(frontierPoints[j], (*it));

				float areaJK = frontierPoints[j].area((*it));
				if (areaJK > bestArea && !tmpRect.intersectCheck(resultRects)) {
					bestJ = j;
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
		//upperBoundary.push_back(Point2D(bestRect.botLeft.x, bestRect.topRight.y));
		//upperBoundary.push_back(Point2D(bestRect.botLeft.x, bestRect.botLeft.y));
		//upperBoundary.push_back(Point2D(bestRect.topRight.x, bestRect.botLeft.y));
		//removePoint(upperBoundary, Point2D(bestRect.topRight.x, bestRect.topRight.y));
		boundingPoly.merge(bestRect);
		upperBoundary = boundingPoly.points;

		//binaryInsert(upperBoundary, Point2D(bestRect.topRight.x, bestRect.topRight.y), 0, upperBoundary.size() - 1);
		//upperBoundaryUpdate(upperBoundary);
	}
}


void PointCloud::maximumAreaCoverage() {
	// First iteration
	findNEFrontier();
	greedySearch(upperBoundary);
}


float PointCloud::area() {
	float total = 0;
	for (int i = 0; i < resultRects.size(); i++) {
		total += resultRects[i].area();
	}
	return total;
}