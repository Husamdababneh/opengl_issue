/* ========================================================================
   $File: openglShaders.cpp
   $Date: 2022-07-11
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: OpenGL Shaders
   ========================================================================*/

template<ShaderType type>
bool loadShader(const char* filepath, Shader<type>* shader)
{
	auto file = read_entire_file(filepath);

	if (file.data == nullptr) return false;
	const char* source = file.data;
	if constexpr (type == ShaderType::VERTEX){
		shader->shader = glCreateShader(GL_VERTEX_SHADER);
	}
	else if constexpr (type == ShaderType::FRAGMENT){
		shader->shader = glCreateShader(GL_FRAGMENT_SHADER);
	}
	
	glShaderSource(shader->shader, 1, &source, NULL);
	glCompileShader(shader->shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader->shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader->shader, 512, NULL, infoLog);
		printf("ERROR: Couldn't compile vertex shader [%s]\n", filepath);
		printf("%s\n", infoLog);
		success = false;
	}


	delete[] source;
	return success ? true : false;;
}





bool createShader(const char* vertexShaderFilePath, const char* fragmentShaderFilePath, ShaderProgram* program)
{
	VertexShader vertex;
	FragmentShader fragment;

	if (!loadShader(vertexShaderFilePath, &vertex))
	{
		glDeleteShader(vertex.shader);
		return false;
	}
	
	if (!loadShader(fragmentShaderFilePath, &fragment))
	{
		glDeleteShader(vertex.shader);
		return false;
	}

	*program = glCreateProgram();
	
	glAttachShader(*program, vertex.shader);
	glAttachShader(*program, fragment.shader);
	glLinkProgram(*program);

	glDeleteShader(vertex.shader);
	glDeleteShader(fragment.shader);
	return true;
}




bool sceneFromObj(ShaderProgram shaderProgram, Model* result)
{

	fastObjMesh* mesh = fast_obj_read("C:\\trials\\twelf\\data\\models\\cube.obj");


	glCreateVertexArrays(1, &result->vertexArray);  
	glCreateBuffers(1, &result->vertexBuffer); 
	glCreateBuffers(1, &result->elementBuffer);



	glNamedBufferData(result->vertexBuffer,
					  mesh->position_count * 3  * sizeof(float),
					  mesh->positions + 3,
					  GL_STATIC_DRAW);
	
	unsigned int* indices = new unsigned int[mesh->index_count];

	for(unsigned int  i = 0; i < mesh->index_count; i++)
	{		
		indices[i] = mesh->indices[i].p - 1;
	}

	glNamedBufferData(result->elementBuffer,
					  mesh->index_count * sizeof(unsigned int),
					  indices,
					  GL_STATIC_DRAW);

	glEnableVertexArrayAttrib (result->vertexArray, 0);  
	glVertexArrayAttribBinding(result->vertexArray, 0, 0);
	glVertexArrayAttribFormat (result->vertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);

	glVertexArrayVertexBuffer (result->vertexArray, 0, result->vertexBuffer, 0, 3  * sizeof(float));
	glVertexArrayElementBuffer(result->vertexArray, result->elementBuffer);

	result->premativesCount = mesh->index_count;
	result->shaderProgram   = shaderProgram;
	//result->position        = glm::vec3(50.0f, 50.0f, 50.0f);
                                   // scale the object by 100
	result->transformation.model = glm::mat4(1.0f);//glm::scale(, position);
	result->transformation.view  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -500.0f));
	result->transformation.proj  = glm::perspective(glm::radians(45.0f),
												(float)WindowWidth / (float)WindowHeight,
												0.1f, 1000.0f);

	
    // TODO: Destory mesh
	fast_obj_destroy(mesh);
	delete[] indices;
	
	return true;
	  
}

/*
void configImguiViewPorts(const ImGuiIO& io)
{
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		{
			style.Colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

			// Headers
			style.Colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
			style.Colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
			style.Colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		
			// Buttons
			style.Colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
			style.Colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
			style.Colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

			// Frame BG
			style.Colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
			style.Colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
			style.Colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

			// Tabs
			style.Colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
			style.Colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
			style.Colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
			style.Colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
			style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

			// Title
			style.Colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
			style.Colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		}
	}
	
}
*/

