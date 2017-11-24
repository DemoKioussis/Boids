#include "WindowManager.h"
#include "Shader.h"
#include "Camera.h"
#include "InputManager.h"
#include <iostream>
#include "TimeSystem.h"
#include "Flock.h"
#include <glm/glm.hpp>


// Number of flocks in simulation
#define NUM_FLOCKS 100

// Number of boids in a flock
#define BOIDS_PER_FLOCK 1024

// Boids will randomly position themselve's somewhere in a sphere
// of this radius
#define START_RADIUS 0

// This is how large the sphere that contains the boids is. Boids can pass this sphere 
// but will be influenced towards the center
#define FIELD_RADIUS 200.0f

// This is how strongly the boids are push back once they hit the endge of the sphere
#define FIELD_STRENGTH 0.01f

// If true, each flock is confined to its own sphere
#define SEPARATE_FLOCKS false

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

const int framerate = 120;



WindowManager* windowManager;
Camera* camera;

bool hasStarted = false;
vector<Flock*> flocks(0);





int main()
{

#pragma region initRegion
	TimeSystem::begin();

	windowManager = new WindowManager(SCR_WIDTH, SCR_HEIGHT);
	windowManager->setFramerate(framerate);
	windowManager->setBackgroundColor(glm::vec4(0.5,0.5,0.5,1));
	Shader shaderProg("ShaderSources/vert.vs", "ShaderSources/frag.fs");

	camera = new Camera(windowManager);
	camera->translate(glm::vec3(0, 0, -FIELD_RADIUS*2));

	InputManager::setWindow(windowManager);
	InputManager::setCamera(camera);
	InputManager::initialize();

	RenderEngine::NumOfCycles = NUM_FLOCKS;

#pragma endregion

	glm::mat4 projection;
	glm::vec3 lightDir(1, 1, 1);
	GLuint projLoc = glGetUniformLocation(shaderProg.ID, "projection");
	GLuint viewLoc = glGetUniformLocation(shaderProg.ID, "view");
	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);
	RenderEngine::setShader(&shaderProg);
	shaderProg.setMat4(projLoc, projection);
	shaderProg.setVec3("ambientLight", glm::vec3(0.5f, 1.0f, 1.0f));

	float rotateSpeed = 1;

#pragma region colors
	std::vector<glm::vec3> colors(0);
	colors.push_back(glm::vec3(0, 0, 0));
	colors.push_back(glm::vec3(0, 0, 1));
	colors.push_back(glm::vec3(0, 1, 0));
	colors.push_back(glm::vec3(0, 1, 1));
	colors.push_back(glm::vec3(1, 0, 0));
	colors.push_back(glm::vec3(1, 0, 1));
	colors.push_back(glm::vec3(1, 1, 0));
	colors.push_back(glm::vec3(1, 1, 1));

	colors.push_back(glm::vec3(0, 0.5, 0.8));
	colors.push_back(glm::vec3(0, 0.8, 0.5));
	colors.push_back(glm::vec3(0.5, 0, 0.8));
	colors.push_back(glm::vec3(0.5, 0.8, 0));
	colors.push_back(glm::vec3(0.8, 0.5, 0));
	colors.push_back(glm::vec3(0.8, 0, 0.5));

	colors.push_back(glm::vec3(0.3, 0.5, 0.8));
	colors.push_back(glm::vec3(0.3, 0.8, 0.5));
	colors.push_back(glm::vec3(0.5, 0.3, 0.8));
	colors.push_back(glm::vec3(0.5, 0.8, 0.3));
	colors.push_back(glm::vec3(0.8, 0.5, 0.3));
	colors.push_back(glm::vec3(0.8, 0.3, 0.5));


	colors.push_back(glm::vec3(0, 0, 0.5));
	colors.push_back(glm::vec3(0, 0.5, 0));
	colors.push_back(glm::vec3(0, 0.5, 0.5));
	colors.push_back(glm::vec3(0.5, 0, 0));
	colors.push_back(glm::vec3(0.5, 0, 0.5));
	colors.push_back(glm::vec3(0.5, 0.5, 0));
	colors.push_back(glm::vec3(0.5, 0.5, 0.5));
