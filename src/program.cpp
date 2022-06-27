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
    Vertex Shader ??? Fragment Shader ??Ό ?΄λ¦λ§ λ°κ³ 
    Shader -> GL shader ??±
    ?΄?
    Program -> GL program ??±
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
    shader ? uniform λ³??? κ°μ ? ?¬?? ?¨?

    Uniform λ³??λ₯? ?£? ?, ?΄?€ ?°?΄? ??Ό? ?£? μ§?? μ€μ?μ§? ??€
    GL program ?¨?λ‘? κ΄?λ¦¬λκΈ? ?λ¬Έμ, ?λ‘κ·Έ?¨ ?΄? ?°?΄?κ°? μ‘΄μ¬?? μ§?λ₯? ???΄?Ό ??€

    GL program ? ?¬?©?? €? uniform λ³??? ?΄λ¦μΌλ‘? ? κ·?
    ?΄?Ή uniform ? ?μΉλ?? ???΄κ³?
    ?μΉμ κ°μ μ§μ΄ ?£??€
 */
void Program::SetUniform(const std::string& name, int value) const
{
    // μ£Όμ΄μ§? ?΄λ¦μ κ°?μ§?? Uniform λ³??? ?μΉλ?? ???κ³?
    auto loc = glGetUniformLocation(m_program, name.c_str());
    // ?΄?Ή ?μΉμ κ°μ μ§μ΄ ?£??€
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
    
    // λ²‘ν°? ?? ¬? ?£? ?? κ°μ λ³΅μ¬?μ§? ?κ³? ?¬?Έ?°λ₯? ?κΈ΄λ€ => value_ptr ?¬?©
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
            OpenGL program object λ₯? λ¦¬μ?€? ? ?Ή?΄? λ§λ€κ³?
            ?Έ?€ λ²νΈ, ? κ·Όν  ? ?? id λ₯? λ¦¬ν΄??€
        
        λ§λ  GL program ? μ»΄ν?Ό ? ?°?΄??€? ?©μΉ? κ²μ΄?€
     */
    m_program = glCreateProgram();
    
    for (auto& shader: shaders) // ?? ₯λ°μ?? λͺ¨λ  gl shaders ? ????΄?
        /* 
            glAttachShader
                GL program object ? μ»΄ν?Ό ? ?¬?¬ κ°μ GL shader objects λ₯? λΆμΈ?€
                program = shader ?¬?¬ κ°κ?? ?°κ²°λ ??΄??Ό?Έ
        */
        glAttachShader(m_program, shader->Get());


    /* 
        glLinkProgram
            μ»΄ν?Ό ? GL shader objects λ₯? λ§ν¬
            shader ?€? ?λ‘? ?°κ²°ν?€ => ?¨? ? gl program ??±
     */
    glLinkProgram(m_program);


    // Link ?±κ³? ?¬λΆ? ??Έ
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