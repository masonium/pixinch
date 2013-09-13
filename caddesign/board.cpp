#include <iostream>
#include "shapes.h"
using std::ostream;


/***
 * Construct the points for the nth groove in the light board.
 * Points are from left to right, low to high, and low depth to high depth.
 */
void light_board_vertices(vector<vec3>& points, int i)
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
  points.push_back( vec3{-BOARD_TAB_WIDTH, -BOARD_UNDERHANG, 0.0} );
  points.push_back( vec3{-BOARD_TAB_WIDTH, -BHHH, 0.0} );
  points.push_back( vec3{-BOARD_TAB_WIDTH, BOARD_FRAME_WIDTH + BHHH, 0.0} );
  points.push_back( vec3{-BOARD_TAB_WIDTH, BOARD_FRAME_WIDTH + BOARD_TAB_WIDTH, 0.0} );

  points.push_back( vec3{BOARD_FRAME_WIDTH + BOARD_TAB_WIDTH, BOARD_FRAME_WIDTH + BOARD_TAB_WIDTH, 0.0} );
  points.push_back( vec3{BOARD_FRAME_WIDTH + BOARD_TAB_WIDTH, BOARD_FRAME_WIDTH + BHHH, 0.0} );
  points.push_back( vec3{BOARD_FRAME_WIDTH + BOARD_TAB_WIDTH, -BHHH, 0.0} );
  points.push_back( vec3{BOARD_FRAME_WIDTH + BOARD_TAB_WIDTH, -BOARD_UNDERHANG, 0.0} );

}

/***
 * Print out the inner faces for light board grooves
 */
void light_board_inner_hole_faces(vector<quad>& quads)
{
  const int eo = GRID_SIZE * 8 + 8;

  for (int i = 0; i < 2 * GRID_SIZE; ++i)
  {
    const int f = 4 * i;
    for (int j = 0; j < 4; ++j)
    {
      int k = (j + 1) % 4;
      quads.push_back( quad{{ f + j, f + j + eo, f + k + eo, f + k}} );
    }
  }
}

void light_board_extrusion_faces(vector<quad>& quads)
{
  
}

void light_board_top_faces(ostream& out)
{
  
}

/***
 * Construct the points for the light board.
 * The light board is arranged so that the grooves are parallel to the axis is
 * and the depth of the board is along the z-axis.
 */
void light_board(const std::string& filename)
{
}
