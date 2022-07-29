
/* ========================================================================
   $File: opengl.cpp
   $Date: 2022-02-10
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: OpenGL
    ========================================================================
*/


// TODO: TEMP
// #include <imgui.h>
// #include <backends\imgui_impl_glfw.h>
// #include <backends\imgui_impl_opengl3.h>
//



#define GLFW_INCLUDE_NONE
//#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
//#include <GLFW/glfw3native.h>

#include <GLAD/glad.h>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <GLAD/glad.c>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SV_IMPLEMENTATION
#include "./sv.h"


#include "opengl.h"
#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#pragma comment(lib, "glfw3_mt")
#pragma comment(lib, "User32")
#pragma comment(lib, "gdi32")
#pragma comment(lib, "Shell32")

#pragma warning(disable: 4101)
#pragma warning(disable: 4533)



//static constexpr int   WindowWidth  = 1920;
//static constexpr int   WindowHeight = 1080;
static constexpr int   WindowWidth  = 1600;
static constexpr int   WindowHeight = 900;
static constexpr float WindowRatio  = (float)WindowWidth/(float)WindowHeight;


static Camera mainCamera = {
	"Main Camera",
	glm::vec3(0.0f, 0.0f, 500.0f), // POS
	glm::vec3(0.0f, 0.0f, 1.0f),   // Front
	glm::vec3(0.0f, 1.0f, 0.0f),   // UP
    WindowRatio,                   // Ratio
	45.f,                          // Angle TODO: More info
	0.1f,                          // Near Plane
	1000.0f                        // Far Plane
};

static glm::vec3 cameraPos		= glm::vec3(0.0f, 0.0f, 500.0f);
static glm::vec3 worldOrigin	= glm::vec3(0.0f, 0.0f,  -1.0f);
static glm::vec3 cameraFront	= worldOrigin; // At Start make the camera look to origin
static glm::vec3 cameraUp		= glm::vec3(0.0f, 1.0f, 0.0f);
static glm::mat4 vvcamera			=  glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
//glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -500.0f));
static glm::mat4 proj			= glm::perspective(glm::radians(45.0f),
										   (float)WindowWidth / (float)WindowHeight,
										   0.1f, 1000.0f);

static float globalDeltaTime = 0.0f;

static float yaw   = -90.0f; // yaw = 0.0, means look to the right, the more minus the more to the left
static float pitch =   0.0f; // 
static float fov   =  45.0f; // 45.f degrees 

static float lastX = 0.0f, lastY = 0.0f;

// @REMOVE(Husam): Remove this if you want to publish this

// Forward declarations
//void configImguiViewPorts(const ImGuiIO& io);
void ImguiModelController(const char*, Model* scene);
//

void GLAPIENTRY
opengl_error_callback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
	(void)userParam;
	(void)length;
	(void)id;
	(void)source;
	if (GL_DEBUG_SEVERITY_NOTIFICATION == severity) return;
	fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
			 ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
			 type, severity, message );

}

void glfw_error_callback(int error, const char* description)
{
    printf("Error[%d]: %s\n", error, description);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	(void)window;
    glViewport(0, 0, width, height);
	// if we want to make the screen "responsive" we have to do transformations here? 
}  

//glm::vec3 move{};
//int hor, ver = 0;

static MousePosition mousePos = glm::vec2(0.0f, 0.0f);
static bool rightMouseButton = false;

static double mouseClickX = 0.f;
static double mouseClickY = 0.f;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	(void)window;
	(void)mods;
	(void)action;

	auto isKeyPressed = [&](int desiredKey){
		return button == desiredKey &&  (action == GLFW_PRESS || action == GLFW_REPEAT);
	};
	
    if (isKeyPressed(GLFW_MOUSE_BUTTON_RIGHT))
	{
		rightMouseButton = true;
		//glfwGetCursorPos(window, &mouseClickX, &mouseClickY);
	}
	if (isKeyPressed(GLFW_MOUSE_BUTTON_RIGHT))
		rightMouseButton = false;
}


