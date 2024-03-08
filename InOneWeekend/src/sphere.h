#ifndef SPHERE_H
#define SPHERE_H

#include"hittable.h"
#include "vec3.h"
#include"material.h"
class sphere : public hittable {
  public:
    sphere(){}
    sphere(vec3 _center, float _radius,material* _material) : center(_center), radius(_radius),mat_ptr(_material) {}
    vec3 center;
    float radius;
    material* mat_ptr;
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const  {
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(),r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc,oc) - radius*radius;

    float discriminant = b * b - a*c;
    // Find the nearest root that lies in the acceptable range.
    if(discriminant > 0){
        float temp = (-b - sqrt(b*b-a*c))/a;
        if(temp < t_max && temp > t_min){
            rec.t = temp;
            rec.p = r.at(rec.t);
            rec.normal = (rec.p - center)/radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(b * b - a * c))/a;
        if(temp < t_max && temp > t_min){
            rec.t = temp;
            rec.p = r.at(rec.t);
            rec.normal = (rec.p - center)/radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}
#endif