#ifndef BOXH
#define BOXH
#include"hittablelist.h"
#include"rect.h"
class box: public hittable  {
    public:
        box() {}
        box(const vec3& p0, const vec3& p1, material* ptr);
        virtual bool hit(const ray& r, float t0, float t1, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& output_box) const {
            output_box = aabb(pmin, pmax);
            return true;
        }

    public:
        vec3 pmin;
        vec3 pmax;
        hittable *list_ptr;
};

box::box(const vec3& p0, const vec3& p1, material* ptr){
    pmin = p0;
    pmax = p1;
    hittable **list = new hittable*[6];
    list[0] = new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr);
    list[1] = new flip_normal( new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr));

    list[2] = new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr);
    list[3] = new flip_normal(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr));

    list[4] = new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr);
    list[5] = new flip_normal(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr));
    list_ptr = new hittable_list(list,6);
}

bool box::hit(const ray& r, float t0, float t1, hit_record& rec) const {
    return list_ptr->hit(r, t0, t1, rec);
}

class translate : public hittable {
    public:
        translate(hittable *p, const vec3& displacement)
            : ptr(p), offset(displacement) {}

        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& output_box) const;

    public:
        hittable *ptr;
        vec3 offset;
};

bool translate::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    ray moved_r(r.origin() - offset, r.direction(), r.time());
    if (!ptr->hit(moved_r, t_min, t_max, rec))
        return false;

    rec.p += offset;
    return true;
}

bool translate::bounding_box(float t0, float t1, aabb& output_box) const {
    if (!ptr->bounding_box(t0, t1, output_box))
        return false;

    output_box = aabb(output_box.min() + offset, output_box.max() + offset);
    return true;
}

class rotate_y : public hittable {
    public:
        rotate_y(hittable* p, float angle);

        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;
        virtual bool bounding_box(float t0, float t1, aabb& output_box) const {
            output_box = bbox;
            return hasbox;
        }

    public:
        hittable* ptr;
        float sin_theta;
        float cos_theta;
        bool hasbox;
        aabb bbox;
};

rotate_y::rotate_y(hittable *p, float angle):ptr(p) {
    float radians = (M_PI/180.0) *angle;
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->bounding_box(0, 1, bbox);

    vec3 min( FLT_MAX,  FLT_MAX,  FLT_MAX);
    vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                float x = i*bbox.max().x() + (1-i)*bbox.min().x();
                float y = j*bbox.max().y() + (1-j)*bbox.min().y();
                float z = k*bbox.max().z() + (1-k)*bbox.min().z();

                float newx =  cos_theta*x + sin_theta*z;
                float newz = -sin_theta*x + cos_theta*z;

                vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    if(tester[c] > max[c])
                        max[c] = tester[c];
                    if(tester[c] < min[c])
                        min[c] = tester[c];
                }
            }
        }
    }

    bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    vec3 origin = r.origin();
    vec3 direction = r.direction();

    origin[0] = cos_theta*r.origin()[0] - sin_theta*r.origin()[2];
    origin[2] = sin_theta*r.origin()[0] + cos_theta*r.origin()[2];

    direction[0] = cos_theta*r.direction()[0] - sin_theta*r.direction()[2];
    direction[2] = sin_theta*r.direction()[0] + cos_theta*r.direction()[2];

    ray rotated_r(origin, direction, r.time());

    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    vec3 p = rec.p;
    vec3 normal = rec.normal;

    p[0] =  cos_theta*rec.p[0] + sin_theta*rec.p[2];
    p[2] = -sin_theta*rec.p[0] + cos_theta*rec.p[2];

    normal[0] =  cos_theta*rec.normal[0] + sin_theta*rec.normal[2];
    normal[2] = -sin_theta*rec.normal[0] + cos_theta*rec.normal[2];

    rec.p = p;
    rec.normal = normal;

    return true;
}

class isotropic : public material {
    public:
        isotropic(texture* a) : albedo(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const  {
            scattered = ray(rec.p, random_in_unit_sphere());
            attenuation = albedo->value(rec.u, rec.v, rec.p);
            return true;
        }

    public:
        texture* albedo;
};

class constant_medium : public hittable {
    public:
        constant_medium(hittable* b, float d, texture* a)
            : boundary(b), neg_inv_density(-1/d)
        {
            phase_function = new isotropic(a);
        }

        virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;

        virtual bool bounding_box(float t0, float t1, aabb& output_box) const {
            return boundary->bounding_box(t0, t1, output_box);
        }

    public:
        hittable* boundary;
        material* phase_function;
        float neg_inv_density;
};

bool constant_medium::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool enableDebug = false;
    const bool debugging = enableDebug && drand48() < 0.00001;

    hit_record rec1, rec2;

    if (!boundary->hit(r, -FLT_MAX, FLT_MAX, rec1))
        return false;

    if (!boundary->hit(r, rec1.t+0.0001, FLT_MAX, rec2))
        return false;

    if (debugging) std::cerr << "\nt0=" << rec1.t << ", t1=" << rec2.t << '\n';

    if (rec1.t < t_min) rec1.t = t_min;
    if (rec2.t > t_max) rec2.t = t_max;

    if (rec1.t >= rec2.t)
        return false;

    if (rec1.t < 0)
        rec1.t = 0;

    const float ray_length = r.direction().length();
    const float distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    const float hit_distance = neg_inv_density * log(drand48());

    if (hit_distance > distance_inside_boundary)
        return false;

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    if (debugging) {
        std::cerr << "hit_distance = " <<  hit_distance << '\n'
                  << "rec.t = " <<  rec.t << '\n'
                  << "rec.p = " <<  rec.p << '\n';
    }

    rec.normal = vec3(1,0,0);  // arbitrary
    rec.mat_ptr = phase_function;

    return true;
}


#endif