#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/vector_angle.hpp>
#include <gtx/string_cast.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

//camera position variable
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//timing variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

const char *vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"uniform mat4 projection;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
	"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"uniform vec4 ourColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = ourColor;\n"
	"}\n\0";

int main()
{
	//Initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create Window
	GLFWwindow *window = glfwCreateWindow(800, 600, "VoxelRender", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	//Set Context
	glfwMakeContextCurrent(window);
	//Set Frame buffer callback
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Shader set up

	//Vertex Shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	//error check
	int success;
	char infolog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infolog << std::endl;
	}

	//Fragment Shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	//error check
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infolog << std::endl;
	}

	//Link the shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//error check
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) 
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infolog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//configure opengl state
	glEnable(GL_DEPTH_TEST);

	//Set up vertex data and attributes
	float vertices[] = 
	{
		//Create cube, each side cosists of four traingles that meet in the center of the side.

		-0.5f,-0.5f,-0.5f,0.0f,0.0f,-0.5f,0.5f,-0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,0.0f,0.0f,0.5f,0.5f,-0.5f,0.5f,
		0.5f,0.5f,-0.5f,0.0f,0.0f,-0.5f,-0.5f,0.5f,-0.5f,
		0.5f,0.5f,0.5f,0.0f,0.0f,0.5f,-0.5f,0.5f,0.5f,
		-0.5f,0.5f,-0.5f,0.0f,0.0f,-0.5f,-0.5f,-0.5f,-0.5f,
		-0.5f,0.5f,0.5f,0.0f,0.0f,0.5f,-0.5f,-0.5f,0.5f,
		0.5f,0.5f,-0.5f,0.0f,0.0f,-0.5f,0.5f,-0.5f,-0.5f,
		0.5f,0.5f,0.5f,0.0f,0.0f,0.5f,0.5f,-0.5f,0.5f,

		-0.5f,0.5f,0.5f,-0.5f,0.0f,0.0f,-0.5f,0.5f,-0.5f,
		0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.5f,0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,-0.5f,0.0f,0.0f,-0.5f,-0.5f,-0.5f,
		0.5f,-0.5f,0.5f,0.5f,0.0f,0.0f,0.5f,-0.5f,-0.5f,
		-0.5f,0.5f,-0.5f,-0.5f,0.0f,0.0f,-0.5f,-0.5f,-0.5f,
		0.5f,0.5f,-0.5f,0.5f,0.0f,0.0f,0.5f,-0.5f,-0.5f,
		-0.5f,0.5f,0.5f,-0.5f,0.0f,0.0f,-0.5f,-0.5f,0.5f,
		0.5f,0.5f,0.5f,0.5f,0.0f,0.0f,0.5f,-0.5f,0.5f,

		-0.5f,0.5f,-0.5f,0.0f,0.5f,0.0f,0.5f,0.5f,-0.5f,
		-0.5f,-0.5f,-0.5f,0.0f,-0.5f,0.0f,0.5f,-0.5f,-0.5f,
		-0.5f,0.5f,0.5f,0.0f,0.5f,0.0f,0.5f,0.5f,0.5f,
		-0.5f,-0.5f,0.5f,0.0f,-0.5f,0.0f,0.5f,-0.5f,0.5f,
		-0.5f,0.5f,0.5f,0.0f,0.5f,0.0f,-0.5f,0.5f,-0.5f,
		-0.5f,-0.5f,0.5f,0.0f,-0.5f,0.0f,-0.5f,-0.5f,-0.5f,
		0.5f,0.5f,-0.5f,0.0f,0.5f,0.0f,0.5f,0.5f,0.5f,
		0.5f,-0.5f,-0.5f,0.0f,-0.5f,0.0f,0.5f,-0.5f,0.5f
	};

	//array buffers
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);



	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices)*sizeof(vertices[0]), vertices, GL_STATIC_DRAW);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);
	//texture coordinate attribute
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	//declare vector for voxel data
	std::vector<glm::vec3> voxeldata;
	std::vector<glm::vec4> colordata;

	//open voxel data file and read data into vector
	std::ifstream infile;
	infile.open("C:\\VSCode\\StevesVoxelRenderer\\VoxRender2\\voxeldata.txt");
	//std::cout << "got here!\n";
	glm::vec3 filedata;
	glm::vec4 filecolor;
	std::string datastring;
	//std::string::size_type sz;
	std::stringstream datastream;

	if (infile.fail())
	{
		std::cout << "File not open!" << "\n";
	}


	while (std::getline(infile,datastring))
	{
		//std::cout << "Got here!" << "\n";
		//std::cout << datastring << "\n";
		//reads a line of data from file.  Each line should be x,y,z.
		//now must separate coordinates by comma and load into vec3
		std::string token;
		datastream << datastring;
		std::getline(datastream, token, ',');
		//std::cout << token << "\n";
		filedata.x = stof(token);
		std::getline(datastream, token, ',');
		//std::cout << token << "\n";
		filedata.y = stof(token);
		std::getline(datastream, token, ',');
		//std::cout << token << "\n";
		filedata.z = stof(token);
		std::getline(datastream, token, ',');
		//std::cout << token << "\n";
		filecolor.x = stof(token);
		std::getline(datastream, token, ',');
		//std::cout << token << "\n";
		filecolor.y = stof(token);
		std::getline(datastream, token, ',');
		//std::cout << token << "\n";
		filecolor.z = stof(token);
		std::getline(datastream, token);
		//std::cout << token << "\n";
		filecolor.w = stof(token);
		std::cout << glm::to_string(filedata) << "\n";
		voxeldata.push_back(filedata);
		colordata.push_back(filecolor);
		datastream.str("");
		datastream.clear();
	}


	//Main loop
	while (!glfwWindowShouldClose(window))
	{
		//calcualte time stuff
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		//render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//set projection, view and model matrices.  
		//model will be static, at least for now
		glm::mat4 model = glm::mat4(1.0f);

		//using projection from learnopengl code
		glm::mat4 projection = glm::perspective(glm::radians(60.0f), (float)800 / (float)600, 0.1f, 100.0f);//radians was 60
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		//set view based on keyboard input
		glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		glBindVertexArray(VAO);

		for (int i = 0; i < voxeldata.size(); i++)
		{
			model = glm::mat4(1.0f);
			//std::cout << glm::to_string(voxeldata[i]) << "\n";
			//translate a block
			model = glm::translate(model, voxeldata[i]);
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

			//activate shader
			glUseProgram(shaderProgram);
			int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
			glUniform4f(vertexColorLocation, colordata[i].x, colordata[i].y, colordata[i].z, colordata[i].w);

			//render box
			glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/3);

		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);


	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		//decrease radius
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		//increase radius
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		//rotate clockwise
		glm::vec3 v3RotAxis(0.0f, 1.0f, 0.0f); // Rotate about y+
		float angleRad = 0.001f; // Rotate of a quarter of a turn
		glm::mat4x4 matRot = glm::rotate(glm::mat4(1.0), angleRad, v3RotAxis);
		cameraPos = glm::vec3(glm::vec4(cameraPos, 0.0f) * matRot);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		//rotate counterclockwise
		glm::vec3 v3RotAxis(0.0f, 1.0f, 0.0f); // Rotate about y+
		float angleRad = -0.001f; // Rotate of a quarter of a turn
		glm::mat4x4 matRot = glm::rotate(glm::mat4(1.0), angleRad, v3RotAxis);
		cameraPos = glm::vec3(glm::vec4(cameraPos, 0.0f) * matRot);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{		
		//rotate up
		glm::vec3 v3RotAxis(cameraPos.z, 0.0f, -cameraPos.x);
		v3RotAxis = glm::normalize(v3RotAxis);
		//std::cout << glm::to_string(v3RotAxis) << "\n";
		float angleRad = -0.001f; // Rotate of a quarter of a turn
		glm::mat4x4 matRot = glm::rotate(glm::mat4(1.0), angleRad, v3RotAxis);
		cameraPos = glm::vec3(glm::vec4(cameraPos, 0.0f) * matRot);
	}
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		//rotate down
		glm::vec3 v3RotAxis(cameraPos.z, 0.0f, -cameraPos.x);
		v3RotAxis = glm::normalize(v3RotAxis);
		float angleRad = 0.001f; // Rotate of a quarter of a turn
		glm::mat4x4 matRot = glm::rotate(glm::mat4(1.0), angleRad, v3RotAxis);
		cameraPos = glm::vec3(glm::vec4(cameraPos, 0.0f) * matRot);
	}
}