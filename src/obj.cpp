/* ========================================================================
   $File: obj.cpp
   $Date: 2022-02-24
   $Revision: : @Incomplete
   $Creator: Husam Dababneh
   $Description: OBJ file Loader
    ========================================================================*/


#include "obj.h"

	//load_obj("C:\\trials\\twelf\\data\\models\\cube.obj");

struct Mesh {
	String_View name;
	Array<glm::vec3> verts;
	Array<glm::vec2> normals;
	Array<glm::vec3> faces;
	Array<glm::vec2> uvs;
};

static void
load_obj(const char * filepath)
{
	// @Leak:
	auto original = read_entire_file(filepath);
	auto data = original;



	Array<glm::vec3> vertices;
	//Array<glm::vec3> faces; 
	Array<Model> models;
	Mesh* currentModel = nullptr;
	if (!init_array(&models, 2)) return;

	if (!init_array(&vertices, 100)) return;

	String_View line;
	String_View temp;
	while(sv_try_chop_by_delim(&data, '\n', &line))
	{
		switch(line.data[0])
		{
		  case 'o':
		  case 'O':
			  sv_try_chop_by_delim(&line, ' ', &temp);
			  sv_try_chop_by_delim(&line, ' ', &temp);
			  //printf( "" SV_Fmt "\n", SV_Arg(line));
			  break;
		  case 'g':
			  break;
		  case 'V':
		  case 'v':
		  {
			  if(line.data[2] == ' ')
			  {
				  sv_try_chop_by_delim(&line, ' ', &temp);
				  double x = atof(line.data);
			  
				  sv_try_chop_by_delim(&line, ' ', &temp);
				  double y = atof(line.data);
			  
				  sv_try_chop_by_delim(&line, ' ', &temp);
				  double z = atof(line.data);

				  insert_array(&vertices, glm::vec3(x, y, z));
			  break;
			  }
		  }
		  case 'F':
		  case 'f':
		  {
			  break;
		  }
		}
	}

	data = original;


	for(size_t i = 0; i < vertices.occupied; i++)
	{
		print(vertices[i]);
	}
	
	free_array(&vertices);

	delete[] original.data;
}