static bool firstMouse = true;
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	(void)window;
	mousePos = glm::vec2((float)xpos, (float)ypos);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mod)
{
	(void)window;
	(void)mod;
	(void)scancode;

	auto isKeyPressed = [&](int desiredKey){
		return key == desiredKey &&  (action == GLFW_PRESS || action == GLFW_REPEAT);
	};
	
	if(isKeyPressed(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

	const float cameraSpeed = globalDeltaTime * 1000.f ; // adjust accordingly
	const auto  rightVec    = glm::normalize(glm::cross(mainCamera.front, mainCamera.up)); 


	if (isKeyPressed(GLFW_KEY_W))
        mainCamera.position += mainCamera.front * cameraSpeed;
    if (isKeyPressed(GLFW_KEY_S))
		mainCamera.position -= mainCamera.front * cameraSpeed;
	
	if (isKeyPressed(GLFW_KEY_A))
		mainCamera.position -= rightVec * cameraSpeed;
	if (isKeyPressed(GLFW_KEY_D))
		mainCamera.position += rightVec * cameraSpeed;
	
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	(void)window;
	(void)yoffset;
	(void)xoffset;
}

// NOTE: One thing good about c++ is this right here (function over\loading )
void print (const glm::vec3 &v)  
{
	printf("{ x = %9.6f, y = %9.6f, z = %9.6f}\n", v.x, v.y, v.z);
}

void print (const glm::mat4 &r)  
{  
	for (int i=0; i<4; i++)
	{  
        for (int j=0; j<4; j++)
		{
			printf("%9.6f, ", r[i][j]) ;  
        }
		printf("\n");
    }
	printf("\n");
}


static inline
void PrintLastWin32Error()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) {
		printf("No Errors to be found\n");
        return;
    }
    
	
    LPSTR messageBuffer = nullptr;
	
    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us
	// (because we don't yet know how long the message string will be).
	
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
								 FORMAT_MESSAGE_FROM_SYSTEM |
								 FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL,
								 errorMessageID,
								 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    
    printf("Error Message: %s\n", messageBuffer);
    LocalFree(messageBuffer);
}


