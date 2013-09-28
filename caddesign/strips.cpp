#include "shapes.h"
#include <iostream>
#include <cassert>
using std::cout;


void grid_strip_slot_points(vector<vec3>& points)
{
  double curr_x = GRID_WIDTH;

  const double SHW = STRIP_DEPTH * 0.5;
    
  for (int i = 0; i < GRID_SIZE-1; ++i)
  {
    points.push_back( {curr_x - SHW, BRIDGE_DEPTH, 0.0} );
    points.push_back( {curr_x - SHW, BRIDGE_DEPTH * 0.5, 0.0} );
    points.push_back( {curr_x + SHW, BRIDGE_DEPTH * 0.5, 0.0} );
    points.push_back( {curr_x + SHW, BRIDGE_DEPTH, 0.0} );

    curr_x += GRID_WIDTH;
  }
};

void grid_strip_slot_points_bottom(vector<vec3>& points, bool bottom)
{
  double curr_x = 0.0;

  const double SHW = STRIP_DEPTH * 0.5;
    
  for (int i = 0; i < GRID_SIZE-1; ++i)
  {
    points.push_back( {curr_x + GRID_WIDTH - SHW, BRIDGE_DEPTH, 0.0} );
    points.push_back( {curr_x + GRID_WIDTH - SHW, BRIDGE_DEPTH * 0.5, 0.0} );
    points.push_back( {curr_x + GRID_WIDTH + SHW, BRIDGE_DEPTH * 0.5, 0.0} );
    points.push_back( {curr_x + GRID_WIDTH + SHW, BRIDGE_DEPTH, 0.0} );
    
    curr_x += GRID_WIDTH * 0.5;
  }
};

void grid_strip_overhang_points_bottom(vector<vec3>& points)
{
  points.push_back( {BOARD_FRAME_WIDTH, BRIDGE_DEPTH, 0.0} );
  points.push_back( {BOARD_FRAME_WIDTH + SLOT_OVERHANG_WIDTH, BRIDGE_DEPTH, 0.0} );
  points.push_back( {BOARD_FRAME_WIDTH + SLOT_OVERHANG_WIDTH, BRIDGE_DEPTH - SLOT_OVERHANG_DEPTH, 0.0} );
  points.push_back( {BOARD_FRAME_WIDTH, BRIDGE_DEPTH - SLOT_OVERHANG_DEPTH, 0.0} );
  points.push_back( {BOARD_FRAME_WIDTH, BRIDGE_DEPTH * 0.5, 0.0 } );
  points.push_back( {BOARD_FRAME_WIDTH, GROOVE_DEPTH, 0.0 } );
  points.push_back( {BOARD_FRAME_WIDTH, 0.0, 0.0} );

  const double GHW = GROOVE_WIDTH * 0.5;
  for (int i = GRID_SIZE - 1; i >= 0; --i)
  {
    double mid_groove = (i + 0.5) * GRID_WIDTH;
    points.push_back( {mid_groove + GHW, 0.0, 0.0} );
    points.push_back( {mid_groove + GHW, GROOVE_DEPTH, 0.0} );
    points.push_back( {mid_groove - GHW, GROOVE_DEPTH, 0.0} );
    points.push_back( {mid_groove - GHW, 0.0, 0.0} );
  }
  
  points.push_back( {0.0, 0.0, 0.0} );
  points.push_back( {0.0, GROOVE_DEPTH, 0.0 } );
  points.push_back( {0.0, BRIDGE_DEPTH * 0.5, 0.0 } );
  points.push_back( {0.0, BRIDGE_DEPTH - SLOT_OVERHANG_DEPTH, 0.0} );
  points.push_back( {-SLOT_OVERHANG_WIDTH, BRIDGE_DEPTH - SLOT_OVERHANG_DEPTH, 0.0} );
  points.push_back( {-SLOT_OVERHANG_WIDTH, BRIDGE_DEPTH, 0.0} );
  points.push_back( {0.0, BRIDGE_DEPTH, 0.0} );
}

