#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "common.h"
#include "shader.h"

/* 
    Program Ŭ����
        GL program object �� ����
    
    GL program
        ���� ���� ������ �� shader ���� ���ĳ��� ��
        programmable, ���� ������ �� �ִ� ���̴����� ��Ƴ��Ҵ�
        � ���̴��� ����� �� ������ �� �ִ�
        ���̴��鿡 ���� Uniform ����
 */
CLASS_PTR(Program)
class Program
{
public:
    static ProgramUPtr Create(const std::vector<ShaderPtr>& shaders);
    static ProgramUPtr Create(const std::string& vertShaderFilename, const std::string& fragShaderFilename);
    ~Program();
    /* 
        �����ϰ� �ִ� GL program �� ����ϰ� ���� �� ���
            GL program �̶�� ����, ������ �� ���̴����� ��Ƴ��� ��
            ��, ���� �ۼ��� � ���̴����� ����ϰ� ���� �� ȣ��
     */    
    void Use() const;

    // Return GL program handle id
    uint32_t Get() const { return m_program; }
    /* 
        shader �� uniform ������ �ѱ�� �Լ���
        �ѱ�� Uniform ������ �°� function overloading

        Uniform ������ �̸� ���ڿ��� ���ؼ� �����Ѵ�
     */
    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, unsigned int value) const;
    void SetUniform(const std::string& name, float value) const;

    void SetUniform(const std::string& name, const glm::vec2& value) const;
    void SetUniform(const std::string& name, const glm::vec3& value) const;
    void SetUniform(const std::string& name, const glm::vec4& value) const;

    void SetUniform(const std::string& name, const glm::mat4& value) const;

private:
    Program() {}
    // compile �� GL shader ���� Link => ������ GL program �� �����
    bool Link(const std::vector<ShaderPtr>& shaders);

    // Program Object id
    uint32_t m_program { 0 };
};

#endif // __PROGRAM_H__