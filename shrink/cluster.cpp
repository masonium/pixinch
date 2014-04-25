#include "cluster.h"
#include <algorithm>
#include <iterator>
#include <vector>
#include <limits>
#include <iostream>

using namespace std;

ColorKey::ColorKey()
{
  yuv[0] = yuv[1] = yuv[2] = 0.0f;
}

ColorKey::ColorKey(uchar r_, uchar g_, uchar b_)
{
  yuv[0] = 0.299f    * r_  + 0.587f      * g_  + 0.114f    * b_;
  yuv[1] = -0.14713f * r_  + -0.28886f   * g_  + 0.436     * b_;
  yuv[2] = 0.615f    * r_  + -0.51499f   * g_  + -0.10001f * b_;
}

bool ColorKey::operator <(const ColorKey& ck) const
{
  return yuv < ck.yuv;
}

float ColorKey::distance_sq(const ColorKey& ck) const
{
  float dy = (yuv[0] - ck.yuv[0]);
  float du = (yuv[1] - ck.yuv[1]);
  float dv = (yuv[2] - ck.yuv[2]);
  return dy * dy + du * du + dv * dv;
}

void ColorKey::to_rgb(uchar& r, uchar& g, uchar& b) const
{
  r = uchar(1.0f * yuv[0] +  0.00000f * yuv[1]  +  1.13983f * yuv[2]);
  g = uchar(1.0f * yuv[0] + -0.39465f * yuv[1]  + -0.58060f * yuv[2]);
  b = uchar(1.0f * yuv[0] +  2.03211f * yuv[1]  +  0.00000f * yuv[2]);
}

////////////////////////////////////////////////////////////////////////////////

ColorKeyMean::ColorKeyMean() : num_samples(0)
{
  yuv_sum[0] = yuv_sum[1] = yuv_sum[2] = 0.0f;
  
}

void ColorKeyMean::update(ColorKey ck, size_t n)
{
  for (size_t i = 0; i < 3; ++i)
    yuv_sum[i] += ck.yuv[i] * n;
  num_samples += n;
}

ColorKey ColorKeyMean::mean() const
{
  ColorKey ck;
  ck.yuv[0] = yuv_sum[0] / num_samples;
  ck.yuv[1] = yuv_sum[1] / num_samples;
  ck.yuv[2] = yuv_sum[2] / num_samples;
  
  return ck;
}

////////////////////////////////////////////////////////////////////////////////
ColorClusterer::ColorClusterer(int num_clusters_) :
  num_clusters(num_clusters_), has_initial_clusters(false), dirty(false)
{
  
}

void ColorClusterer::clear()
{
  has_initial_clusters = false;
  dirty = false;
  
  color_map.clear();
  cluster_means.clear();
}

void ColorClusterer::add_pixel(uchar r, uchar g, uchar b)
{
  ColorKey rgb{r, g, b};
  auto x = color_map.find( rgb );
  if (x == color_map.end())
    color_map.insert( make_pair(rgb, ColorEntry{1, 0}) );
  else
    ++x->second.count;
  dirty = true;
}

void ColorClusterer::update_clusters(int max_iterations,
                                     bool force_reset)
{
  if (!dirty)
    return;

  if (is_trivial())
  {
    trivial_clusters();
    return;
  }
  
  if (!has_initial_clusters || force_reset)
    initial_clusters();

  vector<ColorKey> old_clusters;

  cerr << "Starting clustering with " << color_map.size() << " unique clusters." << endl;
  
  int num_iterations = 0;
  float min_distance;
  do 
  {
    old_clusters = cluster_means;
    
    min_distance = numeric_limits<float>::max();
    /* Assign the cluster indices based on the means. */
    for (auto& entry: color_map)
    {
      entry.second.closest = numeric_limits<float>::max();
      for (size_t i = 0; i < old_clusters.size(); ++i)
      {
        ColorKey& ck = old_clusters[i];
        float ds = entry.first.distance_sq(ck);
        min_distance = min(ds, min_distance);
        if ( entry.second.closest > ds )
        {
          entry.second.closest = ds;
          entry.second.index = i;
        }
      }
    }

    /* Assign the means based on the clusters */
    vector<ColorKeyMean> means( num_clusters );
    for (auto& entry: color_map)
    {
      means[entry.second.index].update( entry.first, entry.second.count );
    }

    for (size_t i = 0; i < old_clusters.size(); ++i)
      cluster_means[i] = means[i].mean();

    num_iterations += 1;
  } while (old_clusters != cluster_means &&
           num_iterations < max_iterations);

  cerr << "Clustering found after " << num_iterations << " iterations.\n";
  cerr << "Minimum distance: " << min_distance << endl;
}

bool ColorClusterer::is_trivial()
{
  return color_map.size() < num_clusters;
}
                                 

void ColorClusterer::trivial_clusters()
{
  int index = 0;
  cluster_means.clear();
  for (auto& entry: color_map) 
  {
    cluster_means.push_back(entry.first);
    entry.second.index = index++;
  }

  cerr << "Used trivial cluster with " << index << " colors.\n";
}

ColorKey first_elem(const map<ColorKey, ColorEntry>::value_type & p)
{
  return p.first;
}

int ColorClusterer::get_index(uchar r, uchar g, uchar b) const
{
  auto it = color_map.find( ColorKey{r, g, b} );
  if (it != color_map.end())
    return it->second.index;
  return -1;
}

int ColorClusterer::clusters() const
{
  return cluster_means.size();
}

ColorKey ColorClusterer::operator[](int index) const
{
  return cluster_means[index];
}

void ColorClusterer::initial_clusters()
{
  vector<ColorKey> keys;
  transform(color_map.begin(), color_map.end(), back_inserter(keys), first_elem );

  random_shuffle(keys.begin(), keys.end());
  
  for (size_t i = 0; i < num_clusters; ++i)
    cluster_means.push_back(keys[i]);
}
