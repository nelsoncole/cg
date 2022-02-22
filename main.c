#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp.h"

struct vec3
{
	float x;
	float y;
	float z;
};

struct color
{
        float x;
        float y;
        float z;
};

struct ray
{
	struct vec3 origin;
	struct vec3 direction;
};


unsigned char *frontbuffer;
void putpixel(int x, int y, unsigned char *rgb, int pitch);
void save_bmp(int width, int height, const char *name);
void toyviewer(int width, int height);

struct vec3 vec3(float x, float y, float z){
	struct vec3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

// norma
float length_squared(struct vec3 *v){
      	float x = (v->x*v->x)+(v->y*v->y)+(v->z*v->z);
	return x;
}

float length(struct vec3 *v){
	float x = length_squared(v);
	x = sqrt(x);
	return x;
}


// operações
struct vec3 divec3(struct vec3 *v, float t){
	struct vec3 r;
	r.x = v->x*(1.0/t);
	r.y = v->y*(1.0/t);
	r.z = v->z*(1.0/t);
	return r;
}

struct vec3 mulvec3(struct vec3 *v, float t){
        struct vec3 r;
        r.x = v->x*t;
        r.y = v->y*t;
        r.z = v->z*t;
        return r;
}

struct vec3 sumvec3(struct vec3 *v1, struct vec3 *v2){
        struct vec3 r;
        r.x = v1->x + v2->x;
        r.y = v1->y + v2->y;
        r.z = v1->z + v2->z;
        return r;
}

struct vec3 subvec3(struct vec3 *u, struct vec3 *v){
        struct vec3 r;
        r.x = u->x - v->x;
        r.y = u->y - v->y;
        r.z = u->z - v->z;
        return r;
}



// produto escalar
float dot(struct vec3 *u, struct vec3 *v){
	float r = (u->x*v->x);
	r += (u->y*v->y);
	r += (u->z*v->z);
	return r;
}


//vector unitario
struct vec3 unitvector(struct vec3 *v){
	struct vec3 r = divec3(v, length(v));
	return r;
}

struct ray ray(struct vec3 *orig, struct vec3 *dir){
	struct ray r;
	memcpy(&r.origin, orig, sizeof(struct vec3));
	memcpy(&r.direction, dir, sizeof(struct vec3));
	return r;
}

struct vec3 at(float t, struct ray *r){
	struct vec3 v;
	v = mulvec3(&r->direction, t);
	v = sumvec3(&r->origin, &v);
	return v;
}

float hit_sphere(struct vec3 *center, float radius, struct ray *r){
        struct vec3 oc = subvec3(&r->origin, center);
        float a = length_squared(&r->direction);
        float b = dot(&oc, &r->direction);
        float c = length_squared(&oc) - radius*radius;
        float discriminant = b*b - a*c;
        if (discriminant < 0) {
        	return -1.0;
    	} else {

		printf("%.2f ",b);
        	float x = (-b - sqrt(discriminant) ) / a;
		if(x < 0.0) x = x*-1.0;
		return x;
    	}
}

struct color color(float x, float y, float z){
	struct color c;
	c.x = x;
	c.y = y;
	c.z = z;
	return c;
}

struct color mulcolor(struct color *u, float t){
	struct color c;
	c.x = u->x*t;
        c.y = u->y*t;
        c.z = u->z*t;
	return c;
}

struct color sumcolor(struct color *u, struct color *v){
        struct color c;
        c.x = u->x+v->x;
        c.y = u->y+v->y;
        c.z = u->z+v->z;
        return c;
}

struct color ray_color(struct ray *r) {

	struct vec3 center =vec3(0.0, 0.0, -1.0);
	float t = hit_sphere(&center, 0.5, r);
	if (t > 0){
		struct vec3 u = at(t, r);
		struct vec3 v = subvec3(&u, &center);
		struct vec3 n = unitvector(&v);
		struct color c =color(n.x+1.0,n.y+1.0,n.z+1.0);
		c = mulcolor(&c, 0.5);
        	return c;

	}
    	struct vec3 unit_direction = unitvector(&r->direction);
    	t = 0.5*(unit_direction.y + 1.0);
	struct color a = color(1.0, 1.0, 1.0);
	a = mulcolor(&a, (float)1.0-t);
	struct color b = color(0.5, 0.7, 1.0);
	b = mulcolor(&b, t);

