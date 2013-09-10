#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <string>

using std::vector;
using std::ofstream;
using std::ostream;

const double GRID_WIDTH = 31.25;
const double SLOT_WIDTH = 2;
const double SLOT_OVERHANG_WIDTH = 10;
const double SLOT_OVERHANG_DEPTH = 10;
const double BRIDGE_DEPTH = 40;

const double BOARD_DEPTH = 10;
const double BOARD_GROOVE_DEPTH = 3;
const double BOARD_GROOVE_HEIGHT = 15;
const double BOARD_HOLE_WIDTH = 20;

const double BOARD_OVERHANG = 30;

const int GRID_SIZE = 32;
const double BOARD_FRAME_WIDTH = GRID_WIDTH * GRID_SIZE;

struct vec3
{
  double x, y, z;
};

int mod1(int a, int m)
{
  return (a-1) % m + 1;
}
void print_vertex( ostream&out, const vec3& v )
{
  out << "v " << v.x << " " << v.y << " " << v.z << "\n";
}

void print_face( ostream& out, const vector<int>& indices )
{
  out << "f";
  for (size_t i = 0; i < indices.size(); ++i) {
    out << " " << indices[i];
  }
  out << "\n";
}

void plot_extruded_points( const vector<vec3>& points,
                           double width, 
                           const std::string& filename )
{
  const size_t num_front_side = points.size();

  ofstream out(filename);
  
  for (size_t i = 0; i < points.size(); ++i)
    print_vertex(out, points[i]);
  
  for (size_t i = 0; i < points.size(); ++i)
    print_vertex(out, {points[i].x, points[i].y, width});

  for (size_t i = 0; i < num_front_side; ++i)
  {
    out << "f " << mod1(i+2, num_front_side)
        << " " << mod1(i+1, num_front_side)
        << " " << (mod1(i+1, num_front_side) + num_front_side)
        << " " << (mod1(i+2, num_front_side) + num_front_side) << "\n";
  }
  
  out << "f";
  for (size_t i = 0; i < num_front_side; ++i)
    out << " " << (i + 1);
  out << "\n";
  
  out << "f";
  for (int i = num_front_side - 1; i >= 0; --i)
    out << " " << (i + 1 + num_front_side);
  out << "\n";
}

/***
 * Construct the points for the nth groove in the light board.
 * Points are from left to right, low to high, and low depth to high depth.
 */
void light_board_groove_vertices(vector<vec3>& points, int i)
{
  const double inner_hole_left = -BOARD_HOLE_WIDTH * 0.5;
  const double inner_hole_right = BOARD_HOLE_WIDTH * 0.5;
  const double outer_hole_left = BOARD_FRAME_WIDTH - BOARD_HOLE_WIDTH * 0.5;
  const double outer_hole_right = BOARD_FRAME_WIDTH + BOARD_HOLE_WIDTH * 0.5;

  double low_y = (i + 0.5) * GRID_SIZE - BOARD_GROOVE_HEIGHT * 0.5;
  double high_y = (i + 0.5) * GRID_SIZE + BOARD_GROOVE_HEIGHT * 0.5;

  points.push_back( {inner_hole_left, low_y, 0.0 });
  points.push_back( {inner_hole_left, high_y, 0.0 });
  points.push_back( {inner_hole_left, low_y, BOARD_DEPTH });
  points.push_back( {inner_hole_left, high_y, BOARD_DEPTH });
    
  points.push_back( {inner_hole_right, low_y, 0.0 });
  points.push_back( {inner_hole_right, high_y, 0.0 });
  points.push_back( {inner_hole_right, low_y, BOARD_DEPTH - BOARD_GROOVE_DEPTH });
  points.push_back( {inner_hole_right, high_y, BOARD_DEPTH - BOARD_GROOVE_DEPTH });
  points.push_back( {inner_hole_right, low_y, BOARD_DEPTH });
  points.push_back( {inner_hole_right, high_y, BOARD_DEPTH });
    
  points.push_back( {outer_hole_left, low_y, 0.0 });
  points.push_back( {outer_hole_left, high_y, 0.0 });
  points.push_back( {outer_hole_left, low_y, BOARD_DEPTH - BOARD_GROOVE_DEPTH });
  points.push_back( {outer_hole_left, high_y, BOARD_DEPTH - BOARD_GROOVE_DEPTH });
  points.push_back( {outer_hole_left, low_y, BOARD_DEPTH });
  points.push_back( {outer_hole_left, high_y, BOARD_DEPTH });
  
  points.push_back( {outer_hole_right, low_y, 0.0 });
  points.push_back( {outer_hole_right, high_y, 0.0 });
  points.push_back( {outer_hole_right, low_y, BOARD_DEPTH });
  points.push_back( {outer_hole_right, high_y, BOARD_DEPTH });

}

