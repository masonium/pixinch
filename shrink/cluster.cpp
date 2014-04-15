#include "cluster.h"
#include <algorithm>
#include <iterator>
#include <vector>
#include <limits>

using namespace std;

ColorKey::ColorKey()
{
  yuv[0] = yuv[1] = yuv[2] = 0.0f;
}

ColorKey::ColorKey(char r_, char g_, char b_)
{
  yuv[0] = 0.299f * r_ + 0.587f * g_ + 0.114f * b_;
  yuv[1] = -0.147f * r_ - 0.289f * g_ + 0.436 * b_;
  yuv[2] = 0.615f * r_ - 0.515f * g_ - 0.100f * b_;
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

////////////////////////////////////////////////////////////////////////////////

ColorKeyMean::ColorKeyMean() : num_samples(0)
{
  yuv_sum[0] = yuv_sum[1] = yuv_sum[2] = 0.0f;
  
}

void ColorKeyMean::update(ColorKey ck, size_t n)
{
  for (size_t i = 0; i < 3; ++i)
    yuv_sum[i] += ck.yuv[i] * n;
  num_samples += n
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


ColorClusterer::ColorClusterer(int num_clusters_) : num_clusters(num_clusters_), has_initial_clusters(false), dirty(false)
{

}

void ColorClusterer::add_pixel(char r, char g, char b)
{
  ColorKey rgb{r, g, b};
  auto x = color_map.find( rgb );
  if (x == color_map.end())
    color_map.insert( make_pair(rgb, ColorEntry{1, 0}) );
  else
    ++x->second.count;
  dirty = true;
}

void ColorClusterer::update_clusters()
{
  if (!dirty)
    return;

  if (is_trivial())
  {
    trivial_clusters();
    return;
  }
  
  if (!has_initial_clusters)
    initial_clusters();

  vector<ColorKey> old_clusters = cluster_means;
  do 
  {
    /* Assign the cluster indices based on the means. */
    for (auto& entry: color_map)
    {
      entry.second.closest = numeric_limits<float>::max();
      for (size_t i = 0; i < old_clusters.size(); ++i)
      {
        ColorKey& ck = old_clusters[i];
        float ds = entry.first.distance_sq(ck);
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
    
  } while (old_clusters != cluster_means);
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
}

ColorKey first_elem(const map<ColorKey, ColorEntry>::value_type & p)
{
  return p.first;
}
  
void ColorClusterer::initial_clusters()
{
  vector<ColorKey> keys;
  transform(color_map.begin(), color_map.end(), back_inserter(keys), first_elem );

  random_shuffle(keys.begin(), keys.end());
  
  for (size_t i = 0; i < num_clusters; ++i)
  {
    cluster_means.push_back(keys[i]);
  }
}
