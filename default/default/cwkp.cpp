#include "cwkp.h"
#include "colors.h"
#include <chrono>
#include <thread>

#include <btBulletDynamicsCommon.h>

//Returns random float
inline float		randf()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;
std::vector<btRigidBody*> MovingBits; // so that can get at all bits
std::vector<btRigidBody*> StaticBits; // especially during clean up.

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
	eye_position(0, 5, -20),
	eye_direction,
	ambient_color = glm::vec3(0.1f,0.2f,0.3f);

glm::mat4 
	model, 
	view, 
	projection;

Obj
	sphere,
	sphere2,
	cube;


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

btRigidBody* SetSphere(float size, btTransform T) {
	btCollisionShape* fallshape = new btSphereShape(size);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(T);
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallshape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallshape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setLinearVelocity(btVector3(randf(), randf(), randf()));
	//fallRigidBody->applyCentralImpulse(btVector3(0,1,0));
	fallRigidBody->setRestitution(COE);
	fallRigidBody->setFriction(0.0f);
	fallRigidBody->setRollingFriction(0.0f);
	fallRigidBody->setDamping(0.0f, 0.0f);
	dynamicsWorld->addRigidBody(fallRigidBody);
	return fallRigidBody;
	return nullptr;
}

btRigidBody* SetCube(float size, btTransform T) {
	btCollisionShape* fallshape = new btBoxShape(size);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(T);
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallshape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallshape, fallInertia);
	btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
	fallRigidBody->setLinearVelocity(btVector3(randf(), randf(), randf()));
	//fallRigidBody->applyCentralImpulse(btVector3(0,1,0));
	fallRigidBody->setRestitution(COE);
	fallRigidBody->setFriction(0.0f);
	fallRigidBody->setRollingFriction(0.0f);
	fallRigidBody->setDamping(0.0f, 0.0f);
	dynamicsWorld->addRigidBody(fallRigidBody);
	return fallRigidBody;
	return nullptr;
}


glm::vec3 bullet_step(int i) {
	btTransform trans;
	btRigidBody* moveRigidBody;
	int n = MovingBits.size();
	moveRigidBody = MovingBits[i];
	dynamicsWorld->stepSimulation(1 / 60.f / n, 10);
	moveRigidBody->getMotionState()->getWorldTransform(trans);
	return glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
	return glm::vec3();
}

void bullet_close() {
  /*
   * This is very minimal and relies on OS to tidy up.
   */
  btRigidBody* moveRigidBody;
  moveRigidBody = MovingBits[0];
  dynamicsWorld->removeRigidBody(moveRigidBody);
  delete moveRigidBody->getMotionState();
  delete moveRigidBody;
  delete dynamicsWorld;
  delete solver;
  delete collisionConfiguration;
  delete dispatcher;
  delete broadphase;
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

	//// BULLET UPDATE

	sphere.pos = bullet_step(0);
	sphere2.pos = bullet_step(1);
	cube.pos = bullet_step(2);

	//// DRAW OBJECTS

	sphere.draw(0, &model_mat_handle, &tex_handle, &normmap_handle, &heightmap_handle);
	sphere2.draw(0, &model_mat_handle, &tex_handle, &normmap_handle, &heightmap_handle);
	cube.draw(0, &model_mat_handle, &tex_handle, &normmap_handle, &heightmap_handle);	

	//printf("%f  %f  %f\n", sphere.pos.x,sphere.pos.y,sphere.pos.z);

	//eye_direction = sphere.pos;
}

