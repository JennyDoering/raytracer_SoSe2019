// -----------------------------------------------------------------------------
// Copyright  : (C) 2014-2017 Andreas-C. Bernstein
// License    : MIT (see the file LICENSE)
// Maintainer : Andreas-C. Bernstein <andreas.bernstein@uni-weimar.de>
// Stability  : experimental
//
// Renderer
// -----------------------------------------------------------------------------

#include "renderer.hpp"
#include "ray.hpp"
#include "shape.hpp"
#include "scene.hpp"
#include "hitpont.hpp"
#include <cmath>

Renderer::Renderer(unsigned w, unsigned h, std::string const& file, Scene scene)
  : width_(w)
  , height_(h)
  , color_buffer_(w*h, Color(0.0, 0.0, 0.0))
  , filename_(file)
  , ppm_(width_, height_)
  , scene_(scene)
{}

// void Renderer::render(Camera const& camera1, Scene const& s1)
void Renderer::render()
{
  std::size_t const checker_pattern_size = 20;

  // Camera cam = camera1;
  Camera cam;

  for (unsigned y = 0; y < height_; ++y) {
    for (unsigned x = 0; x < width_; ++x) {
      Pixel p(x,y);

      /* 
      if ( ((x/checker_pattern_size)%2) != ((y/checker_pattern_size)%2)) {
        p.color = Color(0.0, 1.0, float(x)/height_);
      } else {
        p.color = Color(1.0, 0.0, float(y)/width_);
      }
  */

      // if(y > height_/ 3) { 
      //   p.color = Color(1.0, 0.0, 0.0);
      // }
      // if(y < (height_/ 3)){
      //   p.color = Color(1.0f, 1.0f, 0.0f);
      // }
      // if(y > height_ * 2/3) {
      //   p.color = Color{0.0f, 0.0f, 0.0f};
      // }

      // p.color = Color(0.0f, 1.0f, 0.0f);

      Ray strahlcurrent = cam.calcEyeRay(x, y);
      p.color = trace(strahlcurrent, scene_);

      write(p);
    }
  }
  ppm_.save(filename_);
}

void Renderer::write(Pixel const& p)
{
  // flip pixels, because of opengl glDrawPixels
  size_t buf_pos = (width_*p.y + p.x);
  if (buf_pos >= color_buffer_.size() || (int)buf_pos < 0) {
    std::cerr << "Fatal Error Renderer::write(Pixel p) : "
      << "pixel out of ppm_ : "
      << (int)p.x << "," << (int)p.y
      << std::endl;
  } else {
    color_buffer_[buf_pos] = p.color;
  }

  ppm_.write(p);
}

Color Renderer::trace(Ray const& strahl, Scene const& scene) {
  hitpoint closest_hit = hitpoint();
  float closest_dist = std::numeric_limits<float>::max();
  for(auto element : scene.shapes) {
    hitpoint hit = element->intersect(strahl);
    if(hit.cut == true){
      if(hit.distance < closest_dist){
        closest_dist = hit.distance;
        closest_hit = hit;
        closest_hit.col->kd = shade(closest_hit, scene);
      }
      
      // std::cout << closest_hit.col->kd.r << ", " << closest_hit.col->kd.g << ", " << closest_hit.col->kd.b << std::endl;

      // if(element->intersect(strahl).distance < smallest.distance){
      //   return shade(element->intersect(strahl), scene);
      // }
      //return {1.0f, 1.0f, 1.0f};
    }
    // else{
    //   closest_hit.col->kd = Color{1.0f, 0.5f, 0.5f};
    // }
  }
  
  return closest_hit.col->kd;
}

Color Renderer::shade(hitpoint const& h, Scene const& scene) {

  // std::cout << h.col->kd.r << ", " << h.col->kd.g << ", " << h.col->kd.b << std::endl;

  // std::cout << h.distance << std::endl;

  float shade_r = scene.ambient->standard_.r * h.col->kd.r /* + (h.distance + h.col->kd.r) */;
  float shade_g = scene.ambient->standard_.g * h.col->kd.g /* + (h.distance + h.col->kd.g) */;
  float shade_b = (scene.ambient->standard_.b * h.col->kd.b /* + (h.distance + h.col->kd.b) */ + h.distance) / 100;

  // float shade_r = h.col->kd.r / (h.distance + h.col->kd.r);
  // float shade_g = h.col->kd.g / (h.distance + h.col->kd.g);
  // float shade_b = h.col->kd.b / (h.distance + h.col->kd.b);


  // std::cout << shade_r << ", " << shade_g << ", " << shade_b << std::endl; 

  // float shade_normalize = sqrt(shade_r * shade_r + shade_g * shade_g + shade_b * shade_b);

  // Color shade_col = {shade_r / shade_normalize, shade_g / shade_normalize, shade_b / shade_normalize};

  Color shade_col = {shade_r, shade_g, shade_b};

  // std::cout << shade_col.r << ", " << shade_col.g << ", " << shade_col.b << std::endl; 
  // std::cout << "fertig" << std::endl;

  // Color shade_col = {shade_r, shade_g, shade_b};
  // shade_col = glm::normalize(shade_col);
  return shade_col;
}
