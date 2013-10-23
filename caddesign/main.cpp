#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>
#include "shapes.h"

using std::vector;
using std::ofstream;
using std::ostream;
using std::cout;
using std::endl;

int mod1(int a, int m)
{
  return (a-1) % m + 1;
}
void print_vertex( ostream&out, const vec3& v )
{
  out << "v " << v.x << " " << v.y << " " << v.z << "\n";
}

void print_face( ostream& out, const quad& q )
{
  out << "f " << (q.f[0] + 1) << ' ' << (q.f[1] + 1) << ' ' << (q.f[2] + 1) << '\n';
  out << "f " << (q.f[0] + 1) << ' ' << (q.f[2] + 1) << ' ' << (q.f[3] + 1) << '\n';
}

void write_svg( const std::string& filename, const vector<vec3>& points )
{
  ofstream f(filename.c_str());

  f << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">" << endl;
  f << "<polygon points=\"";

  for (int i = 0; i < (int)points.size() / 2; ++i)
  {
    if (i != 0)
      f << " ";
    f << points[i].x << "," << points[i].y;
  };
  f << "\"/>" << endl;
  f << "</svg>";
}

void write_mesh( const std::string& filename, const vector<vec3>& points,
                 const vector<quad>& quads )
{
  ofstream f(filename.c_str());
  
  for (const vec3& p: points)
    print_vertex(f, p);
  for (const quad& q: quads)
    print_face(f, q);
}


int main(int argc, char ** args)
{
  grid_strip(true, "bottom_strip.svg");
  grid_strip(false, "top_strip.svg");
  light_board("board.obj");
}