// @NOTE(Husam): OpenGL assumes that we are drawing the vertecis counter-clock wise, unless told otherwise.
int main(void)
{

	// printf("sizeof int = %lld\n", sizeof(int));
	// printf("sizeof S64 = %lld\n", sizeof(S64));
	// return 0;
	glfwSetErrorCallback(glfw_error_callback);	
    /* Initialize the library */
    if (!glfwInit())
        return -1;


	Application app = {};
	
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	
	glfwWindowHint(GLFW_FOCUSED, GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_DECORATED, GL_TRUE);


	if(!GL_ARB_direct_state_access)
	{
		fprintf(stderr, "GL_ARB_direct_state_access is false");
		glfwTerminate();
		return -1;
	}
	
    /* Create a windowed mode window and its OpenGL context */

	Monitor mainMonitor;
	mainMonitor.monitor = glfwGetPrimaryMonitor();
	glfwGetMonitorWorkarea(mainMonitor.monitor,
						   (int*)&mainMonitor.xpos , (int*)&mainMonitor.ypos,
						   (int*)&mainMonitor.width, (int*)&mainMonitor.height);

	
    GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "OpenGL Version", NULL, NULL);
	if (window == NULL)
	{
		printf("Could not create window\n");
		glfwTerminate();
		return -1;
	}
	
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
	glfwShowWindow(window);

	auto centerWidth  = mainMonitor.width / 2 - WindowWidth /2;
	auto centerHeight = mainMonitor.height / 2 - WindowHeight /2;
	glfwSetWindowPos(window, (int)centerWidth, (int)centerHeight);
	
	// GLFW Callbacks
	//glfwSetErrorCallback(glfw_error_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_pos_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Error: Faild to inialize GLAD");
		return -1;
	}


	glEnable              ( GL_DEBUG_OUTPUT );
	glDebugMessageCallback( opengl_error_callback, 0 );
	glViewport(0, 0, WindowWidth, WindowHeight);

	// constexpr unsigned int  numberOfScenes = 2;
	// Model* scenes[numberOfScenes];
	// auto cubeResult = createRectancle(glm::vec3(0.0f), glm::vec3(100.0f)); // quad2DScene(&scene1);
    // auto cube2Result = createRectancle(glm::vec3(316.f, 120.0f, 8.0f), glm::vec3(100.0f)); // quad2DScene(&scene1);
	
	// if(!cubeResult.success || !cube2Result.success) goto TERMINATE;
	// Model& cube = cubeResult.data;
	// Model* cube2 = &cube2Result.data;

	// cube.update = updateScene;
	// cube.render = drawScene;

	// cube2->update = updateScene;
	// cube2->render = drawScene;
	
	
	// // TEMP
	// scenes[0] = &cubeResult.data;
	// scenes[1] = &cube2Result.data;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	
	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	
	// float currentTime   = (float)glfwGetTime();
	// float lastFrameTime = currentTime;
	// float deltaTime     = 0.0f;

	

    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  
	// ImGui::StyleColorsDark();
	// configImguiViewPorts(io);
	
	
	// constexpr const char* glsl_version = "#version 460 core";
	// ImGui_ImplGlfw_InitForOpenGL(window, true);
	// ImGui_ImplOpenGL3_Init(glsl_version);



	// ImVec4 clear_color = ImVec4(0.2f, 0.3f, 0.3f, 1.00f);

	/*
	// Edit bools storing our window open/close state
	ImGui::Checkbox("Demo Window", &show_demo_window);
	ImGui::Checkbox("Another Window", &show_another_window);
	*/


	/* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

		// currentTime = (float)glfwGetTime();
		// deltaTime = currentTime  -  lastFrameTime;
		// lastFrameTime = currentTime;

		// glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();
	
		//ImguiCameraController(mainCamera);
		//ImguiModelController("Cube 1", &cubeResult.data);
		//ImguiModelController("Cube 2", &cube2Result.data);
		
		// {
		// 	// Create a window called "Hello, world!" and append into it.
        //     ImGui::Begin("Info!");
			
		// 	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
		// 				1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			
        //     // Edit 3 floats representing a color
		// 	ImGui::ColorEdit3("clear color", (float*)&clear_color);
          
        //     ImGui::End();
        // }

		
		/* Update here */
		// for (unsigned int i = 0; i < numberOfScenes; i++)
		// {
		// 	if (scenes[i]->update != nullptr)
		// 	{
		// 		scenes[i]->update(scenes[i], deltaTime);
		// 	}			
		// }
		
        /* Render here */
		// for (unsigned int i = 0; i < numberOfScenes; i++)
		// {
		// 	if (scenes[i]->render != nullptr)
		// 	{
		// 		scenes[i]->render(scenes[i], deltaTime);
		// 	}			
		// }

		
		// ImGui::EndFrame();
		// ImGui::Render();
		// ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
        /* Swap front and back buffers */

		glfwSwapBuffers(window);
    }

    // Cleanup
    // ImGui_ImplOpenGL3_Shutdown();
    // ImGui_ImplGlfw_Shutdown();
    // ImGui::DestroyContext();
	
	// TERMINATE:
	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void updateScene(Model* scene, float deltaTime)
{
	//float cameraSpeed = 2.5f * deltaTime;
	globalDeltaTime = deltaTime;


	//printf("[UpdateScene] scene->position = %p\n", &scene->position);
	scene->transformation.model = glm::translate(glm::mat4(1.f), scene->position);
	scene->transformation.model = glm::rotate(scene->transformation.model,
											  glm::radians(scene->rotation.x),
											  glm::vec3(1.f, 0.f, 0.f));
	
	scene->transformation.model = glm::rotate(scene->transformation.model,
											  glm::radians(scene->rotation.y),
											  glm::vec3(0.f, 1.f, 0.f));

	scene->transformation.model = glm::rotate(scene->transformation.model,
											  glm::radians(scene->rotation.z),
											  glm::vec3(0.f, 0.f, 1.f));
	proj = mainCamera.getProj(); //glm::perspective(glm::radians(fov),
	//(float)WindowWidth / (float)WindowHeight,
	//						  0.1f, 1000.0f);

	
	vvcamera =  mainCamera.getView();//glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	//	scene->transformation.model = glm::rotate(scene->transformation.model, // Rotate on X axis 
	//											  glm::radians(2.0f ) * deltaTime  ,
	//											  glm::vec3(1.0f, 0.0f, 0.0f));
   
}

