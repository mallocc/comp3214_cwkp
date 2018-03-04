#include "cwkp.h"
#include "colors.h"
#include <chrono>
#include <thread>

GLSLProgramManager programs;
VarHandleManager handles;

VarHandle model_mat_handle, view_mat_handle, proj_mat_handle, tex_handle, normmap_handle;

glm::vec2
window_size(1280, 720);
glm::vec3
eye_position(0,0,5),
eye_direction;

glm::mat4 model, view, projection;

Obj sphere;


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
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		}
	}
}


void loop()
{
	//// LOAD GLOBAL HANDLES
	view_mat_handle.load();
	proj_mat_handle.load();

	tex_handle.load();
	normmap_handle.load();	


	//// DRAW OBJECTS
	sphere.draw(
		0,
		&model_mat_handle,
		&tex_handle,
		&normmap_handle);
}

//Initilise custom objects
void			init()
{
	//// CREATE GLSL PROGAMS
	printf("\n");
	printf("Initialising GLSL programs...\n");
	programs.add_program("shaders/basic.vert", "shaders/basic.frag");



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



	//// CREATE OBJECTS
	printf("\n");
	printf("Initialising objects...\n");
	// create sphere data for screen A, B and D
	std::vector<glm::vec3> v = generate_sphere(200, 200);
	std::vector<Vertex> o = pack_object(&v, GEN_DEFAULT, GREY);

	sphere = Obj("","",
		o,
		glm::vec3(),
		glm::vec3(1, 0, 0),
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
	// enable texturing
	glEnable(GL_TEXTURE_2D);

	// init
	init();

	return window;
}

void run_cwk()
{
	glLoop(loop, initWindow());	
}