#pragma endregion

	for (int i = colors.size();i < NUM_FLOCKS;i++) {
		glm::vec3 randColor((float(rand()) / float(RAND_MAX)) - 0.5f, (float(rand()) / float(RAND_MAX)) - 0.5f, (float(rand()) / float(RAND_MAX)) - 0.5f);
		colors.push_back(randColor);
	}




	
	//get all platforms (drivers)
	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);
	if (all_platforms.size() == 0) {
		std::cout << " No platforms found. Check OpenCL installation!\n";
		//exit(1);
	}
	for (int i = 0; i < all_platforms.size();i++) {
		std::cout << "Platform found: " << all_platforms[i].getInfo<CL_PLATFORM_NAME>() << "\n";
	}

	cl::Platform default_platform = all_platforms[0];
	std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";
	cl_context_properties props[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)glfwGetWGLContext(windowManager->window),
		CL_WGL_HDC_KHR, (cl_context_properties)GetDC(glfwGetWin32Window(windowManager->window)),
		CL_CONTEXT_PLATFORM, (cl_context_properties)default_platform(),
		0
	};

	//get default device of the default platform
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	if (all_devices.size() == 0) {
		std::cout << " No devices found. Check OpenCL installation!\n";
		//exit(1);
	}
	for (int i = 0; i < all_devices.size();i++) {
		std::cout << "Devide found: " << all_devices[i].getInfo<CL_DEVICE_NAME>() << "\n";
	}
	cl::Device default_device = all_devices[0];
	std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";
	std::cout << "Max WG Size: " << default_device.getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << "\n";
	std::cout << "Max WG Dims: " << default_device.getInfo<CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS>() << "\n";

	std::cout << "global mem size: " << default_device.getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() / (1024 * 1024) << " mb\n";
	std::cout << "global mem cache " << default_device.getInfo<CL_DEVICE_GLOBAL_MEM_CACHE_SIZE>() / 1024 << " kb\n";

	std::cout << "local mem size: " << default_device.getInfo<CL_DEVICE_LOCAL_MEM_SIZE>() / 1024 << " kb\n";
	std::cout << "compute units: " << default_device.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << "\n";
	std::cout << "Extensions: " << default_device.getInfo<CL_DEVICE_EXTENSIONS>() << "\n";



	cl::Context context = cl::Context(default_device, props);
	cl::Program::Sources sources;

	std::string codePath = "CLKernelSource/boidKernel.cl";
	std::string clCode;
	std::ifstream clFile;
	// ensure ifstream objects can throw exceptions:
	clFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		clFile.open(codePath);
		std::stringstream clStream;
		// read file's buffer contents into streams
		clStream << clFile.rdbuf();
		// close file handlers
		clFile.close();
		// convert stream into string
		clCode = clStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::KERNEL::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	sources.push_back({ clCode.c_str(),clCode.length() });


	cl::Program program(context, sources);
	if (program.build({ default_device }) != CL_SUCCESS) {
		std::cout << " Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		//exit(1);
	}

	glm::vec3 origin(0, 0, 0);


	shaderProg.setVec3("lightDirection", glm::vec3(-1, 0, 0));
	size_t num_flocks = NUM_FLOCKS;


	// create buffers on the device
	cl::CommandQueue queue(context, default_device);
	
	std::cout << "\n\nGenerating Boids ... ";
	int j = 1;
	for (int i = 0; i < NUM_FLOCKS;i++) {

		Flock * f;
		if (SEPARATE_FLOCKS) {
			glm::vec3 startPos(0, 0, i * 2 * FIELD_RADIUS);
			f = new Flock(BOIDS_PER_FLOCK, startPos, START_RADIUS, FIELD_RADIUS, FIELD_STRENGTH, colors[i],&queue,&context, &program);
		}
		else
			f = new Flock(BOIDS_PER_FLOCK, origin, START_RADIUS, FIELD_RADIUS, FIELD_STRENGTH, colors[i], &queue, &context, &program);
		flocks.push_back(f);
	}
	std::cout << "Done" << std::endl;
	std::cout << NUM_FLOCKS << " flocks\n" << BOIDS_PER_FLOCK << " boids per flock\n" << BOIDS_PER_FLOCK*NUM_FLOCKS << " Total boids\n\n";

#pragma region mainLoop
	while (windowManager->windowHasClosed())
	{
		shaderProg.setVec3("lightDirection", camera->Front);

		glm::mat4 view = camera->GetViewMatrix();
		shaderProg.setMat4(viewLoc, view);

		TimeSystem::update();
		if (windowManager->frameTick()) {
			InputManager::processInput();

			if (!InputManager::pause) {
				for (int i = 0; i < flocks.size();i++) {
					flocks[i]->step();
				}
			}
		
		}
	}


#pragma endregion



	glfwTerminate();
	delete(windowManager);
	return 0;
}
