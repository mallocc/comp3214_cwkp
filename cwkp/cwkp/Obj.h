#pragma once

#include "opengl.h"
#include "glm.h"
#include "varhandle.h"
#include "vertex.h"
#include "load_image.h"

#include <vector>

#define GEN_NORMS 0x1
#define GEN_TANGS 0x2
#define GEN_UVS_POLAR 0x4
#define GEN_UVS_RECTS 0x8
#define GEN_UVS_SPHERE 0x80
#define GEN_MAP_HEIGHTS 0x100
#define GEN_UV_HEIGHTS 0x200
#define GEN_ALL (GEN_NORMS | GEN_TANGS | GEN_UVS_POLAR)
#define GEN_COLOR 0x10
#define GEN_COLOR_RAND 0x20
#define GEN_COLOR_RAND_I 0x40
#define GEN_DEFAULT (GEN_NORMS | GEN_COLOR)


//prototypes
std::vector<glm::vec3>			subdivide(std::vector<glm::vec3> v);
std::vector<glm::vec3>			generate_cube();
std::vector<glm::vec3>			generate_cone(int k);
std::vector<glm::vec3>			generate_cylinder(int k, float len);
std::vector<glm::vec3>			generate_sphere(int lats, int longs);
std::vector<glm::vec3>			generate_rect();
std::vector<glm::vec3>			generate_rects(int w, int h);
std::vector<glm::vec2>			generate_uv_rects(int w, int h);
std::vector<glm::vec3>			generate_normals(std::vector<glm::vec3> v);
std::vector<glm::vec3>			generate_map_heights(std::vector<glm::vec3> v, std::vector<glm::vec3> n, image_data * image, float k);
std::vector<glm::vec3>			generate_map_heights_from_uvs(std::vector<glm::vec3> v, std::vector<glm::vec3> n, std::vector<glm::vec2> uv, image_data * image, float k);
std::vector<glm::vec3>			generate_tangents(std::vector<glm::vec3> v);
std::vector<glm::vec2>			generate_polar_uvs(std::vector<glm::vec3> v);
std::vector<glm::vec2>			generate_sphereical_uvs(std::vector<glm::vec3> v);
std::vector<glm::vec2>			generate_repeated_rect_uvs(std::vector<glm::vec3> v);
std::vector<glm::vec3>			generate_colour_buffer(glm::vec3 colour, int n);
std::vector<glm::vec3>			random_colour_buffer(glm::vec3 max, int n);
std::vector<glm::vec3>			random_intesity_colour_buffer(glm::vec3 colour, int n);

// creates a vector of Vertices to pass to Obj
std::vector<Vertex>				pack_object(
	std::vector<glm::vec3> * v, 
	unsigned int flags, 
	glm::vec3 color
);
std::vector<Vertex>				pack_object(
	std::vector<glm::vec3> * v,
	unsigned int flags,
	glm::vec3 color,
	image_data * image,
	float k
);
// normal packer of custom properties
std::vector<Vertex>				pack_object(
	std::vector<glm::vec3> * v, 
	std::vector<glm::vec3> * c, 
	std::vector<glm::vec3> * n, 
	std::vector<glm::vec2> * uv, 
	std::vector<glm::vec3> * t
);


struct Obj
{
private:

	GLuint 
		vao, 
		buffer, 
		tex = GL_TEXTURE0, 
		norm = GL_TEXTURE0;
	int 
		data_size = 0;

	void load_texture_handle(VarHandle * handle)
	{
		handle->load(tex);
	}
	void load_normal_handle(VarHandle * handle)
	{
		handle->load(norm);
	}

	void init(std::vector<Vertex>  * d);

	void load_textures(
		const char *texfilename, 
		const char *normfilename
	);

public:
	glm::vec3
		rotation = glm::vec3(0, 1, 0),
		scale = glm::vec3(1, 1, 1), 
		pos;
	GLfloat 
		theta;


	Obj() {}

	Obj(
		const char *filename,
		glm::vec3 c,
		glm::vec3 _pos,
		glm::vec3 _rotation,
		GLfloat _theta,
		glm::vec3 _scale
	);
	Obj(
		const char *filename,
		const char *texfilename,
		const char *normfilename,
		glm::vec3 c,
		glm::vec3 _pos,
		glm::vec3 _rotation,
		GLfloat _theta,
		glm::vec3 _scale
	);
	Obj::Obj(
		const char *filename,
		const char *texfilename,
		const char *normfilename,
		const char *heightmapfilename,
		glm::vec3 c,
		glm::vec3 _pos,
		glm::vec3 _rotation,
		GLfloat _theta,
		glm::vec3 _scale
	);
	Obj::Obj(
		int k,
		const char *filename,
		const char *texfilename,
		const char *normfilename,
		const char *heightmapfilename,
		glm::vec3 c,
		glm::vec3 _pos,
		glm::vec3 _rotation,
		GLfloat _theta,
		glm::vec3 _scale
	);
	Obj(
		const char *texfilename,
		const char *normfilename,
		std::vector<Vertex>	data,
		glm::vec3 _pos,
		glm::vec3 _rotation,
		GLfloat _theta,
		glm::vec3 _scale
	);

	void draw(
		int wire_frame,
		VarHandle *model,
		VarHandle *texture_handle,
		VarHandle *normalmap_handle
	);
	void draw_array(
		int wire_frame, 
		VarHandle *texture_handle,
		VarHandle *normalmap_handle
	);

};


struct CompositeObj
{
private:
	std::vector<Obj> 
		objects;
public:
	glm::vec3
		rotation = glm::vec3(1, 0, 0),
		scale = glm::vec3(1, 1, 1),
		pos;
	GLfloat 
		theta;

	CompositeObj() {};
	CompositeObj(
		glm::vec3  _pos,
		glm::vec3 _rotation,
		GLfloat _theta,
		glm::vec3 _scale
	);

	void draw(
		int wire_frame,
		VarHandle *model,
		VarHandle *texture_handle,
		VarHandle *normalmap_handle
	);
	void add(Obj e);
};
