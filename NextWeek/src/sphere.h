#ifndef SPHERE_H
#define SPHERE_H

#include"hittable.h"
#include "vec3.h"
#include"material.h"
#include<cmath>
class sphere : public hittable {
  public:
    sphere(){}
    sphere(vec3 _center, float _radius,material* _material) : center(_center), radius(_radius),mat_ptr(_material) {}
    vec3 center;
    float radius;
    material* mat_ptr;
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb& output_box) const;
    
};

void get_sphere_uv(const vec3& p, float& u, float& v) {
    float phi = atan2(p.z(), p.x());
    float theta = asin(p.y());
    u = 1-(phi + M_PI) / (2*M_PI);
    v = (theta + M_PI/2) / M_PI;
}

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
            get_sphere_uv(rec.normal, rec.u, rec.v);
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(b * b - a * c))/a;
        if(temp < t_max && temp > t_min){
            rec.t = temp;
            rec.p = r.at(rec.t);
            rec.normal = (rec.p - center)/radius;
            get_sphere_uv(rec.normal, rec.u, rec.v);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

bool sphere:: bounding_box(float t0, float t1, aabb& output_box) const{
    output_box = aabb(center - vec3(radius,radius,radius),center + vec3(radius,radius,radius));
    return true;
}

class moving_sphere : public hittable{
    public:
        moving_sphere(){}
        moving_sphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, material *m):
        center0(cen0),center1(cen1),time0(t0),time1(t1),radius(r),mat_ptr(m){};
        virtual bool bounding_box(float t0, float t1, aabb& output_box) const;
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        vec3 center(float time)const;
    public:
        vec3 center0,center1;
        float time0,time1;
        float radius;
        material *mat_ptr;
}; 

vec3 moving_sphere::center(float time)const{
    return center0 + ((time - time0)/ (time1 - time0))*(center1 - center0);
}

bool moving_sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const  {
    vec3 oc = r.origin() - center(r.time());
    float a = dot(r.direction(),r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc,oc) - radius*radius;

    float discriminant = b * b - a*c;
    // Find the nearest root that lies in the acceptable range.
    if(discriminant > 0){
        float temp = (-b - sqrt(discriminant))/a;
        if(temp < t_max && temp > t_min){
            rec.t = temp;
            rec.p = r.at(rec.t);
            rec.normal = (rec.p - center(r.time()))/radius;
            
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-b + sqrt(discriminant))/a;
        if(temp < t_max && temp > t_min){
            rec.t = temp;
            rec.p = r.at(rec.t);
            rec.normal = (rec.p - center(r.time()))/radius;
            
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

bool moving_sphere::bounding_box(float t0, float t1, aabb& output_box) const {
    aabb box0(
        center(t0) - vec3(radius, radius, radius),
        center(t0) + vec3(radius, radius, radius));
    aabb box1(
        center(t1) - vec3(radius, radius, radius),
        center(t1) + vec3(radius, radius, radius));
    output_box = surrounding_box(box0, box1);
    return true;
}

#endif