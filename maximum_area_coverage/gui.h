#pragma once
#ifndef GUI_H
#define GUI_H
#include <iostream>
#include <vector>
#include <queue>

// FLTK
#define WIN32
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Draw.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>

// GLOBALS
static Fl_Input        *G_inptitle = NULL;
static Fl_Input        *G_inpfilter = NULL;
static Fl_Input        *G_inpdirname = NULL;
static Fl_File_Chooser *G_chooser = NULL;


// The main class that display images and drawings
class Canvas : public Fl_Group {
protected:
	// Callback function that handles events through message e, inherited from the group class of FLTK
	int handle(int e);

	// Draw mouse coords in small black rectangle
	void draw_coords();

	// The main draw function of canvas
	void draw();

public:
	// Box to display the background image
	Fl_Box * imageBox;
	static int canvasWidth;
	static int canvasHeight;

	// The canvas to render the ink
	float*** canvas;

	// Drawing the current connected components in liveConComs, called when we do an undo or redo
	void drawLiveConcoms();
	Canvas(int X, int Y, int W, int H, const char *L);
	~Canvas();
	int mouseX, mouseY;
};


class GUI {
public:
	Canvas *canvas;

	Fl_Button *exitButt;
	Fl_Button *maximumAreaButt;
	Fl_Menu_Bar *menuBar;

	Fl_Window *win;

	static std::string canvasFileName;

	static bool canvasRedrawSignal;
	static bool imageRedrawSignal;

	GUI(int winWidth, int winHeight);
	~GUI();

	// Callback function for exit button
	static void exitCallback(Fl_Widget*w, void*data);
	// Callback function for browse button
	static void browseCallback(Fl_Widget*w, void*data);
	// Callback function for save button
	static void saveResultCallback(Fl_Widget*w, void*data);
	// Callback function for finding the maximum area coverage
	static void maximumAreaCallback(Fl_Widget*w, void*data);
};

#endif