unsigned int createTexture(const char* filename)
{
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0); 

	if(!data)
	{
		printf("Error while loading [%s]\n", filename);
		return 0;
	}

	unsigned int texture;
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);

	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	printf("%s:  nrChannels = %d\n", filename, nrChannels);

	int glChannel, glChannel2;
	if (nrChannels == 3)
	{
		glChannel = GL_RGB8;
		glChannel2 = GL_RGB;
	}

	if (nrChannels == 4)
	{
		glChannel = GL_RGBA8;
		glChannel2 = GL_RGBA;
	}
	
	glTextureStorage2D(texture, 1, glChannel, width, height);
	glTextureSubImage2D(texture, 0, 0, 0, width, height, glChannel2, GL_UNSIGNED_BYTE, data);
	glGenerateTextureMipmap(texture);

	stbi_image_free(data);
	return texture;
}

bool quad2DScene(ShaderProgram shaderProgram, Model* result)
{

	constexpr auto width  = 200.0f;
	constexpr auto w      = width;
	constexpr auto height = 200.0f;
	constexpr auto h      = height;

	// prespective
	constexpr float verticesnew[] = {
		    -w / 2, -h/2, 0.0f, 0.1f, 0.5f, 0.4f, 0.0f, 0.0f,  // bottom left
		    -w / 2,  h/2, 0.0f, 0.8f, 0.2f, 0.3f, 0.0f, 1.0f,  // top left 
			 w / 2,  h/2, 0.0f, 0.3f, 0.8f, 0.6f, 1.0f, 1.0f,  // top right
			 w / 2, -h/2, 0.0f, 0.3f, 0.4f, 0.3f, 1.0f, 0.0f,  // bottom right
	}; 

	// ortho 
	constexpr float vertices[] = {
		    0.0f,     0.0f, 0.0f, 0.1f, 0.5f, 0.4f, 0.0f, 0.0f,  // bottom left
		    0.0f,   height, 0.0f, 0.8f, 0.2f, 0.3f, 0.0f, 1.0f,  // top left 
			width,  height, 0.0f, 0.3f, 0.8f, 0.6f, 1.0f, 1.0f,  // top right
			width,    0.0f, 0.0f, 0.3f, 0.4f, 0.3f, 1.0f, 0.0f,  // bottom right
	};


	constexpr unsigned int indices[] = {  
		0, 2, 1,   // first triangle
		0, 3, 2    // second triangle
	};  

	// Gen VertexArray Buffer
	// Gen Vertex Buffer & ElementBufferObject
	glCreateVertexArrays(1, &result->vertexArray);  
	glCreateBuffers(1, &result->vertexBuffer); 
	glCreateBuffers(1, &result->elementBuffer);


	glNamedBufferData(result->vertexBuffer,  sizeof(vertices), vertices, GL_STATIC_DRAW);
	glNamedBufferData(result->elementBuffer, sizeof(indices) , indices , GL_STATIC_DRAW);
	
	glEnableVertexArrayAttrib(result->vertexArray, 0);  
	glVertexArrayAttribBinding(result->vertexArray, 0, 0);
	glVertexArrayAttribFormat(result->vertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
	
	glEnableVertexArrayAttrib(result->vertexArray, 1);  
	glVertexArrayAttribBinding(result->vertexArray, 1, 0);
	glVertexArrayAttribFormat(result->vertexArray, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));

	glEnableVertexArrayAttrib(result->vertexArray, 2);  
	glVertexArrayAttribBinding(result->vertexArray, 2, 0);
	glVertexArrayAttribFormat(result->vertexArray, 2, 2, GL_FLOAT, GL_FALSE, 3 * 2 * sizeof(float));

	

	glVertexArrayVertexBuffer(result->vertexArray, 0, result->vertexBuffer, 0, 8  * sizeof(float));
	glVertexArrayElementBuffer(result->vertexArray, result->elementBuffer);


	result->premativesCount = sizeof(indices)/sizeof(indices[0]);
	result->shaderProgram   = shaderProgram;

	constexpr auto right  = WindowWidth * 1.0f;
	constexpr auto left   = 0.0f;
	constexpr auto top    = WindowHeight * 1.0f;
	constexpr auto bottom = 0.0f;


	result->transformation.model = glm::mat4(1.0f);
	result->transformation.view  = glm::mat4(1.0f);
	result->transformation.proj  = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	
	return true;
}
