#include "shapes.h"

/***
 * Add the same points already in the vector, at a new depth.
 ***/
void extrude_points( vector<vec3>& points, double depth )
{
  const size_t num_front_side = points.size();

  for (size_t i = 0; i < num_front_side; ++i)
    points.push_back({points[i].x, points[i].y, depth });
}

/***
 * Given a set of faces constructed from points at depth z = 0,
 * append faces at the corresponding indices reflecting the extruded
 * points. Orientation is reversed to match z's. oe is the offset between a
 * point's index and the index of the corresponding extruded point.
 */
void extrude_faces( vector<quad>& quads, int eo)
{
  const size_t num_faces = quads.size();
  for (size_t i = 0; i < num_faces; ++i)
  {
    const quad& q = quads[i];
    quads.push_back( quad{{ q.f[3] + eo, q.f[2] + eo, q.f[1] + eo, q.f[0]+ eo }} );
  }
}
