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
    Vertex Shader �� Fragment Shader ���� �̸��� �ް�
    Shader -> GL shader ����
    ����
    Program -> GL program ����
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
    shader �� uniform ������ ���� �����ϴ� �Լ�

    Uniform ������ ���� ��, � ���̴� ���Ͽ� �ִ� ���� �߿����� �ʴ�
    GL program ������ �����Ǳ� ������, ���α׷� ���� ���̴��� �����ϴ� ���� �ľ��ؾ� �Ѵ�

    GL program �� ����Ϸ��� uniform ������ �̸����� ����
    �ش� uniform �� ��ġ�� �˾Ƴ���
    ��ġ�� ���� ���� �ִ´�
 */
void Program::SetUniform(const std::string& name, int value) const
{
    // �־��� �̸��� ������ Uniform ������ ��ġ�� �ľ��ϰ�
    auto loc = glGetUniformLocation(m_program, name.c_str());
    // �ش� ��ġ�� ���� ���� �ִ´�
    glUniform1i(loc, value);
}

void Program::SetUniform(const std::string& name, unsigned int value) const
{
    auto loc = glGetUniformLocation(m_program, name.c_str());
    glUniform1ui(loc, value);
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
    
    // ���ͳ� ����� ���� ���� ���� �������� �ʰ� �����͸� �ѱ�� => value_ptr ���
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
            OpenGL program object �� ���ҽ��� �Ҵ��ؼ� �����
            �ڵ� ��ȣ, ������ �� �ִ� id �� �����Ѵ�
        
        ���� GL program �� ������ �� ���̴����� ��ĥ ���̴�
     */
    m_program = glCreateProgram();
    
    for (auto& shader: shaders) // �Է¹��� ��� gl shaders �� ���ؼ�
        /* 
            glAttachShader
                GL program object �� ������ �� ���� ���� GL shader objects �� ���δ�
                program = shader ���� ���� ����� ����������
        */
        glAttachShader(m_program, shader->Get());


    /* 
        glLinkProgram
            ������ �� GL shader objects �� ��ũ
            shader ���� ���� �����Ѵ� => ������ gl program ����
     */
    glLinkProgram(m_program);


    // Link ���� ���� Ȯ��
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