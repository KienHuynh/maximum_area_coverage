#include "gui.h"

int Canvas::canvasWidth = 0;
int Canvas::canvasHeight = 0;

bool GUI::canvasRedrawSignal = 0;
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
	canvasRedrawSignal = 1;
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
	
}


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
		
		fl_color(FL_BLUE);
		fl_pie(x-5, y-5, 10, 10, 0, 360);

		ret = 1;
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


void Canvas::draw_coords() {
	// Coordinates as a string
	char s[80];
	sprintf_s(s, "x=%d y=%d", (int)Fl::event_x(), (int)Fl::event_y());
	// Black rect
	fl_color(FL_BLACK);
	fl_rectf(this->x(), this->y(), 120, 25);
	// White text
	fl_color(FL_WHITE);
	fl_font(FL_HELVETICA, 18);
	fl_draw(s, this->x() + 7, this->y() + 20);
}


void Canvas::draw() {
	if (GUI::canvasRedrawSignal) {
		this->redraw();
		GUI::canvasRedrawSignal = 0;
	}

	// User damage ONLY? just draw coords and done
	if (damage() == FL_DAMAGE_USER1) {
		draw_coords();
		return;
	}
	// Let group draw itself
	Fl_Group::draw();
	{
		static int redraws = 0;
		fl_color(FL_BLACK);
		fl_font(FL_COURIER, 80);
	}
	// Draw coords last
	draw_coords();
	drawLiveConcoms();
}


void Canvas::drawLiveConcoms() {
	
}


Canvas::Canvas(int X, int Y, int W, int H, const char *L = 0) : Fl_Group(X, Y, W, H, L) {
	canvas = new float**[H];

	imageBox = new Fl_Box(0, 0, W, H, "Canvas");
	imageBox->color(255);
	color(255);
}


Canvas::~Canvas() {
	delete imageBox;
}


GUI::GUI(int winWidth, int winHeight) {
	win = new Fl_Window(winWidth, winHeight);
	win->color(255);
	int menuBarWidth = 1080;
	int menuBarHeight = 25;

	Canvas::canvasWidth = winHeight - menuBarHeight;
	Canvas::canvasHeight = winHeight - menuBarHeight;

	int yButtonUnit = winHeight / 16;
	int xButtonUnit = winWidth / 8;

	Fl_Menu_Bar *menu = new Fl_Menu_Bar(0, 0, winWidth, menuBarHeight);
	menu->add("File/Open", FL_CTRL + 'o', browseCallback);
	menu->add("File/Save", FL_CTRL + 's', saveResultCallback);

	maximumAreaButt = new Fl_Button(Canvas::canvasWidth + xButtonUnit, menuBarHeight + yButtonUnit, 160, 25, "Estimate brush stroke");
	maximumAreaButt->callback(maximumAreaCallback);

	canvas = new Canvas(0, menuBarHeight, Canvas::canvasWidth, Canvas::canvasHeight, 0);

	win->resizable(win);
	win->show();
}


GUI::~GUI() {
	delete canvas;
	delete win;
	delete exitButt;
	delete maximumAreaButt;
}