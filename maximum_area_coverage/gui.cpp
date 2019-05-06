#include "gui.h"

int Canvas::canvasWidth = 0;
int Canvas::canvasHeight = 0;
PointCloud Canvas::pCloud;

bool GUI::smallRedrawSignal = 0;
bool GUI::bigRedrawSignal = 0;
bool GUI::imageRedrawSignal = 0;
std::string GUI::canvasFileName = "";


void GUI::exitCallback(Fl_Widget*w, void*data) {
	exit(0);
}


void GUI::browseCallback(Fl_Widget*w, void*data) {
	if (!G_chooser) {
		G_chooser = new Fl_File_Chooser("", "", Fl_File_Chooser::SINGLE, "");
	}

	G_chooser->directory(NULL);
	G_chooser->filter(NULL);
	G_chooser->type(Fl_File_Chooser::SINGLE);
	G_chooser->label(NULL);
	G_chooser->show();

	// Block until user picks something.
	//     (The other way to do this is to use a callback())
	//
	while (G_chooser->shown()) {
		Fl::wait();
	}

	// Print the results
	if (G_chooser->value() == NULL) {
		fprintf(stderr, "(User hit 'Cancel')\n");
		return;
	}

	fprintf(stderr, "DIRECTORY: '%s'\n", G_chooser->directory());
	fprintf(stderr, "    VALUE: '%s'\n", G_chooser->value());
	fprintf(stderr, "    COUNT: %d files selected\n", G_chooser->count());


	// Flag the signal so that the image box will redraw
	smallRedrawSignal = 1;
	imageRedrawSignal = 1;

	fprintf(stderr, "--------------------\n");
}


void GUI::saveResultCallback(Fl_Widget*w, void*data) {
	if (!G_chooser) {
		G_chooser = new Fl_File_Chooser("", "", Fl_File_Chooser::CREATE, "");
	}

	G_chooser->directory(NULL);
	G_chooser->filter(NULL);
	G_chooser->type(Fl_File_Chooser::CREATE);
	G_chooser->label(NULL);
	G_chooser->show();

	// Block until user picks something.
	//     (The other way to do this is to use a callback())
	//
	while (G_chooser->shown()) {
		Fl::wait();
	}

	// Print the results
	if (G_chooser->value() == NULL) {
		fprintf(stderr, "(User hit 'Cancel')\n");
		return;
	}

	fprintf(stderr, "DIRECTORY: '%s'\n", G_chooser->directory());
	fprintf(stderr, "    VALUE: '%s'\n", G_chooser->value());
	fprintf(stderr, "    COUNT: %d files selected\n", G_chooser->count());
	fprintf(stderr, "--------------------\n");
}


void GUI::maximumAreaCallback(Fl_Widget*w, void*data) {
	Canvas::pCloud.maximumAreaCoverage();
	GUI::bigRedrawSignal = true;
}


void GUI::cloudClearCallback(Fl_Widget*w, void*data) {
	Canvas::pCloud.clear();
	GUI::bigRedrawSignal = 1;
}


void GUI::randomCallback(Fl_Widget*w, void*data) {
	Canvas::pCloud.randomGen(100);
	GUI::bigRedrawSignal = 1;
}


void Canvas::fl_normal_line(float x, float y, float x1, float y1) {
	fl_line(x, canvasHeight-y, x1, canvasHeight-y1);
}


// Function to handle mouse events such as drag, release, etc.
int Canvas::handle(int e) {
	int ret = Fl_Group::handle(e);
	switch (e) {

	// Mouse events
	case FL_DRAG: {
		ret = 0;
		break;
	}
	case FL_RELEASE: {
		float x = (int)Fl::event_x();
		float y = (int)Fl::event_y();
		
		// Convert x and y to normalized coord
		y = canvasHeight - y;
		x = (x - origin) / (float)squareLength;
		y = (y - origin) / (float)squareLength;

		if (!(x < 0 || y < 0 || x > 1 || y > 1)) {
			// Add point to the point cloud data
			pCloud.insertPoint(x, y);
		}

		ret = 1;
		GUI::smallRedrawSignal = 1;
		break;
	}
	case FL_PUSH: {
		// Has to return 1 so to make it clear that this widget "wants" to be clicked.
		// Otherwise FL_DRAG and such will be ignored
		ret = 1;
		break;
	}
	case FL_ENTER:
	{
		mouseX = (int)Fl::event_x();
		mouseY = (int)Fl::event_y();
		ret = 1;                // FL_ENTER: must return(1) to receive FL_MOVE
		break;
	}

	case FL_MOVE:
	{
		mouseX = (int)Fl::event_x();
		mouseY = (int)Fl::event_y();
		// FL_MOVE: mouse movement causes 'user damage' and redraw..
		damage(FL_DAMAGE_USER1);
		ret = 1;
		break;
	}

	// Keyboard events
	case FL_FOCUS:
	case FL_UNFOCUS:
		ret = 1;                // enables receiving keyboard events
		break;
	case FL_SHORTCUT:           // incase widget that isn't ours has focus
	case FL_KEYDOWN:
	{
		int key = Fl::event_key();
		if (key == 'z') {
			//TODO: Implement undo
			this->redraw();
			//this->draw();
			ret = 1;
		}
		else {
			ret = 0;
		}
		if (key == 'y') {
			//TODO: Implement redo
			ret = 0;
		}

		break;
	}

	}
	return(ret);
}


