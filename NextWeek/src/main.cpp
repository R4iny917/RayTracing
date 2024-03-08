#include <iostream>
#include"sphere.h"
#include"hittablelist.h"
#include"camera.h"
#include"rect.h"
#include <float.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "box.h"
#include"bvh.h"
using namespace std;


vec3 color(const ray& r, hittable *world, int depth ){
	hit_record rec;
	if(world->hit(r,0.001,FLT_MAX,rec)){
		ray scattered;
		vec3 attenuation;
		vec3 emmitted = rec.mat_ptr->emitted(rec.u,rec.v,rec.p);
		if(depth < 50 && rec.mat_ptr->scatter(r,rec,attenuation,scattered))
			return emmitted + attenuation*color(scattered,world, depth+1);
		else
			return emmitted;
	}
	else{
		// vec3 unit_direction = unit_vector(r.direction());
		// float t = 0.5*(unit_direction.y() + 1.0);
		// return (1.0 - t)*vec3(1.0,1.0,1.0) + t *vec3(0.5,0.7,1.0);
		return vec3(0,0,0);
	}
}

hittable *random_scene(){
	int n = 500;
	hittable **list = new hittable*[n+1];
	texture * checker = new checker_texture(new constant_texture(vec3(0.2,0.3,0.1)),new constant_texture(vec3(0.9,0.9,0.9)));
	list[0] = new sphere(vec3(0,-1000,0),1000,new lambertian(checker));
	int i = 1;
	for(int a = -11; a < 11; a++){
		for(int b = -11; b < 11; b++){
			float choose_mat = drand48();
			vec3 center(a+0.9*drand48(),0.2,b+0.9*drand48());
			if((center - vec3(4.0,2,0)).length() > 0.9){
				if(choose_mat < 0.8){ //diffuse
					list[i++] = new moving_sphere(center,center + vec3(0,0.5*drand48(),0),0.0,1.0,0.2,new lambertian(new constant_texture(vec3(drand48()*drand48(),drand48()*drand48(),drand48()*drand48()))));
				}
				else if(choose_mat < 0.95){//metal
					list[i++] = new sphere(center,0.2,new metal(vec3(0.5*(1+drand48()),0.5*(1+drand48()),0.5*(1+drand48())),0.5*drand48()));
				}
				else{//glass
					list[i++] = new sphere(center, 0.2,new dielectric(1.5));
				}
			}
		}
	}
	list[i++] = new sphere(vec3(0,1,0),1.0,new dielectric(1.5));
	list[i++] = new sphere(vec3(-4,1,0),1.0,new lambertian(new constant_texture(vec3(0.4,0.2,0.1))));
	list[i++] = new sphere(vec3(4,1,0),1.0,new metal(vec3(0.7,0.6,0.5),0.0));
	return new hittable_list(list,i);
}

hittable *two_spheres(){
    texture* checker = new checker_texture(new constant_texture(vec3(0.2,0.3,0.1)),new constant_texture(vec3(0.9,0.9,0.9)));
    int n = 50;
	hittable **list = new hittable*[n+1];
	list[0] = new sphere(vec3(0,-10,0),10,new lambertian(checker));
	list[1] = new sphere(vec3(0,10,0),10,new lambertian(checker));
	return new hittable_list(list,2);
}

hittable *two_perlin_spheres(){
    texture* pertext = new noise_texture(4);
	hittable **list = new hittable*[2];
	list[0] = new sphere(vec3(0,-1000,0),1000,new lambertian(pertext));
	list[1] = new sphere(vec3(0,2,0),2,new lambertian(pertext));
	return new hittable_list(list,2);
}

hittable *earth(){
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
	material *earth_surface = new lambertian(new image_texture(tex_data, nx, ny));
	hittable **list = new hittable*[1];
	list[0] = new sphere(vec3(0,0,0),2,earth_surface);
	return new hittable_list(list,1);
}

hittable *simple_light() {
        hittable** list = new hittable*[3];
		int i = 0;
        texture* pertext = new noise_texture(4);
        list[i++] = new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext));
        list[i++] = new sphere(vec3(0,2,0), 2, new lambertian (pertext));

        diffuse_light* difflight = new diffuse_light(new constant_texture(vec3(4,4,4)));
        //list[2] = new sphere(vec3(0,7,0), 2, difflight);
        list[i++] = new xy_rect(3, 5, 1, 3, -2, difflight);

        return new hittable_list(list,i);
}

hittable *cornell_box(){
	hittable **list = new hittable*[5];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65,0.05,0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	// list[i++] = new flip_normal(new yz_rect(0,555,0,555,555,green));
	// list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	// list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	// list[i++] = new flip_normal(new xz_rect(0,555,0,555,555,white));
	// list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	// list[i++] = new flip_normal(new xy_rect(0, 555, 0, 555, 555, white));
	
	list[i++] = new yz_rect(0,555,0,555,555,green);
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	//list[i++] = new flip_normal(new xz_rect(0,555,0,555,555,white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new xy_rect(0, 555, 0, 555, 555, white);
	//list[i++] = new box(vec3(130,0,65),vec3(295,165,230),white);
	//list[i++] = new box(vec3(265,0,295),vec3(430,330,460),white);
	
	//list[i++] = new translate(new rotate_y(new box(vec3(0,0,0),vec3(165,165,165),white),-18),vec3(130,0,65));
	//list[i++] = new translate(new rotate_y(new box(vec3(0,0,0),vec3(165,330,165),white),15),vec3(265,0,295));
	return new hittable_list(list,i);
}

