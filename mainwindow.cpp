
#include "mainwindow.h"


bool GLWindow::checkError(const char *Title)
{
    int Error;

    if((Error = glGetError()) != GL_NO_ERROR)
    {
        std::string ErrorString;
        switch(Error)
        {
        case GL_INVALID_ENUM:
            ErrorString = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            ErrorString = "GL_INVALID_VALUE";
            break;
        case GL_INVALID_OPERATION:
            ErrorString = "GL_INVALID_OPERATION";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            ErrorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
            break;
        case GL_OUT_OF_MEMORY:
            ErrorString = "GL_OUT_OF_MEMORY";
            break;
        default:
            ErrorString = "UNKNOWN";
            break;
        }
        qDebug()<<"OpenGL Error:"<<ErrorString.c_str()<<" :"<<Title<<"\n";
    }
    return Error == GL_NO_ERROR;
}

bool GLWindow::validateProgram(GLuint ProgramName)
{
    if(!ProgramName)
        return false;


    glValidateProgram(ProgramName);
    GLint Result = GL_FALSE;
    glGetProgramiv(ProgramName, GL_VALIDATE_STATUS, &Result);

    if(Result == GL_FALSE)
    {
        fprintf(stdout, "Validate program\n");
        int InfoLogLength;
        glGetProgramiv(ProgramName, GL_INFO_LOG_LENGTH, &InfoLogLength);
        std::vector<char> Buffer(InfoLogLength);
        glGetProgramInfoLog(ProgramName, InfoLogLength, NULL, &Buffer[0]);
        fprintf(stdout, "%s\n", &Buffer[0]);

    }

    return Result == GL_TRUE;
}

bool GLWindow::checkProgram(GLuint ProgramName)
{
    if(!ProgramName)
        return false;

    GLint Result = GL_FALSE;
    glGetProgramiv(ProgramName, GL_LINK_STATUS, &Result);

    fprintf(stdout, "Linking program\n");
    int InfoLogLength;
    glGetProgramiv(ProgramName, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> Buffer(std::max(InfoLogLength, int(1)));
    glGetProgramInfoLog(ProgramName, InfoLogLength, NULL, &Buffer[0]);
    fprintf(stdout, "%s\n", &Buffer[0]);

    return Result == GL_TRUE;
}

bool GLWindow::checkShader(GLuint ShaderName, const char *Source)
{
    if(!ShaderName)
        return false;

    GLint Result = GL_FALSE;
    glGetShaderiv(ShaderName, GL_COMPILE_STATUS, &Result);

    fprintf(stdout, "Compiling shader\n%s...\n", Source);
    int InfoLogLength;
    glGetShaderiv(ShaderName, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> Buffer(InfoLogLength);
    glGetShaderInfoLog(ShaderName, InfoLogLength, NULL, &Buffer[0]);
    fprintf(stdout, "%s\n", &Buffer[0]);


    return Result == GL_TRUE;
}




bool GLWindow::checkExtension(char const * String)
{
    GLint ExtensionCount = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &ExtensionCount);
    for(GLint i = 0; i < ExtensionCount; ++i)
        if(std::string((char const*)glGetStringi(GL_EXTENSIONS, i)) == std::string(String))
            return true;
    return false;
}
GLuint  GLWindow::createShader(GLenum Type,std::string const & Source)
{
    bool Validated = true;
    GLuint Name = 0;

    if(!Source.empty())
    {
        std::string SourceContent = loadFile(Source);
        char const * SourcePointer = SourceContent.c_str();
        Name = glCreateShader(Type);
        glShaderSource(Name, 1, &SourcePointer, NULL);
        glCompileShader(Name);
        Validated = checkShader(Name, SourcePointer);
    }

    return Name;
}

inline std::string GLWindow::loadFile(std::string const & Filename)
{
    std::ifstream stream(Filename.c_str(), std::ios::in);

    if(!stream.is_open())
        return "";

    std::string Line = "";
    std::string Text = "";

    while(getline(stream, Line))
        Text += "\n" + Line;

    stream.close();

    return Text;
}


GLWindow::GLWindow(QGLFormat format,QWidget *parent):QGLWidget(format,parent)
{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

 }

bool GLWindow::init_glew()
{
    bool bRet = false;
    GLenum err = glewInit();
    if(err == GLEW_OK )
        bRet = true;
    if(bRet && GLEW_VERSION_3_3)
        bRet = true;
    if(bRet)
        bRet = glewGetExtension("GL_ARB_shader_objects");
    bRet = bRet && checkExtension("GL_ARB_viewport_array");
    bRet = bRet && checkExtension("GL_ARB_separate_shader_objects");
    return  bRet;
}

void GLWindow::initializeGL()
{
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClearDepth(0.0f);

//    QGLFormat format = this->format();


    if(!init_glew())
    {
        qDebug()<<"Glew Related Error";
        exit(1);
    }

    bool bValidated = true;
    if(bValidated)
        bValidated = init_Program();
    if(bValidated)
        bValidated = init_Buffers();

    if(!bValidated)
    {
        qDebug()<<"Error";
        checkError("initializeGL");
        exit(0);
    }


}

void GLWindow::resizeGL(int w, int h)
{
  glViewport(0,0,w,h);
}

void GLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    draw();
}