void Canvas::float2CanvasCoord(float &x, float &y) {
	x = ceil(x * (float)squareLength + origin + 0.5);
	y = ceil(canvasHeight - (y * (float)squareLength + origin) + 0.5);
}


void Canvas::drawPoints() {
	fl_color(fl_rgb_color(255, 150, 50));
	for (int i = 0; i < pCloud.points.size(); i++) {
		float x = pCloud.points[i].x;
		float y = pCloud.points[i].y;

		float2CanvasCoord(x, y);
		fl_pie(((int) x) - 5, ((int) y) - 5, 10, 10, 0, 360);
	}

	/*fl_color(fl_rgb_color(50, 255, 150));
	for (int i = 0; i < pCloud.resultPoints.size(); i++) {
		float x = pCloud.resultPoints[i].x;
		float y = pCloud.resultPoints[i].y;

		float2CanvasCoord(x, y);
		fl_pie(((int)x) - 5, ((int)y) - 5, 10, 10, 0, 360);
	}*/

	fl_color(fl_rgb_color(255, 0, 255));
	for (int i = 0; i < pCloud.upperBoundary.size(); i++) {
		float x = pCloud.upperBoundary[i].x;
		float y = pCloud.upperBoundary[i].y;

		float2CanvasCoord(x, y);
		fl_pie(((int)x) - 5, ((int)y) - 5, 10, 10, 0, 360);
	}
}


void Canvas::drawRects() {
	
	for (int i = 0; i < pCloud.resultRects.size(); i++) {
		fl_color(fl_rgb_color(50, 50, 150));
		float x1 = pCloud.resultRects[i].botLeft.x;
		float y1 = pCloud.resultRects[i].topRight.y;
		float x2 = pCloud.resultRects[i].topRight.x;
		float y2 = pCloud.resultRects[i].botLeft.y;

		float2CanvasCoord(x1, y1);
		float2CanvasCoord(x2, y2);

		fl_rectf((int)x1, (int)y1, (int)(x2 - x1), (int)(y2 - y1));

		fl_color(fl_rgb_color(50, 50, 50));
		fl_begin_loop();
		fl_vertex(x1, y1);
		fl_vertex(x2, y1);
		fl_vertex(x2, y2);
		fl_vertex(x1, y2);
		fl_end_loop();
	}
}


void Canvas::drawCoords() {
	// Coordinates as a string
	char s[80];
	float x = (float)Fl::event_x();
	float y = canvasHeight - (float)Fl::event_y();
	x = (x - origin) / (float)squareLength;
	y = (y - origin) / (float)squareLength;
	sprintf_s(s, "x=%.3f y=%.3f", x, y);
	// Black rect
	fl_color(FL_BLACK);
	fl_rectf(Canvas::canvasWidth-180, this->y(), 180, 25);
	// White text
	fl_color(FL_WHITE);
	fl_font(FL_HELVETICA, 18);
	fl_draw(s, Canvas::canvasWidth - 180 + 7, this->y() + 20);

}


void Canvas::drawArea() {
	// Coordinates as a string
	char s[80];
	
	sprintf_s(s, "area=%.5f", pCloud.area());
	// Black rect
	fl_color(FL_BLACK);
	fl_rectf(Canvas::canvasWidth - 180, this->y()+25, 180, 25);
	// White text
	fl_color(FL_WHITE);
	fl_font(FL_HELVETICA, 18);
	fl_draw(s, Canvas::canvasWidth - 180 + 7, this->y() + 45);
}