hittable *cornell_smoke(){
	hittable **list = new hittable*[8];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65,0.05,0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	list[i++] = new flip_normal(new yz_rect(0,555,0,555,555,green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normal(new xz_rect(0,555,0,555,555,white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normal(new xy_rect(0, 555, 0, 555, 555, white));
	//ist[i++] = new box(vec3(130,0,65),vec3(295,165,230),white);
	//list[i++] = new box(vec3(265,0,295),vec3(430,330,460),white);
	hittable *b1 = new translate(new rotate_y(new box(vec3(0,0,0),vec3(165,165,165),white),-18),vec3(130,0,65));
	hittable *b2 = new translate(new rotate_y(new box(vec3(0,0,0),vec3(165,330,165),white),15),vec3(265,0,295));
	list[i++] = new constant_medium(b1,0.01,new constant_texture(vec3(1.0,1.0,1.0)));
	list[i++] = new constant_medium(b2,0.01,new constant_texture(vec3(0.0,0.0,0.0)));
	return new hittable_list(list,i);
}

hittable *final(){
	int nb = 20;
	hittable **list = new hittable*[30];
	hittable **boxlist = new hittable*[10000];
	hittable **boxlist2 = new hittable*[10000];
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
	
	int b = 0;
	for(int i = 0; i < nb; i++){
		for(int j = 0; j < nb; j++){
			float w = 100;
			float x0 = -1000 + i * w;
			float z0 = -1000 + j * w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100*(drand48()+0.01);
			float z1 = z0 + w;
			boxlist[b++] = new box(vec3(x0,y0,z0),vec3(x1,y1,z1),ground);
		}
	}

	int l = 0;
	//立方体依附在球上
	list[l++] = new bvh_node(boxlist,b,0,1);
	//光源
	material *light = new diffuse_light(new constant_texture(vec3(7,7,7)));
	list[l++] = new xz_rect(123,423,147,412,554,light);
	vec3 center(400,400,200);
	//移动球体
	list[l++] = new moving_sphere(center,center+vec3(30,0,0),0,1,50,new lambertian(new constant_texture(vec3(0.7,0.3,0.1))));
	//只会折射的绝缘球体
	list[l++] = new sphere(vec3(260,150,45),50,new dielectric(1.5));
	//金属球体
	list[l++] = new sphere(vec3(0,150,145),50,new metal(vec3(0.8,0.8,0.9),10.0));
	//球体光源
	hittable* boundary = new sphere(vec3(360,150,145),70,new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constant_medium(boundary,0.2,new constant_texture(vec3(0.2,0.4,0.9)));
	boundary = new sphere(vec3(0,0,0),5000,new dielectric(1.5));
	list[l++] = new constant_medium(boundary,0.0001,new constant_texture(vec3(1.0,1.0,1.0)));
	//地球
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
	material *earth_surface = new lambertian(new image_texture(tex_data, nx, ny));
	list[l++] = new sphere(vec3(400,200,400),100,earth_surface);
	//柏林噪音球
	texture *pertext = new noise_texture(0.1);
	list[l++] = new sphere(vec3(220,280,300),80,new lambertian(pertext));
	//多个球体组成的立方体
	int ns = 1000;
	for(int j = 0; j < ns; j++){
		boxlist2[j] = new sphere(vec3(165*drand48(),165*drand48(),165*drand48()),10,white);
	}
	//进行旋转
	list[l++] = new translate(new rotate_y(new bvh_node(boxlist2,ns,0.0,1.0),15),vec3(-100,270,395));
	return new hittable_list(list,l);
}
    

int main() {
	std::srand(std::time(0)); // 使用当前时间作为随机数种子

	int nx = 400;
	int ny = 200;
	int ns = 1000;
	cout << "P3\n" << nx << " " << ny << "\n255\n";
	hittable *world = simple_light();
	vec3 lookfrom(26,3,6);
	//vec3 lookfrom(0,0,-12);
	//vec3 lookfrom(278,278,-800);
	vec3 lookat(0,2,0);
	//vec3 lookat(278,278,0);
	vec3 vup (0,1,0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	camera cam(lookfrom,lookat,vup,vfov,float(nx/ny),aperture,dist_to_focus,0.0,1.0);
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; ++i) {
			vec3 col(0,0,0);
			for(int s = 0; s < ns; s++){
				float u = float(i + drand48())/float(nx);
				float v = float(j + drand48())/float(ny);
				ray r = cam.get_ray(u,v);
				vec3 p = r.at(2.0);
				col += color(r,world,0);
			}
			col /= float(ns);
			col = vec3(sqrt(col[0]),sqrt(col[1]),sqrt(col[2]));
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);
			cout << ir << " " << ig << " " << ib << "\n";
		}
	}
}