void GLWindow::draw()
{
    glm::mat4 proj_Matrix = glm::perspective<float>(45.0f,4.0/3.0f,0.1f,100.0f);
    glm::mat4 view_Translate_Matrix = glm::translate(glm::mat4(1.0f),
                                                     glm::vec3(0.0f,0.0f,-10.0f));
    glm::mat4 view_Matrix = glm::rotate(view_Translate_Matrix,
                                        45.0f,
                                        glm::vec3(0.0f,1.0f,0.0f));
    glm::mat4 model_Matrix = glm::mat4(1.0f);
    glm::mat4 MVP = proj_Matrix * view_Matrix * model_Matrix;


    glProgramUniform4fv(program_id,uniform_diffuse,1,
                        &glm::vec4(1.0f,0.5f,0.0f,1.0f)[0]);

    glProgramUniformMatrix4fv(program_id,
                              uniform_mvp,
                              1,
                              GL_FALSE,
                              &MVP[0][0]);



    float fDepth(1.0f);
    glClearBufferfv(GL_DEPTH,0,&fDepth);
    glClearBufferfv(GL_COLOR,0,&glm::vec4(0.0f,0.0f,0.0f,1.0f)[0]);


    glUseProgram(program_id);

    glBindVertexArray(vao_id);
    checkError("Bef Draw");
    glDrawElements(GL_LINE_LOOP,24,GL_UNSIGNED_BYTE,0);


    checkError("draw");
    swapBuffers();
}

bool GLWindow::init_Program()
{
    bool bValidated = true;
    if(bValidated)
    {
        GLuint vertex_shader = createShader(GL_VERTEX_SHADER,"./vertex.glsl");
        GLuint fragment_shader = createShader(GL_FRAGMENT_SHADER,"./fragment.glsl");

        program_id = glCreateProgram();
        glAttachShader(program_id,vertex_shader);
        glAttachShader(program_id,fragment_shader);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glLinkProgram(program_id);

        bValidated = checkProgram(program_id);
    }
    if(bValidated)
    {
        uniform_mvp = glGetUniformLocation(program_id,"MVP");
        uniform_diffuse = glGetUniformLocation(program_id,"Diffuse");
    }
    return bValidated && checkError("init_Program");
}

bool GLWindow::init_Buffers()
{
    float fVertex[]={
            -1.0f,1.0f,
            1.0f,1.0f,
            1.5f,0.5f,
            1.5f,-0.5f,
            1.0f,-1.0f,
            -1.0f,-1.0f,
            -1.5f,-0.5f,
            -1.5f,.05f,
            0.0f,0.0f
    };

    GLubyte ubIndex[]={
            8,0,1,
            8,1,2,
            8,2,3,
            8,3,4,
            8,4,5,
            8,5,6,
            8,6,7,
            8,7,0};

    glGenVertexArrays(1,&vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(1,&vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_id);
    glBufferData(GL_ARRAY_BUFFER,sizeof(fVertex),fVertex,GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    glGenBuffers(1,&index_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,index_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(ubIndex),ubIndex,GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER,vbo_id);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(0);

    return checkError("init_buffer");

}

GLWindow::~GLWindow()
{
    glDeleteProgram(program_id);
    glBindVertexArray(0);
    glDeleteVertexArrays(1,&vao_id);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glDeleteBuffers(1,&vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    glDeleteBuffers(1,&index_id);
    glUseProgram(0);
    checkError("end");
}
