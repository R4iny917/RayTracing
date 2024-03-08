#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
#include"aabb.h"
class material;

struct hit_record {
    vec3 p;
    vec3 normal;
    float t;
    float u;
    float v;
    material *mat_ptr;
};

class hittable {
  public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(float t0, float t1, aabb& output_box) const = 0;
    virtual float pdf_value(const vec3& o,const vec3& v)const{return 0.0;}
    virtual vec3 random(const vec3& o)const{return vec3(1,0,0);}  
};

class flip_normal : public hittable {
  public:
      flip_normal(hittable *p) : ptr(p) {}

      virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
          if (!ptr->hit(r, t_min, t_max, rec))
              return false;

          rec.normal = -rec.normal;
          return true;
      }

      virtual bool bounding_box(float t0, float t1, aabb& output_box) const {
          return ptr->bounding_box(t0, t1, output_box);
      }

  public:
      hittable * ptr;
  };



#endif