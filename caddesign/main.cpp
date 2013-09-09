#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

using std::vector;
using std::ofstream;

struct vec3
{
  double x, y, z;
};

int mod1(int a, int m)
{
  return (a-1) % m + 1;
}
  

void plot_extruded_points( const vector<vec3>& points,
                           int width, 
                           const std::string& filename )
{
  const size_t num_front_side = points.size();

  ofstream out(filename);
  
  for (size_t i = 0; i < points.size(); ++i)
    out << "v " << points[i].x << " " << points[i].y
        << " " << points[i].z << "\n";
  
  for (size_t i = 0; i < points.size(); ++i)
    out << "v " << points[i].x << " " << points[i].y
        << " " << width << "\n";

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

void ab_slot(bool vertical, const std::string& filename)
{
  const double grid_space = 31.25;
  const double slot_space = 2;
  const double extra_space = 10;
  const double extra_depth = 10;
  const double full_height = 40;
  
  const int grid_size = 32;

  const double slot_width = slot_space;
  
  vector<vec3> points;


  double curr_x = extra_space + slot_space * 0.5;
  
  //points.push_back( {curr_x, full_height, 0.0} );
  for (int i = 0; i < grid_size-1; ++i)
  {
    points.push_back( {curr_x + grid_space - slot_space, full_height, 0.0} );

    points.push_back( {curr_x + grid_space - slot_space, full_height * 0.5, 0.0} );
    points.push_back( {curr_x + grid_space, full_height * 0.5, 0.0} );
    points.push_back( {curr_x + grid_space, full_height, 0.0} );

    curr_x += grid_space;
  }
  
  curr_x += grid_space - slot_space * 0.5;
  
  
  if (vertical)
  {
    points.push_back( {curr_x, full_height, 0.0} );
    points.push_back( {curr_x, extra_depth, 0.0} );
    points.push_back( {curr_x + extra_space, extra_depth, 0.0} );
    points.push_back( {curr_x + extra_space, 0.0, 0.0} );
  }
  else
  {
    points.push_back( {curr_x + extra_space, full_height, 0.0} );
    points.push_back( {curr_x + extra_space, full_height - extra_depth, 0.0} );
    points.push_back( {curr_x, full_height - extra_depth, 0.0} );
    points.push_back( {curr_x, 0.0, 0.0} );
  }

  for (size_t i = 0; i < grid_size - 1; ++i)
  {
    curr_x -= grid_space;
    points.push_back( {curr_x + slot_space * 0.5, 0.0, 0.0} );
    points.push_back( {curr_x - slot_space * 0.5, 0.0, 0.0} );
  }
  
  if (vertical) 
  {
    points.push_back( {0.0, 0.0, 0.0} );    
    points.push_back( {0.0, extra_depth, 0.0} );
    points.push_back( {extra_space, extra_depth, 0.0} );
    points.push_back( {extra_space, full_height, 0.0} );

    for (size_t i = 0; i < points.size(); ++i)
      points[i].y = full_height - points[i].y;
  }
  else
  {
    
    points.push_back( {extra_space, 0.0, 0.0f} );
    points.push_back( {extra_space, full_height - extra_depth, 0.0} );
    points.push_back( {0.0, full_height - extra_depth, 0.0} );
    points.push_back( {0.0, full_height, 0.0} );
  }

  plot_extruded_points( points, slot_width, filename );
}

int main(int argc, char ** args)
{
  ab_slot(false, "horizontal_slot.obj");
  ab_slot(true, "vertical_slot.obj");
}
