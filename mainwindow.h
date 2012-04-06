#ifndef GLF_WINDOW_INCLUDED
#define GLF_WINDOW_INCLUDED

#define GL_GLEXT_PROTOTYPES 1
#include "GL/glew.h"

#include <QDebug>
#include <QGLWidget>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/half_float.hpp>
#include <glm/gtc/type_precision.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <iostream>
#include <fstream>


class GLWindow:public QGLWidget
{

    Q_OBJECT
public:
    GLWindow(QGLFormat format,QWidget* parent=0);
     ~GLWindow();
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    //Key , mouse handling below

private:
    void draw();

    GLuint program_id;
    GLuint vbo_id;
    GLuint vao_id;
    GLuint index_id;

    GLuint uniform_mvp;
    GLuint uniform_diffuse;

private:
    //Error Checking
    bool checkError(const char* Title);
    bool checkProgram(GLuint ProgramName);
    bool checkExtension(char const * String);
    bool checkShader(GLuint ShaderName, char const* Source);
    bool validateProgram(GLuint ProgramName);
    bool init_glew();

    //Shader Utils
    std::string loadFile(std::string const & Filename);
    GLuint createShader(GLenum Type,std::string const & Source);

    bool init_Program(void);
    bool init_Buffers(void);
};

#endif