void drawScene(Model* scene, float deltaTime)
{
	(void)deltaTime;
	//int obj_textureLocation     = glGetUniformLocation(obj.shaderProgram, "myTexture");
	int u_color					= glGetUniformLocation(scene->shaderProgram, "ourColor");
	int u_projection			= glGetUniformLocation(scene->shaderProgram, "projection");
	int u_view					= glGetUniformLocation(scene->shaderProgram, "view");
	int u_model					= glGetUniformLocation(scene->shaderProgram, "model");
	int cube_textureLocation    = glGetUniformLocation(scene->shaderProgram, "myTexture");
	
	
	float timeValue = (float)glfwGetTime();
	float greenValue = ((float)sin(timeValue) / 2.0f) + 0.5f;


	glUseProgram(scene->shaderProgram);

	// Send our color to the shader
	glUniform3f(u_color, greenValue, greenValue, greenValue);
	
	glBindTextureUnit(0, scene->texture);
	glUniform1i(cube_textureLocation, 0);


	glBindVertexArray(scene->vertexArray);
	
	
	glUniformMatrix4fv(u_model      , 1, GL_FALSE, glm::value_ptr(scene->transformation.model));
	glUniformMatrix4fv(u_view       , 1, GL_FALSE, glm::value_ptr(vvcamera));
	//glUniformMatrix4fv(u_view       , 1, GL_FALSE, glm::value_ptr(scene->transformation.view));
	glUniformMatrix4fv(u_projection , 1, GL_FALSE, glm::value_ptr(proj));
	//glUniformMatrix4fv(u_projection , 1, GL_FALSE, glm::value_ptr(scene->transformation.proj));


	glDrawElements(GL_TRIANGLES, scene->premativesCount, GL_UNSIGNED_INT, 0);
	
	
}


