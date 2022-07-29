/* ========================================================================
   $File: opengl.h
   $Date: 2022-02-12
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: OpenGL
    ========================================================================*/
#pragma once

// TODO: I need some orgnaization, but before that i need some code to work with

#include <stdint.h>

// Fixed Width singed integers
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef void*    PTR;
//typedef intptr_t PTR;

// Fixed Width unsinged integers
typedef uint8_t   U8;
typedef uint16_t  U16;
typedef uint32_t  U32;
typedef uint64_t  U64;

// This is actually not neccecry 
typedef float     F32;
typedef double    F64;

// Any Value but 0 is true
typedef uint8_t   B8;
typedef uint16_t  B16;
typedef uint32_t  B32;
typedef uint64_t  B64;

// Sizes
typedef size_t Size;



// Forward declaration
void print (const glm::vec3 &v);


struct Application
{
	bool shouldClose;
};

struct Monitor
{
	GLFWmonitor*	 monitor;
	S64	xpos;
	S64	ypos;
	S64	width;
	S64	height;
};

typedef unsigned int ShaderProgram;

struct Transformation
{
	glm::mat4 model, view, proj;
};


typedef glm::vec2 MousePosition;

struct Model;


typedef void (*Update)(Model*, float);
typedef void (*Render)(Model*, float);

struct Model
{
	U32  vertexArray;
	U32  premativesCount;
	U32  vertexBuffer;
	U32  elementBuffer;
	U32	 texture;

	glm::vec3        rotation;
	glm::vec3        position;
	Transformation	 transformation;
	ShaderProgram    shaderProgram;

	Update   update;
	Render   render;
};

struct Camera
{
	const char* name;
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	
	float ratio; 
	float angle;
	float nearPlane; 
	float farPlane;
	
	glm::mat4 getView(){
		return glm::lookAt(position, front, up);
	}
	
	glm::mat4 getProj()	{
		return glm::perspective(glm::radians(angle), ratio, nearPlane, farPlane);
	}
	
};


enum ShaderType { VERTEX, FRAGMENT };

template<ShaderType _type>
struct Shader
{
	static inline ShaderType type = _type;
	U32 shader;	
};

typedef Shader<ShaderType::VERTEX>   VertexShader;
typedef Shader<ShaderType::FRAGMENT> FragmentShader;


void updateCameraFront();


bool quad2DScene(ShaderProgram, Model*);

bool sceneFromObj(ShaderProgram, Model*);


void updateScene(Model*, float);
void drawScene(Model*, float);

bool loadShaders(Model*);

bool createShader(const char*, const char*, ShaderProgram*);
template<ShaderType type>
bool loadShader(const char*, Shader<type>*);

U32 createTexture(const char* filename);


static String_View read_entire_file(const char* filename)
{
	FILE *file;
	fopen_s(&file, filename, "rb" ); // remember to read the file as binary or else we're fucked

	if (!file)
	{
		printf("Cannot Open [%s]\n", filename);
		return {0};
	}

	fseek(file, 0, SEEK_END);
	size_t length = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	if (length == 0) return {0};

	
	char* data = (char*)malloc(length + 1);

	auto success = fread((void*)data, length, 1, file);
	if (success < 1) {
		delete[] data;
		data = nullptr;
		goto _return;
	}	

	((char*)data)[length] = 0;

  _return:
	fclose(file);

	
	return {length, data};
	
}

template<typename T> 
struct Array
{
	size_t count;
	size_t occupied;
	T* data;


	// TODO: Bounds check here
	T operator[](size_t i) {return data[i];}
};


template<typename T>
bool init_array(Array<T>* arr, size_t initialCount)
{
	arr->count    = initialCount;
	arr->occupied = 0;
	arr->data     = (T*)malloc(initialCount * sizeof(T));

	return arr->data != nullptr;
	
}

template<typename T>
bool resize_array(Array<T>* arr, size_t newCount)
{
	// TODO: Report 
	if(newCount <= arr->count) return false;
	
	arr->count  = newCount;
	T* temp     = (T*)realloc(arr->data, newCount * sizeof(T));
	
	if (temp == nullptr) return false;
	arr->data = temp;
	return true;
}

template<typename T>
bool insert_array(Array<T>* arr, T item)
{
	if (arr->occupied == arr->count ) {
		// @TODO: report here
		if (! resize_array(arr, arr->count + arr->count / 2)) return false;
	}

	arr->data[arr->occupied] = item;
	arr->occupied +=1;
	return true;
}

template<typename T> 
void free_array(Array<T>* arr)
{

	free(arr->data);
	arr->count = 0;
	arr->occupied = 0;
	arr->data = nullptr;
}


void ImguiCameraController(Camera& camera);


template<typename T>
struct Result
{
	T data;
	bool success;
};

Result<Model> createRectancle(glm::vec3, glm::vec3);





// glEnableVertexArrayAttrib(result.vertexArray, 0);  
// glVertexArrayAttribBinding(result.vertexArray, 0, 0);
// glVertexArrayAttribFormat(result.vertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float));

#define CREATE_VERTEX_ATTRIBUTE(vertexArray, id, count, glType, glNormalized, offset) \
	glEnableVertexArrayAttrib(vertexArray, id);							\
	glVertexArrayAttribBinding(vertexArray, id, 0);						\
	glVertexArrayAttribFormat(vertexArray, id, count, glType, glNormalized, offset); \
