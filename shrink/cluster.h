#pragma once

#include <set>
#include <map>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <array>

using std::multiset;
using std::map;
using std::array;
using std::vector;

/**
 * Given a set of input colors, create a limited-color palette by clustering
 * similar colors together into a canonical color.
 */

struct ColorKey
{
  ColorKey();
  ColorKey(char, char, char);
  array<float, 3> yuv;
  
  bool operator <(const ColorKey& k) const;
  float distance_sq(const ColorKey& ck) const;
};

struct ColorKeyMean
{
  ColorKeyMean();
  array<float, 3> yuv_sum;

  int num_samples;
  
  void update(ColorKey, size_t);
  ColorKey mean() const;
}
  

struct ColorEntry
{
  int count;
  int index;

  float closest;
};

class ColorClusterer
{
public: 
  explicit ColorClusterer(int num_clusters_);

  void add_pixel(char r, char g, char b);
  void update_clusters();
  
  ~ColorClusterer();

private:
  void initial_clusters();

  /**
   * Creates a clustering ordered by RGB tuples when the number of unique colors
   * is <= the desired number of clusters.
   */
  bool is_trivial();
  
  void trivial_clusters();
  
  map<ColorKey, ColorEntry> color_map;

  vector<ColorKey> cluster_means;
  
  size_t num_clusters;
  bool has_initial_clusters;
  bool dirty;
};