Result<Model> createRectancle(glm::vec3 position, glm::vec3 dimensions)
{
	float hWidth	= dimensions.x / 2.0f;
	float hHeight	= dimensions.y / 2.0f;
	float hDepth	= dimensions.z / 2.0f;

	// We can optimize the size of this array be removing duplicates
	float vertices[] = {

		// Back Face
		-hWidth, -hHeight,  -hDepth,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
		-hWidth,  hHeight,  -hDepth,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
		 hWidth,  hHeight,  -hDepth,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
		 hWidth, -hHeight,  -hDepth,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,

		// Front Face
		-hWidth, -hHeight,  hDepth,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		-hWidth,  hHeight,  hDepth,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
		 hWidth,  hHeight,  hDepth,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
		 hWidth, -hHeight,  hDepth,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,


		 // Right Face
		 hWidth, -hHeight,   hDepth,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		 hWidth, -hHeight,  -hDepth,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,		 
		 hWidth,  hHeight,  -hDepth,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
		 hWidth,  hHeight,   hDepth,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,


		 // Left Face
		 -hWidth, -hHeight,  -hDepth,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
		 -hWidth, -hHeight,   hDepth,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		 -hWidth,  hHeight,   hDepth,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
		 -hWidth,  hHeight,  -hDepth,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,


		 // Upper Face
		 -hWidth,  hHeight,   hDepth,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
		  hWidth,  hHeight,   hDepth,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		  hWidth,  hHeight,  -hDepth,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
		 -hWidth,  hHeight,  -hDepth,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
		 
		 // Buttom Face
		 -hWidth,  -hHeight,  -hDepth,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
		  hWidth,  -hHeight,  -hDepth,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		  hWidth,  -hHeight,   hDepth,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
		 -hWidth,  -hHeight,   hDepth,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
		 
    };

	unsigned int indices[] = {  
		0, 1, 2,   
		0, 2, 3,   

		4, 6, 5,
		4, 7, 6,

		8, 9, 10,
		8, 10, 11,
		
		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23
	};  

	Model result = {};
	// Gen VertexArray Buffer
	// Gen Vertex Buffer & ElementBufferObject
	glCreateVertexArrays(1, &result.vertexArray);  
	glCreateBuffers(1, &result.vertexBuffer); 
	glCreateBuffers(1, &result.elementBuffer);


	glNamedBufferData(result.vertexBuffer,  sizeof(vertices), vertices, GL_STATIC_DRAW);
	glNamedBufferData(result.elementBuffer, sizeof(indices) , indices , GL_STATIC_DRAW);

	CREATE_VERTEX_ATTRIBUTE(result.vertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float));
	CREATE_VERTEX_ATTRIBUTE(result.vertexArray, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
	CREATE_VERTEX_ATTRIBUTE(result.vertexArray, 2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float));
	
	glVertexArrayVertexBuffer(result.vertexArray, 0, result.vertexBuffer, 0, (3 + 2 + 3)   * sizeof(float));
	glVertexArrayElementBuffer(result.vertexArray, result.elementBuffer);


	// Calling this function 2 times will result in creating dublicate textures and shaders
	result.texture = createTexture("c:/trials/twelf/data/images/hamster.png");
	bool success   = createShader("c:/trials/twelf/data/shaders/vertex.glsl",
								  "c:/trials/twelf/data/shaders/fragment.glsl",
								  &result.shaderProgram);
	
	if(!success) return {{}, false};
	
	result.premativesCount		= sizeof(indices)/sizeof(indices[0]);
	result.position				= position;
	result.rotation				= glm::vec3(0.f);
	result.transformation.model = glm::mat4(1.f);
	result.transformation.view  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -500.0f));
	result.transformation.proj  = glm::perspective(glm::radians(45.0f),
												   (float)WindowWidth / (float)WindowHeight,
												   0.1f, 1000.0f);
	
	return {result, true};
}





void ImguiCameraController(Camera& camera)
{
	// ImGui::Begin(camera.name);
			

	// ImGui::SliderFloat3("Camera Posotion", glm::value_ptr(camera.position), -500.0f, 1000.0f);
	// ImGui::SliderFloat3("Camera Front",    glm::value_ptr(camera.front),    -500.0f,  500.0f);
	// ImGui::SliderFloat3("Camera Up",       glm::value_ptr(camera.up),       -1.0f,      1.0f);

	// ImGui::SliderFloat("Ratio",   &camera.ratio, 1.33f, 2.59f);
	// ImGui::SliderFloat("Angle",   &camera.angle, 0.0f, 90.0f);
	// ImGui::SliderFloat2("Planes", &camera.nearPlane, -500.0f, 1500.0f);
	// ImGui::End();
}




inline void ImguiModelController(const char* name, Model* scene)
{
	// ImGui::Begin(name);
	// ImGui::SliderFloat3("Model Position", glm::value_ptr(scene->position), -500.0f, 1000.0f);
	// ImGui::SliderFloat3("Model Rotation", glm::value_ptr(scene->rotation), -180.0f, 180.0f);
	// ImGui::End();
}



#include "openglShaders.cpp"
