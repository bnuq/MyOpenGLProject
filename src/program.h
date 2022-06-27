#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "common.h"
#include "shader.h"

/* 
    Program ?��?��?��
        GL program object �? �?�?
    
    GL program
        ?��?�� 개의 컴파?�� ?�� shader ?��?�� ?��쳐놓??? �?
        programmable, ?���? ?��?��?�� ?�� ?��?�� ?��?��?��?��?�� 모아?��?��?��
        ?��?�� ?��?��?���? ?��?��?�� �? 결정?�� ?�� ?��?��
        ?��?��?��?��?�� ?��?���??�� Uniform �?�?
 */
CLASS_PTR(Program)
class Program
{
public:
    static ProgramUPtr Create(const std::vector<ShaderPtr>& shaders);
    static ProgramUPtr Create(const std::string& vertShaderFilename, const std::string& fragShaderFilename);
    ~Program();
    /* 
        �?리하�? ?��?�� GL program ?�� ?��?��?���? ?��?�� ?�� ?��?��
            GL program ?��?��?�� 것�??, 컴파?�� ?�� ?��?��?��?��?�� 모아?��??? �?
            �?, ?���? ?��?��?�� ?��?�� ?��?��?��?��?�� ?��?��?���? ?��?�� ?�� ?���?
     */    
    void Use() const;

    // Return GL program handle id
    uint32_t Get() const { return m_program; }
    /* 
        shader ?�� uniform �??���? ?��기는 ?��?��?��
        ?��기는 Uniform �??��?�� 맞게 function overloading

        Uniform �??��?�� ?���? 문자?��?�� ?��?��?�� ?��근한?��
     */
    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, bool value) const;
    void SetUniform(const std::string& name, float value) const;

    void SetUniform(const std::string& name, const glm::vec2& value) const;
    void SetUniform(const std::string& name, const glm::vec3& value) const;
    void SetUniform(const std::string& name, const glm::vec4& value) const;

    void SetUniform(const std::string& name, const glm::mat4& value) const;

private:
    Program() {}
    // compile ?�� GL shader ?��?�� Link => ?��?��?�� GL program ?�� 만든?��
    bool Link(const std::vector<ShaderPtr>& shaders);

    // Program Object id
    uint32_t m_program { 0 };
};

#endif // __PROGRAM_H__