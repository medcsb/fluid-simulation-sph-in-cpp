#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <exception>
#include <ios>

#include "util.h"

// Create a GPU program i.e., a graphics pipeline
ShaderProgram::ShaderProgram() : _id(glCreateProgram()) {}

ShaderProgram::~ShaderProgram() {
  glDeleteProgram(_id);
}

void ShaderProgram::loadShaderFromSource(GLenum type, const std::string &shaderSource) {
  // loads and compiles a shader given its source code
  loadShader(type, shaderSource.c_str());
}

void ShaderProgram::loadShaderFromFile(GLenum type, const std::string &shaderFilename) {
  // loads and compiles a shader given its filename
  std::string shaderSource = file2String(shaderFilename);
  loadShader(type, shaderSource.c_str());
}

void ShaderProgram::loadShader(GLenum type, const char *shaderSource) {
  // loads and compiles a shader given its source code
    GLuint shader = glCreateShader(type);
    const GLchar *shaderSourceChar = (const GLchar *) shaderSource;
    glShaderSource(shader, 1, &shaderSourceChar, NULL);
    glCompileShader(shader);
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
      GLsizei len;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
      GLchar *log = new GLchar[len+1];
      glGetShaderInfoLog(shader, len, &len, log);
      exitOnCriticalError("Compilation error in shader : " + std::string(log), "loadShader");
      delete [] log;
      glDeleteShader(shader);
    }
    glAttachShader(_id, shader);
    glDeleteShader(shader);
}

std::shared_ptr<ShaderProgram> ShaderProgram::genBasicShaderProgram(
  const std::string &vertexShaderFilename,
  const std::string &fragmentShaderFilename) {
  std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
  shaderProgramPtr->loadShaderFromFile(GL_VERTEX_SHADER, vertexShaderFilename);
  shaderProgramPtr->loadShaderFromFile(GL_FRAGMENT_SHADER, fragmentShaderFilename);
  shaderProgramPtr->link();
  shaderProgramPtr->use();
  return shaderProgramPtr;
}

std::shared_ptr<ShaderProgram> ShaderProgram::genBasicShaderProgramFromSource(
  const std::string &vertexShaderSource,
  const std::string &fragmentShaderSource) {
  std::shared_ptr<ShaderProgram> shaderProgramPtr = std::make_shared<ShaderProgram>();
  shaderProgramPtr->loadShaderFromSource(GL_VERTEX_SHADER, vertexShaderSource);
  shaderProgramPtr->loadShaderFromSource(GL_FRAGMENT_SHADER, fragmentShaderSource);
  shaderProgramPtr->link();
  shaderProgramPtr->use();
  return shaderProgramPtr;
}
