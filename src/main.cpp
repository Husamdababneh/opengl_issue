





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


	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	
	glFrontFace(GL_CW);
	glEnable(GL_DEPTH_TEST);
	

	/* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

	
        /* Swap front and back buffers */

		glfwSwapBuffers(window);
    }

	// TERMINATE:
	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

