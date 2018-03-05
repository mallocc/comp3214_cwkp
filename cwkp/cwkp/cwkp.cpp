#include "cwkp.h"
#include "colors.h"
#include <chrono>
#include <thread>

GLSLProgramManager	programs;

VarHandle
	model_mat_handle, 
	view_mat_handle, 
	proj_mat_handle, 
	tex_handle, 
	normmap_handle,
	heightmap_handle,
	light_handles[5],
	ambient_color_handle, 
	eye_dir_handle;

glm::vec2
	window_size(1280, 720);

glm::vec3
	eye_position(0, 1, 3),
	eye_direction,
	ambient_color = glm::vec3(0.1f,0.2f,0.3f);

glm::mat4 
	model, 
	view, 
	projection;

Obj
sphere,
object,
sphere2,
square,
square2;

Light lights = { glm::vec3(0,5,10),glm::vec3(1,1,1),100,0.8,100 };

//Error callback  
static void		error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

static void		key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ENTER:
			eye_position = glm::vec3(0,0,1) * eye_position;
			break;
		case GLFW_KEY_UP:
			eye_position += eye_position * 0.1f;
			break;
		case GLFW_KEY_DOWN:
			eye_position -= eye_position * 0.1f;
			break;
		case GLFW_KEY_RIGHT:
			eye_position = glm::quat(glm::vec3(0, glm::radians(3.0f), 0)) * eye_position;
			break;
		case GLFW_KEY_LEFT:
			eye_position = glm::quat(glm::vec3(0, -glm::radians(3.0f), 0)) * eye_position;
			break;
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
	}
}


void loop()
{
	//lights.pos = glm::quat(glm::vec3(0, 0.005f, 0)) * lights.pos;
	eye_position = glm::quat(glm::vec3(0, 0.001f, 0)) * eye_position;

	//// LOAD GLOBAL HANDLES
	view_mat_handle.load();
	proj_mat_handle.load();
	for (VarHandle v : light_handles)
		v.load();
	ambient_color_handle.load();
	eye_dir_handle.load();

	//// DRAW OBJECTS

	sphere.draw(0, &model_mat_handle, &tex_handle, &normmap_handle, &heightmap_handle);
	//sphere2.draw(0,&model_mat_handle,&tex_handle,&normmap_handle, &heightmap_handle);

	//object.draw(0, &model_mat_handle, &tex_handle, &normmap_handle, &heightmap_handle);

	square.draw(0, &model_mat_handle, &tex_handle, &normmap_handle, &heightmap_handle);
	//square2.draw(0,&model_mat_handle,&tex_handle,&normmap_handle, &heightmap_handle);

	
}

