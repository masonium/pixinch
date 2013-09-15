 #include <iostream>
#include <cassert>
#include "shapes.h"
using std::ostream;
using std::cout;
using std::endl;

const int NUM_OUTER_VERTICES = 8; // outer vertices per z-plane

/***
 * Construct the vertices for the z=0 face of the light board.
 */
void light_board_vertices(vector<vec3>& points)
{
  const double ys[2] = {0, BOARD_FRAME_WIDTH};

  const double GHW = GROOVE_WIDTH * 0.5;
  const double BHHH = BOARD_HOLE_HEIGHT * 0.5;

  /** inner groove vertices **/
  for (int i = 0; i < GRID_SIZE; ++i)
  {
    double mid_x = (i + 0.5) * GRID_WIDTH;
    for (double y: ys)
    {
      points.push_back( vec3{mid_x - GHW, y - BHHH, 0.0} );
      points.push_back( vec3{mid_x - GHW, y + BHHH, 0.0} );
      points.push_back( vec3{mid_x + GHW, y + BHHH, 0.0} );
      points.push_back( vec3{mid_x + GHW, y - BHHH, 0.0} );
    }
  }

  /** outer board vertices **/
  const double min_x = -BOARD_TAB_WIDTH;
  points.push_back( vec3{min_x, -BOARD_UNDERHANG, 0.0} );
  points.push_back( vec3{min_x, -BHHH, 0.0} );
  points.push_back( vec3{min_x, BOARD_FRAME_WIDTH + BHHH, 0.0} );
  points.push_back( vec3{min_x, BOARD_FRAME_WIDTH + BOARD_TAB_WIDTH, 0.0} );

  const double max_x = BOARD_FRAME_WIDTH + BOARD_TAB_WIDTH;
  points.push_back( vec3{max_x, BOARD_FRAME_WIDTH + BOARD_TAB_WIDTH, 0.0} );
  points.push_back( vec3{max_x, BOARD_FRAME_WIDTH + BHHH, 0.0} );
  points.push_back( vec3{max_x, -BHHH, 0.0} );
  points.push_back( vec3{max_x, -BOARD_UNDERHANG, 0.0} );
}

/***
 * Add the inner faces for light board grooves.
 */
void light_board_inner_hole_faces(vector<quad>& quads)
{
  const int eo = GRID_SIZE * 8 + NUM_OUTER_VERTICES;

  for (int i = 0; i < 2 * GRID_SIZE; ++i)
  {
    const int f = 4 * i;
    for (int j = 0; j < 4; ++j)
    {
      int k = (j + 1) % 4;
      quads.push_back( quad{{ f + j + eo, f + j, f + k, f + k + eo}} );
    }
  }
}

/*** 
 * Add the faces connecting and between the holes
 */
void light_board_extrusion_faces(vector<quad>& quads)
{
  // connecting holes
  for (int i = 0; i < GRID_SIZE; ++i)
  {
    const int offset = 8 * i;
    quads.push_back( quad{{ offset + 7, offset + 2, offset + 1, offset + 4 }} );
  }

  // between holes
  for (int i = 0; i < GRID_SIZE - 1; ++i)
  {
    const int offset = 8 * i;
    quads.push_back( quad{{ offset + 3, offset + 6, offset + 13, offset + 8}} );
  }

  // four surrounding faces
  const int ovo = 8 * GRID_SIZE; // outer vertex offset
  quads.push_back( quad{{ ovo + 1, ovo + 2, 5, 0 }} );
  quads.push_back( quad{{ ovo + 2, ovo + 3, ovo + 4, ovo + 5 }} );
  quads.push_back( quad{{ ovo - 5, ovo - 2, ovo + 5, ovo + 6 }} );
  quads.push_back( quad{{ ovo, ovo + 1, ovo + 6, ovo + 7 }} );
}

void light_board_outer_edge_faces(vector<quad>& quads)
{
  const int ovo = 8 * GRID_SIZE;
  const int eo = GRID_SIZE * 8 + 8;
  for (int i = 0; i < 8; ++i)
    quads.push_back( quad{{ ovo + (i + 1) % 8, ovo + i, ovo + eo + i, ovo + eo + (i + 1) % 8 }} );
}

/***
 * Construct the points for the light board.
 * The light board is arranged so that the grooves are parallel to the axis is
 * and the depth of the board is along the z-axis.
 */
void light_board(const std::string& filename)
{
  vector<vec3> vertices;
  light_board_vertices(vertices);

  const int eo = vertices.size();
  assert(eo == GRID_SIZE * 8 + 8);
  extrude_points(vertices, BOARD_DEPTH);
  
  vector<quad> quads;
  light_board_extrusion_faces(quads);

  extrude_faces(quads, eo);

  light_board_inner_hole_faces(quads);
  light_board_outer_edge_faces(quads);
  
  write_mesh(filename, vertices, quads);
}
