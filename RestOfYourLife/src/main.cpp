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
#include"pdf.h"
#include <fstream>
using namespace std;


vec3 color(const ray& r, hittable *world,hittable* light_shape, int depth ){
	hit_record hrec;
	if(world->hit(r,0.001,FLT_MAX,hrec)){
		scatter_record srec;
		vec3 emitted = hrec.mat_ptr->emitted(r,hrec,hrec.u,hrec.v,hrec.p);
		if(depth < 50 && hrec.mat_ptr->scatter(r,hrec,srec)){
			if(srec.is_specular){
				return srec.attenuation * color(srec.specular_ray,world,light_shape,depth+1);
			}
			else{
				hittable_pdf plight(light_shape,hrec.p);
				mixture_pdf p(&plight,srec.pdf_ptr);
				ray scattered = ray(hrec.p,p.generate(),r.time());
				float pdf_val = p.value(scattered.direction());
				delete srec.pdf_ptr;
				return emitted + srec.attenuation*hrec.mat_ptr->scattering_pdf(r,hrec,scattered)*color(scattered,world,light_shape,depth+1)/pdf_val;
			}
		}
		else
			return emitted;
	}
	else{
		return vec3(0,0,0);
	}
}

void cornell_box(hittable** world,camera **cam,float aspect){
	hittable **list = new hittable*[8];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65,0.05,0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	list[i++] = new flip_normal(new yz_rect(0,555,0,555,555,green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new flip_normal(new xz_rect(213, 343, 227, 332, 554, light));
	list[i++] = new flip_normal(new xz_rect(0,555,0,555,555,white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normal(new xy_rect(0, 555, 0, 555, 555, white));
	//list[i++] = new translate(new rotate_y(new box(vec3(0,0,0),vec3(165,165,165),white),-18),vec3(130,0,65));
	list[i++] = new sphere(vec3(190,90,190),90,new dielectric(1.5));
	//material *aluminum =new metal(vec3(0.8,0.85,0.88),0.0);
	list[i++] = new translate(new rotate_y(new box(vec3(0,0,0),vec3(165,330,165),white),15),vec3(265,0,295));
	//list[i++] =  new sphere(vec3(190,90,190),90,0);
	*world =  new hittable_list(list,i);

	vec3 lookfrom(278,278,-800);
	vec3 lookat(278,278,0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	*cam = new camera(lookfrom,lookat,vec3(0,1,0),vfov,aspect,aperture,dist_to_focus,0.0,1.0);
}

int main() {
	std::srand(std::time(0)); // 使用当前时间作为随机数种子

	int nx = 200;
	int ny = 200;
	int samples_per_pixels = 1000;
	ofstream file("image.ppm");
	streambuf* original_cout_buffer = cout.rdbuf();
	cout.rdbuf(file.rdbuf());

	cout << "P3\n" << nx << " " << ny << "\n255\n";
	hittable *world;
	camera *cam;
	cornell_box(&world,&cam,float(nx/ny));
	hittable *light_shape = new xz_rect(213,343,227,332,554,new material());
	hittable *glass_sphere = new sphere(vec3(190,90,190),90,new material());
	hittable *a[2];
	a[0] = light_shape;
	a[1] = glass_sphere;
	hittable_list hlist(a,2);
	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; ++i) {
			vec3 col(0,0,0);
			for(int s = 0; s < samples_per_pixels; s++){
				float u = float(i + drand48())/float(nx);
				float v = float(j + drand48())/float(ny);
				ray r = cam->get_ray(u,v);
				vec3 p = r.at(2.0);
				col += de_nan(color(r,world,&hlist,0));
			}
			col /= float(samples_per_pixels);
			col = vec3(sqrt(col[0]),sqrt(col[1]),sqrt(col[2]));
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);
			cout << ir << " " << ig << " " << ib << "\n";
		}
	}

	file.close();
	return 0;
}