/***
 * Print out the inner faces for a specific light board groove.
 */
void light_board_groove_faces(ostream& out, int i)
{
  const int GROOVE_VERTICES = 20;
  const int offset = GROOVE_VERTICES * i + 1;

  auto print_offset_face = [&](int offset, int a, int b, int c, int d) {
    print_face(out, {offset + a, offset + b, offset + c, offset + d });
  };

  print_offset_face(offset, 0, 1, 3, 2);
  print_offset_face(offset, 4, 6, 7, 5);
  print_offset_face(offset, 0, 2, 8, 4);
  print_offset_face(offset, 1, 5, 9, 3);

  print_offset_face(offset, 6, 8, 14, 12);
  print_offset_face(offset, 6, 12, 13, 7);
  print_offset_face(offset, 7, 13, 15, 9);

  print_offset_face(offset, 10, 11, 13, 12);
  print_offset_face(offset, 10, 14, 18, 16);
  print_offset_face(offset, 11, 17, 19, 15);
}

/***
 * Construct the points for the light board.
 * The light board is arranged so that the grooves are parallel to the axis is
 * and the depth of the board is along the z-axis.
 */
void light_board(const std::string& filename)
{
  vector<vec3> inner_points;
  vector<vec3> outer_points;
  
  for (int i = 0; i < GRID_SIZE; ++i)
    light_board_groove_vertices(inner_points, i);
  
  const double left_edge = -BOARD_OVERHANG;
  const double right_edge = BOARD_FRAME_WIDTH + BOARD_OVERHANG;
  
  // NOTE: This only works because the grid is square.
  const double bottom_edge = left_edge; 
  const double top_edge = right_edge;
  
  const double zs[2] = {0.0f, BOARD_DEPTH };

  /* Outer 'C'-points of the grid. */
  for (int zi = 0; zi < 2; ++zi)
  {
    double z = zs[zi];
    outer_points.push_back( {right_edge, bottom_edge, z} );
    outer_points.push_back( {left_edge, bottom_edge, z} );
    
    for (int y = 0; y <= GRID_SIZE; ++y)
      outer_points.push_back( {left_edge, y * GRID_WIDTH } );
    
    outer_points.push_back( {left_edge, top_edge, z} );    
    outer_points.push_back( {right_edge, top_edge, z} );
  }

  /** 
   * Write out the model. First the groove verices, then the outer vertices,
   * then the faces formed by the grooves, and finally the faces outside of the
   * groves.
   */
  ofstream out(filename.c_str());

  for (size_t i = 0; i < inner_points.size(); ++i)
    print_vertex(out, inner_points[i]);
  for (size_t i = 0; i < outer_points.size(); ++i)
    print_vertex(out, outer_points[i]);
  
  /** groove faces */
  for (int i = 0; i < GRID_SIZE; ++i)
    light_board_groove_faces(out, i);

  // top face
  out << "f";
  const int GROOVE_VERTICES = inner_points.size() / GRID_SIZE;
  for (int i = 0; i < GRID_SIZE; ++i)
  {
    const int offset = i * GROOVE_VERTICES + 1;
    out << " " << (offset + 18) << " " << (offset + 2) 
        << " " << (offset + 3) << " " << (offset + 19);
  }
  
  {
    const int outer_top_offset = inner_points.size() + 1;

    for (int i = (int)(outer_points.size() / 2) - 1; i >= 0; --i)
      out << " " << (outer_top_offset + i);
    out << "\n";
  }

  // bottom face;
  out << "f";
  for (int i = GRID_SIZE - 1; i >= 0; --i)
  {
    const int offset = i * GROOVE_VERTICES + 1;
    out << " " << (offset + 17) << " " << (offset + 1) 
        << " " << (offset + 0) << " " << (offset + 16);
  }
  {
    const int outer_bottom_offset = inner_points.size() + outer_points.size() / 2 + 1;

    for (int i = 0; i < (int)(outer_points.size() / 2); ++i)
      out << " " << (outer_bottom_offset + i);
    out << "\n";
  }
}

