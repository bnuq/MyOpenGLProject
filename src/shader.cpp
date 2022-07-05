#include "shader.h"

/* PUBLIC */
ShaderUPtr Shader::CreateFromFile(const std::string& filename, GLenum shaderType)
{
    auto shader = std::unique_ptr<Shader>(new Shader());
    if (!shader->LoadFile(filename, shaderType))
        return nullptr;
    return std::move(shader);
}

Shader::~Shader()
{
    if(m_shader)
    {
        glDeleteShader(m_shader);
    }
}

/* PRIVATE */
bool Shader::LoadFile(const std::string& filename, GLenum shaderType)
{
    auto result = LoadTextFile(filename);
    // optional::has_value() => ���� �����ϴ� ��, �ƴ� ���� �Ǵ��ϴ� �޼���
    if (!result.has_value()) // ���� ���� == ������ ���� ���� ���
        return false;
    // optional::value() => optional �� ���ΰ� �ִ� �� ����
    // Text ���� �� �ۼ��� �ڵ�
    auto& code = result.value();
    /*
        string::c_str() => string �� �����ϰ� �ִ� ���ڿ� �ּ� ����
        string => c-style string ��ȯ
        GLshader ���� c-style ���·� ����ȴ�
     */
    const char* codePtr = code.c_str();             // file �� �ڵ� ���ڿ� �ּ�
    int32_t codeLength = (int32_t)code.length();    // file �� �ڵ� ����
    /* 
        gl shader obj �� ����

        glCreateShader
            GL shader object �� ����
            �ڵ� ��ȣ�� ����
            
        shader type �� ���ؼ� � ������ ���̴��� ����� �������� �˸���
            Vertex Shader
            Fragment Shader
            Compute Shader 
            ��� GLshader �� ������ �����ϰ�, �����Ѵ� 
     */
    m_shader = glCreateShader(shaderType);
    /* 
        gl shader �ȿ� �ҽ��ڵ带 �ִ´�

        glShaderSource
            GL shader object �� shader code �� ���� �ִ´�
            1�� �̻��� �ڵ带 ���� �� �ִ�, ���� ����
            �ڵ�� �ڵ� ���� ����

        <����>
            const �� ���ʿ� ����ȴ�
            ���ʿ� �ƹ��͵� ���ٸ� �����ʿ� ����ȴ�
            ���� const GLchar* const* == const GLchar**
     */
    glShaderSource(m_shader, 1, (const GLchar**)&codePtr, &codeLength);
    // GLshader compile, ���̴� �ڵ带 ���� ����, �ٷ� �������� �����Ѵ�
    glCompileShader(m_shader);
   
    /*
        glGetShader|iv
            shader �� �ִ� ������ �����´�
            � ������ �˾Ƴ��� ���� ���� ����
            iv = integer pointer parameter
            �ѱ� ���� �����Ϳ� ������ �����ؼ� �����Ѵ�
     */
    int success = 0;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
    if (!success) // ������ ���� ��, ���� ���� Ȯ��
    {
        char infoLog[1024];

        // shader object �� ���� ������ �����´�
        glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);

        // speed log ���
        SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
        SPDLOG_ERROR("reason: {}", infoLog);
        return false;
    }


    SPDLOG_INFO("{} shader make, id = {}", filename, m_shader);
    return true;
}