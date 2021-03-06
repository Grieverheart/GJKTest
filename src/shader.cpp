#include "include/shader.h"
#include <fstream>
#include <cstring>

static inline void validateShader(GLuint shader, const char *file = 0){
	const unsigned int BUFFER_SIZE = 512;
	char buffer[BUFFER_SIZE];
	//memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;
	
	glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
	
	if(length>0){
		printf("Shader %u (%s) compile error: %s\n", shader, (file?file:""), buffer);
	}
}

static inline bool validateProgram(GLuint program){
	GLsizei BUFFER_SIZE = 512;
	GLchar buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	GLsizei length = 0;
	
	glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
	
	if(length>0){
		printf("Program %u link error: %s\n", program, buffer);
	}
	
	glValidateProgram(program);
	GLint status;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
	
	if(status == GL_FALSE){
		printf("Error validating program %u\n", program);
		return false;
	}
	return true;
}

static std::string textFileRead(const char *fileName){
	std::string fileString = std::string(); // A string for storing the file contents
	std::string line = std::string(); // A string for holding the current line

	std::ifstream file(fileName); // Open an input stream with the selected file
	if (file.is_open()) { // If the file opened successfully
		while (!file.eof()) { // While we are not at the end of the file
			getline(file, line); // Get the current line
		  	fileString.append(line); // Append the line to our file string
			fileString.append("\n"); // Appand a new line character
		}
		file.close(); // Close the file
	}

    return fileString; // Return our string
}

Shader::Shader(void):
    shader_id(0), shader_vp(0), shader_gp(0), shader_fp(0)
{}

Shader::Shader(const char *vsFile, const char *fsFile, const char *gsFile):
    shader_id(0), shader_vp(0), shader_gp(0), shader_fp(0)
{
	bool isGS = false;
	if(gsFile) isGS = true;
	shader_vp = glCreateShader(GL_VERTEX_SHADER);
	shader_fp = glCreateShader(GL_FRAGMENT_SHADER);
	if(isGS) shader_gp = glCreateShader(GL_GEOMETRY_SHADER);
	
	std::string vsText = textFileRead(vsFile);
	std::string fsText = textFileRead(fsFile);
	std::string gsText = "";
	if(isGS) gsText = textFileRead(gsFile);
    
	const char *vertexText = vsText.c_str();
	const char *fragmentText = fsText.c_str();
	const char *geometryText = gsText.c_str();
	
	if(vertexText == NULL || fragmentText == NULL || (isGS && geometryText == NULL)){
		printf("Either vertex shader or fragment shader file not found\n");
		return;
	}
	
	glShaderSource(shader_vp, 1, &vertexText, 0);
	glCompileShader(shader_vp);
	validateShader(shader_vp, vsFile);
	
	if(isGS){
		glShaderSource(shader_gp, 1, &geometryText, 0);
		glCompileShader(shader_gp);
		validateShader(shader_gp, gsFile);
	}
	
	glShaderSource(shader_fp, 1, &fragmentText, 0);
	glCompileShader(shader_fp);
	validateShader(shader_fp, fsFile);
	
	shader_id = glCreateProgram();
	
	glAttachShader(shader_id, shader_fp);
	if(isGS) glAttachShader(shader_id, shader_gp);
	glAttachShader(shader_id, shader_vp);
	
	glLinkProgram(shader_id);
	if(validateProgram(shader_id)){
		init();
	}
}

Shader::~Shader(void){
	glDetachShader(shader_id, shader_fp);
	glDetachShader(shader_id, shader_vp);
	if(shader_gp){
		glDetachShader(shader_id, shader_gp);
		glDeleteShader(shader_gp);
	}
	
	glDeleteShader(shader_fp);
	glDeleteShader(shader_vp);
	glDeleteShader(shader_id);
}

void Shader::init(void){
	GLint nUniforms;
	glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &nUniforms);
	for(GLint i = 0; i < nUniforms; ++i){
		GLchar name[64];
		GLint size;
		GLenum type;
		glGetActiveUniform(shader_id, i, 64, NULL, &size, &type, name);
		GLint location = glGetUniformLocation(shader_id, name);
		mUniformLocations[std::string(name)] = location;
		if(location == -1) printf("Couldn't bind uniform %s in program %u.\n", name, shader_id);
	}
}

unsigned int Shader::id(void){
	return shader_id;
}

void Shader::bind(void){
	glUseProgram(shader_id);
}

void Shader::unbind(void){
	glUseProgram(0);
}

void Shader::setUniform(const char * name, int count, glm::vec2 const &value)const{
	GLint location = mUniformLocations.find(std::string(name))->second;
	glUniform2fv(location, count, &value[0]);
}

void Shader::setUniform(const char * name, int count, glm::vec3 const &value)const{
	GLint location = mUniformLocations.find(std::string(name))->second;
	glUniform3fv(location, count, &value[0]);
}

void Shader::setUniform(const char * name, int count, glm::vec4 const &value)const{
	GLint location = mUniformLocations.find(std::string(name))->second;
	glUniform4fv(location, count, &value[0]);
}

void Shader::setUniform(const char * name, int count, glm::ivec2 const &value)const{
	GLint location = mUniformLocations.find(std::string(name))->second;
	glUniform2iv(location, count, &value[0]);
}

void Shader::setUniform(const char * name, int count, glm::ivec3 const &value)const{
	GLint location = mUniformLocations.find(std::string(name))->second;
	glUniform3iv(location, count, &value[0]);
}
void Shader::setUniform(const char * name, int count, glm::ivec4 const &value)const{
	GLint location = mUniformLocations.find(std::string(name))->second;
	glUniform4iv(location, count, &value[0]);
}

void Shader::setUniform(const char * name, int count, glm::mat3 const &value)const{
	GLint location = mUniformLocations.find(std::string(name))->second;
	glUniformMatrix3fv(location, count, GL_FALSE, &value[0][0]);
}

void Shader::setUniform(const char * name, int count, glm::mat4 const &value)const{
	GLint location = mUniformLocations.find(std::string(name))->second;
	glUniformMatrix4fv(location, count, GL_FALSE, &value[0][0]);
}

void Shader::setUniform(const char * name, int value)const{
	GLint location = mUniformLocations.find(std::string(name))->second;
	glUniform1i(location, value);
}

void Shader::setUniform(const char * name, float value)const{
	GLint location = mUniformLocations.find(std::string(name))->second;
	glUniform1f(location, value);
}

void Shader::setUniform(const char * name, float value1, float value2, float value3)const{
	GLint location = mUniformLocations.find(std::string(name))->second;
	glUniform3f(location, value1, value2, value3);
}
