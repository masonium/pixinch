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

typedef unsigned char uchar;

struct ColorKey
{
  ColorKey();
  ColorKey(uchar, uchar, uchar);
  array<float, 3> yuv;
  
  bool operator <(const ColorKey& k) const;
  bool operator ==(const ColorKey& k) const
  {
    return yuv == k.yuv;
  }
  
  float distance_sq(const ColorKey& ck) const;

  void to_rgb(uchar& r, uchar& g, uchar& b) const;
};

struct ColorKeyMean
{
  ColorKeyMean();
  array<float, 3> yuv_sum;

  int num_samples;
  
  void update(ColorKey, size_t);
  ColorKey mean() const;
};  

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

  void add_pixel(uchar r, uchar g, uchar b);
  void update_clusters(int max_iterations = 20,
                       bool force_reset = false);

  int get_index(uchar r, uchar g, uchar b) const;
  ColorKey operator[](int index) const;
  
  int clusters() const;

  void clear();
  
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

