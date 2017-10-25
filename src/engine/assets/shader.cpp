#include "shader.h"
#include "engine/engine.h"

const std::string SHADER_BASE_DIRECTORY = ":/glsl/";

Shader::Shader() : Asset(),
    context(Engine::GetInstance()->GetContext()), shaderName(""),
    vertShader(-1), fragShader(-1), prog(-1),
    attrPos(-1), attrNor(-1), attrTan(-1), attrBi(-1), attrCol(-1), attrUV(-1), attrKind(-1),
    loaded(false)
{
    vertexFilename = "";
    fragmentFilename = "";
}

Shader::~Shader()
{
}

void Shader::LoadFromFilename(std::string filename)
{
    shaderName = filename;
    vertexFilename = SHADER_BASE_DIRECTORY + shaderName + ".vert.glsl";
    fragmentFilename = SHADER_BASE_DIRECTORY + shaderName + ".frag.glsl";
}

void Shader::Upload()
{    
    if(loaded)
        return;

    Engine::LogInfo("Loading " + vertexFilename);
    Engine::LogInfo("Loading " + fragmentFilename);

    // Allocate space on our GPU for a vertex shader and a fragment shader and a shader program to manage the two
    vertShader = context->glCreateShader(GL_VERTEX_SHADER);
    fragShader = context->glCreateShader(GL_FRAGMENT_SHADER);
    prog = context->glCreateProgram();

    // Get the body of text stored in our two .glsl files
    QString qVertSource = qTextFileRead(vertexFilename.data());
    QString qFragSource = qTextFileRead(fragmentFilename.data());

    char* vertSource = new char[qVertSource.size() + 1];
    strcpy(vertSource, qVertSource.toStdString().c_str());

    char* fragSource = new char[qFragSource.size() + 1];
    strcpy(fragSource, qFragSource.toStdString().c_str());

    // Send the shader text to OpenGL and store it in the shaders specified by the handles vertShader and fragShader
    context->glShaderSource(vertShader, 1, &vertSource, 0);
    context->glShaderSource(fragShader, 1, &fragSource, 0);

    // Tell OpenGL to compile the shader text stored above
    context->glCompileShader(vertShader);
    context->glCompileShader(fragShader);

    // Check if everything compiled OK
    GLint compiled;
    context->glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(vertShader);
    }
    context->glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(fragShader);
    }

    // Tell prog that it manages these particular vertex and fragment shaders
    context->glAttachShader(prog, vertShader);
    context->glAttachShader(prog, fragShader);
    context->glLinkProgram(prog);

    // Check for linking success
    GLint linked;
    context->glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        printLinkInfoLog(prog);
    }

    // Get the handles to the variables stored in our shaders
    // See shaderprogram.h for more information about these variables

    attrPos = context->glGetAttribLocation(prog, "vs_Pos");
    attrNor = context->glGetAttribLocation(prog, "vs_Nor");
    attrTan = context->glGetAttribLocation(prog, "vs_Tan");
    attrBi =  context->glGetAttribLocation(prog, "vs_Bi");

    attrCol = context->glGetAttribLocation(prog, "vs_Col");
    attrUV = context->glGetAttribLocation(prog, "vs_UV");
    attrKind = context->glGetAttribLocation(prog, "vs_Kind");

    loaded = true;
}

void Shader::Bind()
{
    context->glUseProgram(prog);
}


//This function, as its name implies, uses the passed in GL widget
void Shader::Render(Mesh *mesh, GLenum drawMode)
{
    Bind();

    if(mesh->IsInterleaved())
    {
        if(mesh->BindInterleaved())
        {
            if (attrPos != -1)
            {
                context->glEnableVertexAttribArray(attrPos);
                context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, sizeof(Vertex), 0);
            }

            if (attrNor != -1)
            {
                context->glEnableVertexAttribArray(attrNor);
                context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, sizeof(Vertex), (void*)(sizeof(glm::vec4)));
            }

            if (attrTan != -1)
            {
                context->glEnableVertexAttribArray(attrTan);
                context->glVertexAttribPointer(attrTan, 4, GL_FLOAT, false, sizeof(Vertex), (void*)(2 *sizeof(glm::vec4)));
            }

            if (attrBi != -1)
            {
                context->glEnableVertexAttribArray(attrBi);
                context->glVertexAttribPointer(attrBi, 4, GL_FLOAT, false, sizeof(Vertex), (void*)(3 *sizeof(glm::vec4)));
            }

            if (attrCol != -1)
            {
                context->glEnableVertexAttribArray(attrCol);
                context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, sizeof(Vertex), (void*)(4 * sizeof(glm::vec4)));
            }

            if (attrUV != -1)
            {
                context->glEnableVertexAttribArray(attrUV);
                context->glVertexAttribPointer(attrUV, 2, GL_FLOAT, false, sizeof(Vertex), (void*)(5 * sizeof(glm::vec4)));
            }


            if (attrKind != -1)
            {
                context->glEnableVertexAttribArray(attrKind);
                context->glVertexAttribPointer(attrKind, 1, GL_FLOAT, false, sizeof(Vertex), (void*)(5 * sizeof(glm::vec4) + sizeof(glm::vec2)));
            }

        }
    }
    else
    {
        if (attrPos != -1 && mesh->BindVertices()) {
            context->glEnableVertexAttribArray(attrPos);
            context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
        }

        if (attrNor != -1 && mesh->BindNormals()) {
            context->glEnableVertexAttribArray(attrNor);
            context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
        }

        if (attrTan != -1 && mesh->BindTangents()) {
            context->glEnableVertexAttribArray(attrTan);
            context->glVertexAttribPointer(attrTan, 4, GL_FLOAT, false, 0, NULL);
        }

        if (attrBi != -1 && mesh->BindBiNormals()) {
            context->glEnableVertexAttribArray(attrBi);
            context->glVertexAttribPointer(attrBi, 4, GL_FLOAT, false, 0, NULL);
        }

        if (attrCol != -1 && mesh->BindColors()) {
            context->glEnableVertexAttribArray(attrCol);
            context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
        }

        if (attrUV != -1 && mesh->BindUVs()) {
            context->glEnableVertexAttribArray(attrUV);
            context->glVertexAttribPointer(attrUV, 2, GL_FLOAT, false, 0, NULL);
        }

        if (attrKind != -1 && mesh->BindKinds()) {
            context->glEnableVertexAttribArray(attrKind);
            context->glVertexAttribPointer(attrKind, 1, GL_INT, false, 0, NULL);
        }

    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    mesh->BindIndices();
    context->glDrawElements(drawMode, mesh->GetIndicesCount(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrTan != -1) context->glDisableVertexAttribArray(attrTan);
    if (attrBi != -1) context->glDisableVertexAttribArray(attrBi);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);

    if (attrUV != -1) context->glDisableVertexAttribArray(attrUV);
    if (attrKind != -1) context->glDisableVertexAttribArray(attrKind);

    context->printGLErrorLog();
}