void ab_slot(bool vertical, const std::string& filename)
{
  vector<vec3> points;

  double curr_x = SLOT_OVERHANG_WIDTH + SLOT_WIDTH * 0.5;
  
  //points.push_back( {curr_x, BRIDGE_DEPTH, 0.0} );
  for (int i = 0; i < GRID_SIZE-1; ++i)
  {
    points.push_back( {curr_x + GRID_WIDTH - SLOT_WIDTH, BRIDGE_DEPTH, 0.0} );

    points.push_back( {curr_x + GRID_WIDTH - SLOT_WIDTH, BRIDGE_DEPTH * 0.5, 0.0} );
    points.push_back( {curr_x + GRID_WIDTH, BRIDGE_DEPTH * 0.5, 0.0} );
    points.push_back( {curr_x + GRID_WIDTH, BRIDGE_DEPTH, 0.0} );

    curr_x += GRID_WIDTH;
  }
  
  curr_x += GRID_WIDTH - SLOT_WIDTH * 0.5;
  
  
  if (vertical)
  {
    points.push_back( {curr_x, BRIDGE_DEPTH, 0.0} );
    points.push_back( {curr_x, SLOT_OVERHANG_DEPTH, 0.0} );
    points.push_back( {curr_x + SLOT_OVERHANG_WIDTH, SLOT_OVERHANG_DEPTH, 0.0} );
    points.push_back( {curr_x + SLOT_OVERHANG_WIDTH, 0.0, 0.0} );
  }
  else
  {
    points.push_back( {curr_x + SLOT_OVERHANG_WIDTH, BRIDGE_DEPTH, 0.0} );
    points.push_back( {curr_x + SLOT_OVERHANG_WIDTH, BRIDGE_DEPTH - SLOT_OVERHANG_DEPTH, 0.0} );
    points.push_back( {curr_x, BRIDGE_DEPTH - SLOT_OVERHANG_DEPTH, 0.0} );
    points.push_back( {curr_x, 0.0, 0.0} );
  }

  for (size_t i = 0; i < GRID_SIZE - 1; ++i)
  {
    curr_x -= GRID_WIDTH;
    points.push_back( {curr_x + SLOT_WIDTH * 0.5, 0.0, 0.0} );
    points.push_back( {curr_x - SLOT_WIDTH * 0.5, 0.0, 0.0} );
  }
  
  if (vertical) 
  {
    points.push_back( {0.0, 0.0, 0.0} );    
    points.push_back( {0.0, SLOT_OVERHANG_DEPTH, 0.0} );
    points.push_back( {SLOT_OVERHANG_WIDTH, SLOT_OVERHANG_DEPTH, 0.0} );
    points.push_back( {SLOT_OVERHANG_WIDTH, BRIDGE_DEPTH, 0.0} );

    for (size_t i = 0; i < points.size(); ++i)
      points[i].y = BRIDGE_DEPTH - points[i].y;
  }
  else
  {
    
    points.push_back( {SLOT_OVERHANG_WIDTH, 0.0, 0.0f} );
    points.push_back( {SLOT_OVERHANG_WIDTH, BRIDGE_DEPTH - SLOT_OVERHANG_DEPTH, 0.0} );
    points.push_back( {0.0, BRIDGE_DEPTH - SLOT_OVERHANG_DEPTH, 0.0} );
    points.push_back( {0.0, BRIDGE_DEPTH, 0.0} );
  }

  plot_extruded_points( points, SLOT_WIDTH, filename );
}

int main(int argc, char ** args)
{
  ab_slot(false, "horizontal_slot.obj");
  ab_slot(true, "vertical_slot.obj");
  light_board("board.obj");
}