//Initilise custom objects
void			init()
{
	//// CREATE GLSL PROGAMS
	printf("\n");
	printf("Initialising GLSL programs...\n");
	programs.add_program("shaders/basic.vert", "shaders/basic.frag");
	programs.add_program("shaders/complex.vert", "shaders/complex.frag");
	programs.add_program("shaders/complex2.vert", "shaders/complex2.frag");



	//// CREATE HANDLES
	printf("\n");
	printf("Initialising variable handles...\n");
	model_mat_handle = VarHandle("u_m", &model);
	model_mat_handle.init(programs.current_program);

	view_mat_handle = VarHandle("u_v", &view);
	view_mat_handle.init(programs.current_program);

	proj_mat_handle = VarHandle("u_p", &projection);
	proj_mat_handle.init(programs.current_program);

	tex_handle = VarHandle("u_tex");
	tex_handle.init(programs.current_program);

	normmap_handle = VarHandle("u_norm");
	normmap_handle.init(programs.current_program);

	heightmap_handle = VarHandle("u_height");
	heightmap_handle.init(programs.current_program);

	light_handles[0] = VarHandle("u_light_pos", &lights.pos);
	light_handles[1] = VarHandle("u_diffuse_color", &lights.color);
	light_handles[2] = VarHandle("u_brightness", &lights.brightness);
	light_handles[3] = VarHandle("u_shininess", &lights.shininess);
	light_handles[4] = VarHandle("u_specular_scale", &lights.specular_scale);

	for (int i = 0; i < 5; ++i)
		light_handles[i].init(programs.current_program);

	eye_dir_handle = VarHandle("u_eye_pos", &eye_position);
	eye_dir_handle.init(programs.current_program);

	ambient_color_handle = VarHandle("u_ambient_color", &ambient_color);
	ambient_color_handle.init(programs.current_program);
	
	
	
	//// CREATE OBJECTS
	printf("\n");
	printf("Initialising objects...\n");
	// create sphere data for screen A, B and D
	std::vector<glm::vec3> v = generate_sphere(100, 100);
	//v = subdivide(v);
	std::vector<glm::vec3> n = generate_normals(v);
	std::vector<glm::vec2> uv = generate_sphereical_uvs(v);
	std::vector<glm::vec3> t = generate_tangents(v);
	image_data image = get_data("textures/moss_height.jpg");
	//v = generate_map_heights_from_uvs(v, n, uv, &image, 0.1f);
	std::vector<Vertex> o = pack_object(&v,NULL,&n,&uv,&t);//pack_object(&v, GEN_ALL | GEN_UVS_SPHERE, GREY);

	sphere = Obj(
		"textures/moss_color.jpg",
		"textures/moss_norm.jpg",
		"textures/moss_height.jpg",
		o,
		glm::vec3(),
		glm::vec3(1, 0, 0),
		glm::radians(90.0f),
		glm::vec3(1, 1, 1)
	);

	//sphere2 = Obj(
	//	"textures/metal_color.jpg",
	//	"textures/metal_norm.jpg",
	//	"textures/metal_height.jpg",
	//	o,
	//	glm::vec3(1,0,0),
	//	glm::vec3(1, 0, 0),
	//	glm::radians(90.0f),
	//	glm::vec3(1, 1, 1)
	//);

	//object = Obj(
	//	0,
	//	"objects/bunny.obj",
	//	"textures/metal_color.jpg",
	//	"textures/metal_norm.jpg",
	//	"textures/metal_height.jpg",
	//	glm::vec3(),
	//	glm::vec3(-1,0,0),
	//	glm::vec3(1, 0, 0),
	//	glm::radians(90.0f),
	//	glm::vec3(1, 1, 1) //* 0.01f
	//);

	square = Obj(
			"textures/metal_color.jpg",
			"textures/metal_norm.jpg",
			"textures/metal_height.jpg",
		pack_object(&generate_rect(), GEN_ALL | GEN_UVS_RECTS, GREY),
		glm::vec3(0,-1,0),
		glm::vec3(0, 1, 0),
		glm::radians(0.0f),
		glm::vec3(1, 1, 1)
	);

	square2 = Obj(
		"textures/moss_color.jpg",
		"textures/moss_norm.jpg",
		"textures/moss_height.jpg",
		pack_object(&generate_rect(), GEN_ALL | GEN_UVS_RECTS, GREY),
		glm::vec3(),
		glm::vec3(0, 1, 0),
		glm::radians(90.0f),
		glm::vec3(1, 1, 1)
	);
}


//GL graphics loop
void			glLoop(void(*graphics_loop)(), GLFWwindow * window)
{
	printf("\n");
	printf("Running GL loop...\n");

	//Main Loop  
	do
	{
		// start clock for this tick
		auto start = std::chrono::high_resolution_clock::now();

		//Clear color buffer  
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set clear color
		glClearColor(0.0f,0.0f,0.0f, 1.);

		projection = glm::perspective(glm::radians(45.0f), (float)window_size.x / (float)window_size.y, 0.001f, 1000.0f);
		view = glm::lookAt(eye_position, eye_direction, glm::vec3(0,1,0));

		// call the graphics loop
		graphics_loop();

		//Swap buffers  
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();

		// stop clock
		auto finish = std::chrono::high_resolution_clock::now();
		int ms = float(std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count());
		long newWait = 5 - ms;// -(gm.gameSpeed);
		newWait = newWait < 0 ? 0 : newWait;
		// throttle the graphics loop to cap at a certain fps
		std::this_thread::sleep_for(std::chrono::milliseconds(newWait));

	} //Check if the ESC or Q key had been pressed or if the window had been closed  
	while (!glfwWindowShouldClose(window));

	printf("\n");
	printf("Window has closed. Application will now exit.\n");

	//Close OpenGL window and terminate GLFW  
	glfwDestroyWindow(window);
	//Finalize and clean up GLFW  
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
//GL window initialose
GLFWwindow *				initWindow()
{
	GLFWwindow * window;

	//Set the error callback  
	glfwSetErrorCallback(error_callback);

	printf("Initialising GLFW...\n");
	//Initialize GLFW  
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}
#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	printf("Creating window...\n");
	//Create a window and create its OpenGL context  
	window = glfwCreateWindow(window_size.x, window_size.y, "Test Window", NULL, NULL);
	//If the window couldn't be created  
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	printf("Setting window context...\n");
	//This function makes the context of the specified window current on the calling thread.   
	glfwMakeContextCurrent(window);

	//Sets the key callback  
	glfwSetKeyCallback(window, key_callback);

	printf("Initialising GLEW...\n");
	//Initialize GLEW  
	GLenum err = glewInit();
	//If GLEW hasn't initialized  
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);
	// enable texturineg
	glEnable(GL_TEXTURE_2D);
	// init
	init();


	
	return window;
}

void run_cwk()
{
	glLoop(loop, initWindow());	
}