char* Shader::textFileRead(const char* fileName) {
    char* text = const_cast<char*>("");

    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");

        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);

            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }

    return text;
}

QString Shader::qTextFileRead(const char *fileName)
{
    QString text;
    QFile file(fileName);
    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        text = in.readAll();
        text.append('\0');
    }
    return text;
}

std::string Shader::GetName()
{
    return shaderName;
}

void Shader::printShaderInfoLog(int shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0)
    {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
        qDebug() << "ShaderInfoLog:" << endl << infoLog << endl;
        delete [] infoLog;
    }

    // should additionally check for OpenGL errors here
}

void Shader::printLinkInfoLog(int prog)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    qDebug() << infoLogLen;

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        qDebug() << "LinkInfoLog:" << endl << infoLog << endl;
        delete [] infoLog;
    }
}

int Shader::GetUniformLocation(const char *uniform)
{
    Bind();

    return context->glGetUniformLocation(prog, uniform);
}

void Shader::SetIntUniform(int uniform, int value)
{
    Bind();

    if(uniform != -1)
        context->glUniform1i(uniform, value);
}

void Shader::SetFloatUniform(int uniform, float value)
{
    Bind();

    if(uniform != -1)
        context->glUniform1f(uniform, value);
}

void Shader::SetTextureUniform(int uniform, GLuint value, uint counter)
{
    Bind();

    if(uniform != -1)
    {

        context->glUniform1i(uniform, counter);
        context->glActiveTexture(GL_TEXTURE0 + counter);

        //context->glDisable(GL_TEXTURE_CUBE_MAP);
        //context->glEnable(GL_TEXTURE_2D);

        context->glBindTexture(GL_TEXTURE_2D, value);
    }
}

void Shader::SetCubeTextureUniform(int uniform, GLuint value, uint &counter)
{
    Bind();


    if(uniform != -1)
    {
        context->glUniform1i(uniform, counter);
        context->glActiveTexture(GL_TEXTURE0 + counter);

        //context->glDisable(GL_TEXTURE_2D);
        //context->glEnable(GL_TEXTURE_CUBE_MAP);

        context->glBindTexture(GL_TEXTURE_CUBE_MAP, value);
        counter++;
    }
}

void Shader::SetMatrixUniform(int uniform, const glm::mat4 &matrix)
{
        Bind();

        if(uniform != -1)
        {
            // Pass a 4x4 matrix into a uniform variable in our shader
            // Handle to the matrix variable on the GPU
            context->glUniformMatrix4fv(uniform,
                            // How many matrices to pass
                               1,
                            // Transpose the matrix? OpenGL uses column-major, so no.
                               GL_FALSE,
                            // Pointer to the first element of the matrix
                               &matrix[0][0]);
        }
}

void Shader::SetMatrixArrayUniform(int uniform, glm::mat4 *matrix, int size)
{
    Bind();

    if(uniform != -1)
    {
        // Pass a 4x4 matrix into a uniform variable in our shader
        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(uniform,
                        // How many matrices to pass
                           size,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &(matrix[0])[0][0]);
    }
}

void Shader::SetVectorUniform(int uniform, const glm::vec4 &v)
{
    Bind();

    if(uniform != -1)
        context->glUniform4fv(uniform, 1, &v[0]);
}

void Shader::SetIVectorUniform(int uniform, const glm::ivec4 &v)
{
    Bind();

    if(uniform != -1)
        context->glUniform4iv(uniform, 1, &v[0]);
}
