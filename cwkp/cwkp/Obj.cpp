#include "Obj.h"
#include <math.h>  
#include "tiny_object_loader\tiny_obj_loader.h"

// converts cartesian to polar
glm::vec2 cart_polar(glm::vec3 v)
{
	float r = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	return glm::vec2(atan(v.y / v.x), acos(v.z / r));
}
// converts polar to cartesian
glm::vec3 polar_cart(float theta, float phi)
{
	return glm::vec3(cos(theta)*cos(phi), cos(theta) * sin(phi), sin(theta));
}
//Returns random float
inline float		randf()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}


//Cube vertex data array
GLfloat cube_v_b[] = {
	-1.0f,-1.0f,-1.0f, // triangle 1 : begin
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f, // triangle 1 : end
	1.0f, 1.0f,-1.0f, // triangle 2 : begin
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f, // triangle 2 : end
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f
};


///shape generators non indexed triangles
std::vector<glm::vec3>			generate_cube()
{
	std::vector<glm::vec3> v;
	for (int i = 0; i < 36; i++)
		v.push_back(glm::vec3(cube_v_b[i * 3], cube_v_b[i * 3 + 1], cube_v_b[i * 3 + 2]));
	return v;
}
std::vector<glm::vec3>			generate_cone(int k)
{
	std::vector<glm::vec3> v;
	float step = 2.0f * 3.141596f / float(k);
	float c = 0.0f, s = 0.0f;
	float len = 2.0f;
	//cone
	for (float a = 0; a <= (2.0f * 3.141596f); a += step)
	{
		v.push_back(glm::vec3());
		c = cos(a);
		s = sin(a);
		v.push_back(glm::vec3(c, s, len));
		c = cos(a - step);
		s = sin(a - step);
		v.push_back(glm::vec3(c, s, len));
	}
	//circle
	for (float a = 0; a <= (2.0f * 3.141596f); a += step)
	{
		c = cos(a - step);
		s = sin(a - step);
		v.push_back(glm::vec3(c, s, len));
		c = cos(a);
		s = sin(a);
		v.push_back(glm::vec3(c, s, len));
		v.push_back(glm::vec3(0.0f, 0.0f, len));
	}
	return v;
}
std::vector<glm::vec3>			generate_cylinder(int k, float len)
{
	std::vector<glm::vec3> v;
	glm::vec3 t1, t2;
	float step = 2. * 3.141596 / float(k);
	float Radius = 1., c = 0., s = 0.;
	for (float a = 0; a <= (2. * 3.141596); a += step)
	{
		v.push_back(glm::vec3(0., 0., 0));
		c = Radius * cos(a);
		s = Radius * sin(a);
		v.push_back(glm::vec3(c, s, 0));
		c = Radius * cos(a - step);
		s = Radius * sin(a - step);
		v.push_back(glm::vec3(c, s, 0));
	}
	for (float a = 0; a > -(2. * 3.141596); a -= step)
	{
		c = Radius * cos(a);
		s = Radius * sin(a);
		t1 = glm::vec3(c, s, 0);
		t2 = glm::vec3(c, s, len);
		c = Radius * cos(a - step);
		s = Radius * sin(a - step);

		v.push_back(t1);
		v.push_back(t2);
		v.push_back(glm::vec3(c, s, len));
		v.push_back(glm::vec3(c, s, len));
		v.push_back(glm::vec3(c, s, 0));
		v.push_back(t1);
	}
	for (float a = 0; a <= (2. * 3.141596); a += step)
	{
		c = Radius * cos(a - step);
		s = Radius * sin(a - step);
		v.push_back(glm::vec3(c, s, len));
		c = Radius * cos(a);
		s = Radius * sin(a);
		v.push_back(glm::vec3(c, s, len));
		v.push_back(glm::vec3(0., 0., len));
	}
	return v;
}
std::vector<glm::vec3>			generate_sphere(int lats, int longs)
{
	std::vector<glm::vec3> v;
	float step_lats = glm::radians(360.0f) / float(lats);
	float step_longs = glm::radians(360.0f) / float(longs);
	float Radius = 1., x, y, z;
	for (float a = 0; a <= glm::radians(360.0f); a += step_lats)
		for (float b = 0; b <= glm::radians(360.0f); b += step_longs)
		{
			v.push_back(polar_cart(a, b));
			v.push_back(polar_cart(a + step_lats, b));
			v.push_back(polar_cart(a + step_lats, b + step_longs));
			v.push_back(polar_cart(a + step_lats, b + step_longs));
			v.push_back(polar_cart(a, b + step_longs));
			v.push_back(polar_cart(a, b));

		}
	return v;
}
std::vector<glm::vec3>			generate_rect()
{
	std::vector<glm::vec3> n;
	n.push_back(glm::vec3(1, 0, 1));
	n.push_back(glm::vec3(1, 0, -1));
	n.push_back(glm::vec3(-1, 0, -1));
	n.push_back(glm::vec3(-1, 0, -1));
	n.push_back(glm::vec3(-1, 0, 1));
	n.push_back(glm::vec3(1, 0, 1));
	return n;
}
// generates normals from every triangle
std::vector<glm::vec3>			generate_normals(std::vector<glm::vec3> v)
{
	std::vector<glm::vec3> n;
	for (int i = 0; i < v.size(); i += 3)
	{
		glm::vec3 nm = glm::normalize(glm::cross(v[i + 1] - v[i], v[i + 2] - v[i]));
		for (int j = 0; j < 3; ++j)
			n.push_back(nm);
	}
	return n;
}
// generates a second normal (tangent) for every normal (used for normal mapping)
std::vector<glm::vec3>			generate_tangents(std::vector<glm::vec3> v)
{
	std::vector<glm::vec3> n;
	for (int i = 0; i < v.size(); i += 6)
	{
		glm::vec3 nm = glm::normalize(v[i + 2] - v[i]);
		for (int j = 0; j < 6; ++j)
			n.push_back(nm);
	}
	return n;
}
// generates uvs from converted carts to polar
std::vector<glm::vec2>			generate_polar_uvs(std::vector<glm::vec3> v)
{
	std::vector<glm::vec2> uv;
	for (int i = 0; i < v.size(); i++)
		uv.push_back(cart_polar(v[i]));
	return uv;
}
std::vector<glm::vec2>			generate_sphereical_uvs(std::vector<glm::vec3> v)
{
	std::vector<glm::vec2> uv;
	for (int i = 0; i < v.size(); i++)
	{
		uv.push_back(glm::vec2((atan2(v[i].y, v[i].x) / 3.1415926f + 1.0f) * 0.5, (asin(v[i].z) / 3.1415926 + 0.5)));
	}
	return uv;
}
std::vector<glm::vec2>			generate_repeated_rect_uvs(std::vector<glm::vec3> v)
{
	std::vector<glm::vec2> uv;
	for (int i = 0; i < v.size(); i += 6)
	{
		uv.push_back(glm::vec2(0, 0));
		uv.push_back(glm::vec2(1, 0));
		uv.push_back(glm::vec2(1, 1));
		uv.push_back(glm::vec2(1, 1));
		uv.push_back(glm::vec2(0, 1));
		uv.push_back(glm::vec2(0, 0));
	}
	return uv;
}
// generates a single color buffer
std::vector<glm::vec3>			generate_colour_buffer(glm::vec3 colour, int n)
{
	std::vector<glm::vec3> v;
	for (int i = 0; i < n; i++)
		v.push_back(colour);
	return v;
}
// generates a random color buffer where max is the cap color
std::vector<glm::vec3>			random_colour_buffer(glm::vec3 max, int n)
{
	std::vector<glm::vec3> v;
	for (int i = 0; i < n; i++)
		v.push_back(glm::vec3(max.x*randf(), max.y*randf(), max.z*randf()));
	return v;
}
// generates a single color buffer of random intensities
std::vector<glm::vec3>			random_intesity_colour_buffer(glm::vec3 colour, int n)
{
	std::vector<glm::vec3> v;
	float f;
	for (int i = 0; i < n; i++)
	{
		f = randf();
		v.push_back(glm::vec3(colour.x*f, colour.y*f, colour.z*f));
	}
	return v;
}
// creates a vector of Vertices to pass to Obj
std::vector<Vertex>				pack_object(
	std::vector<glm::vec3> * v,
	unsigned int flags, 
	glm::vec3 color
)
{
	std::vector<Vertex> object;
	std::vector<glm::vec3> n, c, t;
	std::vector<glm::vec2> uv;

	if (flags == NULL)
		flags = GEN_DEFAULT;

	if ((flags & GEN_NORMS) == GEN_NORMS)
		n = generate_normals(*v);
	if ((flags & GEN_COLOR) == GEN_COLOR)
		c = generate_colour_buffer(color, v->size());
	if ((flags & GEN_COLOR_RAND) == GEN_COLOR_RAND)
		c = random_colour_buffer(color, v->size());
	if ((flags & GEN_COLOR_RAND_I) == GEN_COLOR_RAND_I)
		c = random_intesity_colour_buffer(color, v->size());
	if ((flags & GEN_UVS_POLAR) == GEN_UVS_POLAR)
		uv = generate_polar_uvs(*v);
	if ((flags & GEN_UVS_RECTS) == GEN_UVS_RECTS)
		uv = generate_repeated_rect_uvs(*v);
	if ((flags & GEN_UVS_SPHERE) == GEN_UVS_SPHERE)
		uv = generate_sphereical_uvs(*v);
	if ((flags & GEN_TANGS) == GEN_TANGS)
		t = generate_tangents(*v);

	for (int i = 0; i < v->size(); ++i)
	{
		Vertex vert;
		if (v->size() != 0)
			vert.position = (*v)[i];
		if (c.size() != 0)
			vert.color = c[i];
		if (n.size() != 0)
			vert.normal = n[i];
		if (uv.size() != 0)
			vert.uv = uv[i];
		if (t.size() != 0)
			vert.tangent = t[i];
		object.push_back(vert);
	}
	return object;
}
// normal packer of custom properties
std::vector<Vertex>				pack_object(
	std::vector<glm::vec3> * v, 
	std::vector<glm::vec3> * c, 
	std::vector<glm::vec3> * n, 
	std::vector<glm::vec2> * uv, 
	std::vector<glm::vec3> * t
)
{
	std::vector<Vertex> object;
	for (int i = 0; i < v->size(); ++i)
	{
		Vertex vert;
		if (v != NULL)
			vert.position = (*v)[i];
		if (c != NULL)
			vert.color = (*c)[i];
		if (n != NULL)
			vert.normal = (*n)[i];
		if (uv != NULL)
			vert.uv = (*uv)[i];
		if (t != NULL)
			vert.tangent = (*t)[i];
		object.push_back(vert);
	}
	return object;
}