void Canvas::drawAxes() {
	// Draw the square
	char dashStyle[3] = { 10,5,0 };
	fl_color(fl_rgb_color(100, 100, 255));
	fl_line_style(FL_DASH, 2, dashStyle);
	fl_normal_line(origin, origin, origin, origin + squareLength);
	fl_normal_line(origin, origin, origin + squareLength, origin);
	fl_normal_line(origin + squareLength, origin, origin + squareLength, origin + squareLength);
	fl_normal_line(origin, origin + squareLength, origin + squareLength, origin + squareLength);

	// Draw the axes
	fl_color(fl_rgb_color(50, 50, 200));
	fl_line_style(FL_SOLID, 2);

	// Horizontal axis
	fl_normal_line(origin, origin, origin + axisLength, origin);
	// Arrow tip
	fl_normal_line(origin + axisLength, origin, origin + axisLength - 7, origin - 5);
	fl_normal_line(origin + axisLength, origin, origin + axisLength - 7, origin + 5);

	// Vertical axis
	fl_normal_line(origin, origin, origin, origin + axisLength);
	// Arrow tip
	fl_normal_line(origin, origin + axisLength, origin - 5, origin + axisLength - 7);
	fl_normal_line(origin, origin + axisLength, origin + 5, origin + axisLength - 7);

}


void Canvas::draw() {
	if (GUI::smallRedrawSignal) {
		this->redraw();
		GUI::smallRedrawSignal = 0;
	}

	if (GUI::bigRedrawSignal) {
		// Not sure how do you clear drawn objects in FLTK so I just draw a white rectangle instead
		// TODO: find out how to properly clear things
		fl_color(255);
		fl_rectf(0, 0, canvasWidth, canvasHeight);
		this->redraw();
		GUI::bigRedrawSignal = 0;
	}

	// User damage ONLY? just draw coords and done
	if (damage() == FL_DAMAGE_USER1) {
		drawCoords();
		return;
	}
	// Let group draw itself
	Fl_Group::draw();
	{
		static int redraws = 0;
		fl_color(FL_BLACK);
		fl_font(FL_COURIER, 80);
	}


	drawAxes();
	drawCoords();
	drawArea();
	drawRects();
	drawPoints();
}


Canvas::Canvas(int X, int Y, int W, int H, const char *L = 0) : Fl_Group(X, Y, W, H, L) {
	canvas = new float**[H];

	// Create a white background
	imageBox = new Fl_Box(0, 0, W, H, "Canvas");
	imageBox->color(255);
	color(255);

	// TODO: Create the x / y axis
	axisLength = ((W < H ? W : H)) * 0.9;
	origin = ((W < H ? W : H)) * 0.05;
	squareLength = ((W < H ? W : H)) * 0.8;
}


Canvas::~Canvas() {
	delete imageBox;
}


GUI::GUI(int winWidth, int winHeight) {
	// Init the main window
	win = new Fl_Window(winWidth, winHeight);
	win->color(255);

	// Init the top menu bar
	int menuBarWidth = 1080;
	int menuBarHeight = 25;

	// Init the canvas for drawing / displaying points
	Canvas::canvasWidth = winHeight - menuBarHeight;
	Canvas::canvasHeight = winHeight - menuBarHeight;

	// Init the width/height unit of the buttons
	int yButtonUnit = winHeight / 16;
	int xButtonUnit = winWidth / 8;

	// Init the menu bar and create related menu options
	Fl_Menu_Bar *menu = new Fl_Menu_Bar(0, 0, winWidth, menuBarHeight);
	menu->add("File/Open", FL_CTRL + 'o', browseCallback);
	menu->add("File/Save", FL_CTRL + 's', saveResultCallback);

	// Assign callbacks to corresponding buttons
	maximumAreaBu = new Fl_Button(Canvas::canvasWidth + xButtonUnit, menuBarHeight + yButtonUnit, 160, 25, "Run");
	maximumAreaBu->callback(maximumAreaCallback);
	clearBu = new Fl_Button(Canvas::canvasWidth + xButtonUnit, menuBarHeight + yButtonUnit*2, 160, 25, "Clear");
	clearBu->callback(cloudClearCallback);
	randomBu = new Fl_Button(Canvas::canvasWidth + xButtonUnit, menuBarHeight + yButtonUnit * 3, 160, 25, "Random");
	randomBu->callback(randomCallback);

	// Create  the actual canvas
	canvas = new Canvas(0, menuBarHeight, Canvas::canvasWidth, Canvas::canvasHeight, 0);

	win->resizable(win);
	win->show();
}


GUI::~GUI() {
	delete canvas;
	delete win;
	delete exitBu;
	delete maximumAreaBu;
}