void grid_strip_overhang_points_top(vector<vec3>& points)
{
  points.push_back( {BOARD_FRAME_WIDTH, BRIDGE_DEPTH, 0.0} );
  points.push_back( {BOARD_FRAME_WIDTH, BRIDGE_DEPTH * 0.5, 0.0 } );
  points.push_back( {BOARD_FRAME_WIDTH, SLOT_OVERHANG_DEPTH, 0.0} );
  points.push_back( {BOARD_FRAME_WIDTH + SLOT_OVERHANG_WIDTH, SLOT_OVERHANG_DEPTH, 0.0} );
  points.push_back( {BOARD_FRAME_WIDTH + SLOT_OVERHANG_WIDTH, 0.0, 0.0} );
  points.push_back( {BOARD_FRAME_WIDTH, 0.0, 0.0} );
  
  points.push_back( {0.0, 0.0, 0.0} );
  points.push_back( {-SLOT_OVERHANG_WIDTH, 0.0, 0.0} );
  points.push_back( {-SLOT_OVERHANG_WIDTH, SLOT_OVERHANG_DEPTH, 0.0} );
  points.push_back( {0.0, SLOT_OVERHANG_DEPTH, 0.0} );
  points.push_back( {0.0, BRIDGE_DEPTH * 0.5, 0.0 } );
  points.push_back( {0.0, BRIDGE_DEPTH, 0.0} );
}

void grid_strip_overhang_faces_bottom(vector<quad>& quads, int eo)
{
  const int lspi = (GRID_SIZE - 1) * 4  - 1; // last slot point index
  quads.push_back( quad{{lspi-1, lspi, lspi + 1, lspi + 5}} );
  quads.push_back( quad{{lspi + 1, lspi + 2, lspi + 3, lspi + 4}} );

  // faces between grooves
  const int fgpi = lspi + 8;
  for (int f = 0; f <= GRID_SIZE; ++f)
  {
    const int s = fgpi + f*4;
    quads.push_back( quad{{s-2, s-1, s, s+1}} );
  }
  
  const int lgpi = fgpi + GRID_SIZE * 4 - 1;
  assert(lgpi + 8 == eo);

  quads.push_back( quad{{lspi + 5, lspi + 6, lgpi + 2, lgpi + 3}} );
  
  quads.push_back( quad{{lgpi + 4, lgpi + 5, lgpi + 6, lgpi + 7}} );
  quads.push_back( quad{{0, 1, lgpi + 3, lgpi + 7}} );
}

void grid_strip_overhang_faces_top(vector<quad>& quads)
{
  const int lspi = (GRID_SIZE - 1) * 4  - 1; // last slot point index
  quads.push_back( quad{{lspi - 1, lspi, lspi + 1, lspi + 2}} );
  quads.push_back( quad{{lspi + 3, lspi + 4, lspi + 5, lspi + 6}} );

  quads.push_back( quad{{lspi + 2, lspi + 6, lspi + 7, lspi + 11}} );

  quads.push_back( quad{{lspi + 8, lspi + 9, lspi + 10, lspi + 7}} );
  quads.push_back( quad{{0, 1, lspi + 11, lspi + 12}} );
}

void grid_strip(bool bottom, const std::string& filename)
{
  vector<vec3> points;
  vector<quad> quads;
  
  // assemble all of the points first
  grid_strip_slot_points( points );
  if (bottom)
    grid_strip_overhang_points_bottom( points );
  else
    grid_strip_overhang_points_top( points );

  // extrude the existing points
  extrude_points( points, STRIP_DEPTH );
  
  const int eo = points.size() >> 1; // extrusion_offset
  
  // (top & bottom) common z=0 faces
  for (int i = 1; i < GRID_SIZE-1; ++i)
  {
    const int offset = 4 * i;
    quads.push_back( quad{{offset, offset+1, offset-2, offset-1}} );
  }

  // top/bottom unique faces
  if (bottom)
    grid_strip_overhang_faces_bottom(quads, eo);
  else
    grid_strip_overhang_faces_top(quads);  

  extrude_faces( quads, eo );
  
  // construct the extruded point faces
  for (int i = 0; i < eo; ++i)
    quads.push_back( quad{ {(i+1) % eo, i, i + eo, (i+1) % eo + eo }} );

  write_svg(filename, points);
  //write_mesh(filename, points, quads);
}