// Obj code bodies
Obj::Obj(
	const char *filename,
	glm::vec3 c,
	glm::vec3 _pos,
	glm::vec3 _rotation,
	GLfloat _theta,
	glm::vec3 _scale
)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::vector< glm::vec3 > vertices;
	std::vector< Vertex > o;

	std::string obj_err =
		tinyobj::LoadObj(shapes, materials, filename, NULL);

	for (int i = 0; i < shapes.size(); i++)
		for (int j = 0; j < shapes[i].mesh.indices.size(); j++)
			vertices.push_back(glm::vec3(
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3],
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3 + 1],
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3 + 2]
			));

	std::vector<Vertex> data = pack_object(&vertices, GEN_ALL | GEN_COLOR, c);

	printf("New object file loaded: \n   [ \n%s \n   ]\n   Vertex count: %i\n", obj_err, data.size());

	pos = _pos;
	rotation = _rotation;
	theta = _theta;
	scale = _scale;

	init(&data);
}
Obj::Obj(
	const char *filename,
	const char *texfilename,
	const char *normfilename,
	glm::vec3 c,
	glm::vec3 _pos,
	glm::vec3 _rotation,
	GLfloat _theta,
	glm::vec3 _scale
)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::vector< glm::vec3 > vertices;
	std::vector< Vertex > o;

	std::string obj_err =
		tinyobj::LoadObj(shapes, materials, filename, NULL);

	for (int i = 0; i < shapes.size(); i++)
		for (int j = 0; j < shapes[i].mesh.indices.size(); j++)
			vertices.push_back(glm::vec3(
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3],
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3 + 1],
				shapes[i].mesh.positions[shapes[i].mesh.indices[j] * 3 + 2]
			));

	std::vector<Vertex> data = pack_object(&vertices, GEN_ALL | GEN_COLOR, c);

	printf("New object file loaded: \n   [ \n%s \n   ]\n   Vertex count: %i\n", obj_err, data.size());

	pos = _pos;
	rotation = _rotation;
	theta = _theta;
	scale = _scale;

	load_textures(texfilename, normfilename);
	init(&data);
}
Obj::Obj(
	const char *texfilename, 
	const char *normfilename,
	std::vector<Vertex>	data,
	glm::vec3 _pos,
	glm::vec3 _rotation,
	GLfloat _theta,
	glm::vec3 _scale
)
{
	printf("New primitive object loaded:\n   Vertex count: %i\n", data.size());

	pos = _pos;
	rotation = _rotation;
	theta = _theta;
	scale = _scale;

	load_textures(texfilename, normfilename);
	init(&data);
}
void Obj::load_textures(
	const char *texfilename, 
	const char *normfilename
)
{
	if (texfilename != "")
	{
		tex = load_texture_from_image(texfilename);
		printf("   Texture file:   %s -> \n", texfilename, tex);
	}
	else
	{
		printf("   No texture loaded\n");

	}

	if (normfilename != "")
	{
		norm = load_texture_from_image(normfilename);
		printf("   NormalMap file: %s -> \n", normfilename, norm);
	}
	else
	{
		printf("   No normal map loaded\n");
	}
}
void Obj::init(std::vector<Vertex> * d)
{
	data_size = d->size();
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, data_size * sizeof(struct Vertex), d->data(), GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
		(const GLvoid*)offsetof(struct Vertex, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
		(const GLvoid*)offsetof(struct Vertex, color));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
		(const GLvoid*)offsetof(struct Vertex, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer((GLuint)3, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
		(const GLvoid*)offsetof(struct Vertex, uv));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer((GLuint)4, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
		(const GLvoid*)offsetof(struct Vertex, tangent));
	glEnableVertexAttribArray(4);
	glBindVertexArray(0);
	printf("   Buffered VAO -> %i\n", vao);
	glFlush();
}
void Obj::draw(
	int wire_frame, 
	VarHandle *model, 
	VarHandle *texture_handle,
	VarHandle *normalmap_handle
)
{
	glm::mat4 m =
		glm::translate(glm::mat4(1.), pos) *
		glm::rotate(glm::mat4(1.), theta, rotation) *
		glm::scale(glm::mat4(1.), scale);
	model->load(m);

	draw_array(wire_frame, texture_handle, normalmap_handle);
}
void Obj::draw_array(
	int wire_frame, 
	VarHandle *texture_handle, 
	VarHandle *normalmap_handle)
{
	load_texture_handle(texture_handle);
	load_normal_handle(normalmap_handle);

	glActiveTexture(GL_TEXTURE0 + tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glActiveTexture(GL_TEXTURE0 + norm);
	glBindTexture(GL_TEXTURE_2D, norm);

	glBindVertexArray(vao);
	glDrawArrays(wire_frame ? GL_LINE_LOOP : GL_TRIANGLES, 0, data_size);
	glBindVertexArray(0);

	glFinish();
}



CompositeObj::CompositeObj(
	glm::vec3 _pos, 
	glm::vec3 _rotation, 
	GLfloat _theta, 
	glm::vec3 _scale
)
{
	pos = _pos;
	rotation = _rotation;
	theta = _theta;
	scale = _scale;
}
void CompositeObj::draw(
	int wire_frame,
	VarHandle *model,
	VarHandle *texture_handle,
	VarHandle *normalmap_handle
)
{
	glm::mat4 comp_m = glm::translate(glm::mat4(1.), pos) * glm::rotate(glm::mat4(1.), theta, rotation) * glm::scale(glm::mat4(1.), scale);

	for (Obj e : objects)
	{
		glm::mat4 m =
			glm::translate(glm::mat4(1.), pos) *
			glm::rotate(glm::mat4(1.), theta, rotation) *
			glm::scale(glm::mat4(1.), scale);
		m = comp_m * m;
		model->load(m);
		e.draw_array(wire_frame, texture_handle, normalmap_handle);
	}

}
void CompositeObj::add(Obj e)
{
	objects.push_back(e);
}