void init_bullet()
{
	/*
	* set up world
	*/
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0., GRAVITY, 0));
	/*
	* Set up bottom
	*/
	btCollisionShape* bottomShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	btDefaultMotionState* bottomMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -5, 0)));
	btRigidBody::btRigidBodyConstructionInfo bottomRigidBodyCI(0, bottomMotionState, bottomShape, btVector3(0, 0, 0));
	btRigidBody* bottomRigidBody = new btRigidBody(bottomRigidBodyCI);
	bottomRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(bottomRigidBody);
	/*
	* Set up top
	*/
	btCollisionShape* topShape = new btStaticPlaneShape(btVector3(0, -1, 0), 1);
	btDefaultMotionState* topMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 5, 0)));
	btRigidBody::btRigidBodyConstructionInfo topRigidBodyCI(0, topMotionState, topShape, btVector3(0, 0, 0));
	btRigidBody* topRigidBody = new btRigidBody(topRigidBodyCI);
	topRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(topRigidBody);

	/*
	* Set up left
	*/
	btCollisionShape* leftShape = new btStaticPlaneShape(btVector3(-1, 0, 0), 1);
	btDefaultMotionState* leftMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(5, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo leftRigidBodyCI(0, leftMotionState, leftShape, btVector3(0, 0, 0));
	btRigidBody* leftRigidBody = new btRigidBody(leftRigidBodyCI);
	leftRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(leftRigidBody);

	/*
	* Set up right
	*/
	btCollisionShape* rightShape = new btStaticPlaneShape(btVector3(1, 0, 0), 1);
	btDefaultMotionState* rightMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-5, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo rightRigidBodyCI(0, rightMotionState, rightShape, btVector3(0, 0, 0));
	btRigidBody* rightRigidBody = new btRigidBody(rightRigidBodyCI);
	rightRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(rightRigidBody);

	/*
	* Set up back
	*/
	btCollisionShape* backShape = new btStaticPlaneShape(btVector3(0, 0, -1), 1);
	btDefaultMotionState* backMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 5)));
	btRigidBody::btRigidBodyConstructionInfo backRigidBodyCI(0, backMotionState, backShape, btVector3(0, 0, 0));
	btRigidBody* backRigidBody = new btRigidBody(backRigidBodyCI);
	backRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(backRigidBody);

	/*
	* Set up front
	*/
	btCollisionShape* frontShape = new btStaticPlaneShape(btVector3(0, 0, 1), 1);
	btDefaultMotionState* frontMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -5)));
	btRigidBody::btRigidBodyConstructionInfo frontRigidBodyCI(0, frontMotionState, frontShape, btVector3(0, 0, 0));
	btRigidBody* frontRigidBody = new btRigidBody(frontRigidBodyCI);
	frontRigidBody->setRestitution(COE);
	dynamicsWorld->addRigidBody(frontRigidBody);


	/*
	* Set up sphere 0
	*/
	MovingBits.push_back(SetSphere(1, btTransform(btQuaternion(0,0,0,1),btVector3(sphere.pos.x,sphere.pos.y,sphere.pos.z))));
	MovingBits.push_back(SetSphere(1, btTransform(btQuaternion(0,0,0,1),btVector3(sphere2.pos.x,sphere2.pos.y,sphere2.pos.z))));
	MovingBits.push_back(SetCube(1, btTransform(btQuaternion(0,0,0,1),btVector3(cube.pos.x,cube.pos.y,cube.pos.z))));
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
	programs.add_program("shaders/phong.vert", "shaders/phong.frag");
	programs.load_program(0);



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

	sphere = Obj(
		"","","",
		//"textures/moss_color.jpg",
		//"textures/moss_norm.jpg",
		//"textures/moss_height.jpg",
		pack_object(&v, GEN_DEFAULT, GREY),
		glm::vec3(1,0,0),
		glm::vec3(1, 0, 0),
		glm::radians(90.0f),
		glm::vec3(1, 1, 1)
	);

	sphere2 = sphere;
	sphere2.pos = glm::vec3(0,0,0);

	v = generate_cube();

	cube = Obj(
		"","","",
		pack_object(&v, GEN_DEFAULT, RED),
		glm::vec3(-1,0,0),
		glm::vec3(1,0,0),
		glm::radians(0.0f),
		glm::vec3(1,1,1)
	);

	init_bullet();
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

	bullet_close();

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