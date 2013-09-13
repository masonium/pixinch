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
  grid_strip(true, "bottom_strip.obj");
  grid_strip(false, "top_strip.obj");
  //light_board("board.obj");
}
