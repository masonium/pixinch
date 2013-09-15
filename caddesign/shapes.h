#pragma once

#include <string>
#include <vector>
using std::vector;
using std::string;

struct vec3
{
  double x, y, z;
};

struct quad
{
  int f[4];
};

/** All measurements are in mm. **/
const double GRID_WIDTH = 31.25;
const double SLOT_WIDTH = 2;
const double SLOT_OVERHANG_WIDTH = 10;
const double SLOT_OVERHANG_DEPTH = 10;
const double BRIDGE_DEPTH = 40;

const double BOARD_DEPTH = 10;
const double GROOVE_DEPTH = 3;
const double GROOVE_WIDTH = 15;

const double BOARD_HOLE_HEIGHT = 20;

const double BOARD_TAB_WIDTH = 20;
const double BOARD_UNDERHANG = 100;

const int GRID_SIZE = 32;
const double BOARD_FRAME_WIDTH = GRID_WIDTH * GRID_SIZE;

void extrude_points( vector<vec3>& points, double depth );
void extrude_faces( vector<quad>& quads, int eo);

void write_mesh( const std::string& filename, const vector<vec3>& points,
                 const vector<quad>& quads );


void grid_strip(bool bottom, const std::string& filename);
void light_board(const std::string& filename);
