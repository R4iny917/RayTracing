#ifndef HITABLELISTH
#define HITABLELISTH
#include"hittable.h"

class hittable_list: public hittable {
    public:
        hittable_list(){}
        hittable_list(hittable **l,int n){list = l; list_size = n;}
        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const ;
        virtual bool bounding_box(float t0, float t1, aabb& output_box) const;
        virtual float pdf_value(const vec3& o,const vec3& v)const;
        virtual vec3 random(const vec3& o)const; 
        hittable ** list;
        int list_size;
};

float hittable_list::pdf_value(const vec3& o, const vec3& v)const{
    float weight = 1.0/list_size;
    float sum = 0;
    for(int i = 0; i < list_size; i++)
        sum += weight*list[i]->pdf_value(o,v);
    return sum;
}

vec3 hittable_list::random(const vec3& o)const{
    int index = int(drand48()*list_size);
    return list[index]->random(o);
}

bool hittable_list::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    hit_record temp_rec;
    bool hit_anything = false;
    double closest_so_far = t_max;
    for(int i = 0; i <list_size; i++){
        if(list[i]->hit(r,t_min,closest_so_far,temp_rec)){
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

bool hittable_list::bounding_box(float t0, float t1, aabb& output_box) const {
    if (list_size < 1) return false;

    aabb temp_box;
    bool first_true = list[0]->bounding_box(t0,t1,temp_box);
    if(!first_true)
        return false;
    else
        output_box = temp_box;
    for(int i = 1; i < list_size; i++){
        if(list[0]->bounding_box(t0,t1,temp_box)){
            output_box = surrounding_box(output_box,temp_box);
        }
        else
            return false;
    }

    return true;
}

#endif