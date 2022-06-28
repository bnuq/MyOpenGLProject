#include "program.h"

/* PUBLIC */
ProgramUPtr Program::Create(const std::vector<ShaderPtr>& shaders)
{
    auto program = ProgramUPtr(new Program());
    if (!program->Link(shaders))
        return nullptr;
    return std::move(program);
}

/* 
    Vertex Shader ??? Fragment Shader ?��?�� ?��름만 받고
    Shader -> GL shader ?��?��
    ?��?��
    Program -> GL program ?��?��
 */
ProgramUPtr Program::Create(const std::string& vertShaderFilename, const std::string& fragShaderFilename)
{
    ShaderPtr vs = Shader::CreateFromFile(vertShaderFilename, GL_VERTEX_SHADER);
    ShaderPtr fs = Shader::CreateFromFile(fragShaderFilename, GL_FRAGMENT_SHADER);
    
    if (!vs || !fs)
        return nullptr;
    return std::move(Create({vs, fs}));
}

Program::~Program()
{
    if (m_program)
    {   
        glDeleteProgram(m_program);
    }
}

void Program::Use() const
{
    glUseProgram(m_program);
}

/* 
    shader ?�� uniform �??��?�� 값을 ?��?��?��?�� ?��?��

    Uniform �??���? ?��?�� ?��, ?��?�� ?��?��?�� ?��?��?�� ?��?�� �??�� 중요?���? ?��?��
    GL program ?��?���? �?리되�? ?��문에, ?��로그?�� ?��?�� ?��?��?���? 존재?��?�� �?�? ?��?��?��?�� ?��?��

    GL program ?�� ?��?��?��?��?�� uniform �??��?�� ?��름으�? ?���?
    ?��?�� uniform ?�� ?��치�?? ?��?��?���?
    ?��치에 값을 집어 ?��?��?��
 */
void Program::SetUniform(const std::string& name, int value) const
{
    // 주어�? ?��름을 �?�??�� Uniform �??��?�� ?��치�?? ?��?��?���?
    auto loc = glGetUniformLocation(m_program, name.c_str());
    // ?��?�� ?��치에 값을 집어 ?��?��?��
    glUniform1i(loc, value);
}

void Program::SetUniform(const std::string& name, bool value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1i(loc, value);
}



void Program::SetUniform(const std::string& name, float value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1f(loc, value);
}

void Program::SetUniform(const std::string& name, const glm::vec2& value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform2fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const glm::vec3& value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const glm::vec4& value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform4fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string& name, const glm::mat4& value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

/* PRIVATE */
bool Program::Link( const std::vector<ShaderPtr>& shaders )
{
    /* 
        glCreateProgram
            OpenGL program object �? 리소?��?�� ?��?��?��?�� 만들�?
            ?��?�� 번호, ?��근할 ?�� ?��?�� id �? 리턴?��?��
        
        만든 GL program ?�� 컴파?�� ?�� ?��?��?��?��?�� ?���? 것이?��
     */
    m_program = glCreateProgram();
    
    for (auto& shader: shaders) // ?��?��받�?? 모든 gl shaders ?�� ????��?��
        /* 
            glAttachShader
                GL program object ?�� 컴파?�� ?�� ?��?�� 개의 GL shader objects �? 붙인?��
                program = shader ?��?�� 개�?? ?��결된 ?��?��?��?��?��
        */
        glAttachShader(m_program, shader->Get());


    /* 
        glLinkProgram
            컴파?�� ?�� GL shader objects �? 링크
            shader ?��?�� ?���? ?��결한?�� => ?��?��?�� gl program ?��?��
     */
    glLinkProgram(m_program);


    // Link ?���? ?���? ?��?��
    int success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to link program: {}", infoLog);
        return false;
    }


    return true;
}