    	return sumcolor(&a, &b);
}

unsigned int write_color(struct color *c){
	unsigned char rgb[4];

    	rgb[0] = (unsigned char)(255.999 * c->z);
     	rgb[1] = (unsigned char)(255.999 * c->y);
    	rgb[2] = (unsigned char)(255.999 * c->x);
	rgb[3] = 0;
	return *(unsigned int*)&rgb;
}

int main(){

	frontbuffer = (unsigned char *) malloc(0x400000);
	memset(frontbuffer, 0, 0x400000);

	// Image
	const float aspect_ratio = 16.0 / 9.0;
    	const int image_width = 400;
    	const int image_height = (const int)(image_width / aspect_ratio);

	// Camera
	float viewport_height = 2.0;
	float viewport_width = aspect_ratio * viewport_height;
	float focal_length = 1.0;

	struct vec3 origin = vec3(0.0, 0.0, 0.0);
	struct vec3 horizontal = vec3(viewport_width, 0.0, 0.0);
	struct vec3 vertical = vec3(0.0,viewport_height, 0.0);


	struct vec3 vector1 = divec3(&horizontal, 2);
	vector1 = subvec3(&origin, &vector1);
	struct vec3 vector2 = vec3(0, 0, focal_length);
	struct vec3 vector3 = divec3(&vertical, 2);
	vector2 = subvec3(&vector3, &vector2);

	struct vec3 lower_left_corner =subvec3(&vector1, &vector2);

	// Render
	int y = 0;
	for (int j = image_height-1; j >= 0; --j) {

        	for (int i = 0; i < image_width; ++i) {
            		float u = (float)i / (image_width-1);
            		float v = (float)j / (image_height-1);
			vector1 = mulvec3(&horizontal, u);
			vector1 = sumvec3(&lower_left_corner, &vector1);
			vector2 = mulvec3(&vertical, v);
			vector1 = sumvec3(&vector1, &vector2);
			struct vec3 dir = subvec3(&vector1, &origin);
			struct ray r = ray(&origin, &dir);
          		struct color pixel_color = ray_color(&r);
			unsigned int rgb = write_color(&pixel_color);
			putpixel(i, y, (unsigned char*)&rgb, image_width*3);
        	}
		y++;
    	}

	save_bmp(image_width, image_height,  "image2.bmp");
	free(frontbuffer);
	return 0;
}

void toyviewer(int width, int height){
	int z = height-1;
        for(int j=0;j < height; j++){
                for(int i=0; i < width; i++){
                        unsigned char rgb[4];

                        float r = (float)i /(width-1);
                        float g = (float)z /(height-1);
                        float b = 0.25;
                        rgb[0] = (unsigned char)(255.999 * b);
                        rgb[1] = (unsigned char)(255.999 * g);
                        rgb[2] = (unsigned char)(255.999 * r);
                        putpixel(i, j, rgb, width*3);
                }
                z--;
        }
}

void putpixel(int x, int y, unsigned char *rgb, int pitch){
        unsigned char *memory = frontbuffer;
        memory += y*pitch+x*3;
        *memory++ = rgb[0];
        *memory++ = rgb[1];
        *memory++ = rgb[2];
}



void save_bmp(int width, int height, const char *name){
        FILE *img = fopen(name,"wb");

        BMP b;
        memset(&b, 0, sizeof(BMP));
        b.bformat.type = 0x4D42;
        b.bformat.size = sizeof(BMP)+(width*height*3);
        b.bformat.offset_bits = sizeof(BMP);
        b.binfo.size = sizeof(BMP_INFO);
        b.binfo.width = width;
        b.binfo.height = height;
        b.binfo.count = 24;
        b.binfo.size_imag = width * height * 3;

        fwrite(&b,1, sizeof(BMP), img);
        for(int y=height-1; y >=0; --y){
                for(int x=0; x < width; x++){
                        unsigned char *rgb = frontbuffer;
                        rgb += y*width*3 + x*3;
                        fwrite(rgb, 1, 3, img);
                }
        }

        fflush(img);
        fclose(img);

}
