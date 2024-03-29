#ifndef MATERIALH
#define MATERIALH
#include"hittable.h"
#include"texture.h"
#include"onb.h"
#include"pdf.h"
struct scatter_record{
    ray specular_ray;
    bool is_specular;
    vec3 attenuation;
    pdf *pdf_ptr;
};


class material{
  public:
    virtual bool scatter(const ray& r_in,const hit_record& hrec, scatter_record& srec)const{
        return false;
    };
	virtual float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
		return 0;}
   virtual vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p)const {
        return vec3(0,0,0);}
    
};

class lambertian : public material{
  public:
    lambertian(texture *a):albedo(a){}
    virtual float scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered)const{
       float cosine = dot(rec.normal,unit_vector(scattered.direction()));
        if(cosine < 0)
            return 0;
        return cosine/M_PI;
    }
    virtual bool scatter(const ray& r_in,const hit_record& hrec, scatter_record& srec)const{
        srec.is_specular = false;
        srec.attenuation = albedo->value(hrec.u,hrec.v,hrec.p);
        srec.pdf_ptr = new cosine_pdf(hrec.normal);
        return true;
    };
	texture* albedo;
};

class metal : public material{
  public:
    metal(const vec3& a, float f):albedo(a){if(f < 1) fuzz = f; else fuzz = 1;}
    virtual bool scatter(const ray& r_in,const hit_record& hrec, scatter_record& srec)const{
        vec3 reflected = reflect(unit_vector(r_in.direction()),hrec.normal);
        srec.specular_ray = ray(hrec.p,reflected+fuzz*random_in_unit_sphere());
        srec.attenuation = albedo;
        srec.is_specular = true;
        srec.pdf_ptr = 0;
        return true;
    };
	vec3 albedo;
    float fuzz;
};

class dielectric : public material{
    public:
        float ref_idx;
        dielectric(float ri): ref_idx(ri){}
        virtual bool scatter(const ray& r_in,const hit_record& hrec, scatter_record& srec)const{
            srec.is_specular = true;
            srec.pdf_ptr = NULL;
            srec.attenuation = vec3(1.0,1.0,1.0);
            
            vec3 outward_normal;
            vec3 reflected = reflect(r_in.direction(),hrec.normal);
            float ni_over_nt;
            srec.attenuation = vec3(1.0,1.0,1.0);
            vec3 refracted;
            float reflect_prob;
            float cosine;
            if(dot(r_in.direction(),hrec.normal) > 0){
                outward_normal = -hrec.normal;
                ni_over_nt = ref_idx;
                cosine = ref_idx * dot(r_in.direction(),hrec.normal)/r_in.direction().length();
            }
            else{
                outward_normal = hrec.normal;
                ni_over_nt = 1.0/ref_idx;
                cosine = -dot(r_in.direction(),hrec.normal)/r_in.direction().length();
            }
            if(refract(r_in.direction(),outward_normal,ni_over_nt,refracted)){
                reflect_prob = schlick(cosine,ref_idx);
            }
            else{
                srec.specular_ray = ray(hrec.p,reflected);
                reflect_prob = 1.0;
            }
            if(drand48() < reflect_prob){
                srec.specular_ray = ray(hrec.p,reflected,r_in.time());
            }
            else{
                srec.specular_ray = ray(hrec.p,refracted,r_in.time());
            }
            return true;
        }
};

class diffuse_light : public material  {
    public:
        diffuse_light(texture * a) : emit(a) {}

        virtual bool scatter(
            const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered
        ) const {
            return false;
        }

        virtual vec3 emitted(const ray& r_in, const hit_record& rec, float u, float v, const vec3& p)const{
            if(dot(rec.normal,r_in.direction())<0.0)
                return emit->value(u,v,p);
            else
                return vec3(0,0,0);
        }

    public:
        texture * emit;
};


#endif