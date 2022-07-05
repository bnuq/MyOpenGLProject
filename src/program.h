#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "common.h"
#include "shader.h"

/* 
    Program ?΄??€
        GL program object λ₯? κ΄?λ¦?
    
    GL program
        ?¬?¬ κ°μ μ»΄ν?Ό ? shader ?€? ?©μ³λ??? κ²?
        programmable, ?΄κ°? ?? ?  ? ?? ?°?΄??€? λͺ¨μ???€
        ?΄?€ ?°?΄?λ₯? ?¬?©?  μ§? κ²°μ ?  ? ??€
        ?°?΄??€? ?€?΄κ°?? Uniform κ΄?λ¦?
 */
CLASS_PTR(Program)
class Program
{
public:
    static ProgramUPtr Create(const std::vector<ShaderPtr>& shaders);
    static ProgramUPtr Create(const std::string& vertShaderFilename, const std::string& fragShaderFilename);
    ~Program();
    /* 
        κ΄?λ¦¬νκ³? ?? GL program ? ?¬?©?κ³? ?Ά? ? ?¬?©
            GL program ?΄?Ό? κ²μ??, μ»΄ν?Ό ? ?°?΄??€? λͺ¨μ???? κ²?
            μ¦?, ?΄κ°? ??±? ?΄?€ ?°?΄??€? ?¬?©?κ³? ?Ά? ? ?ΈμΆ?
     */    
    void Use() const;

    // Return GL program handle id
    uint32_t Get() const { return m_program; }
    /* 
        shader ? uniform λ³??λ₯? ?κΈ°λ ?¨??€
        ?κΈ°λ Uniform λ³??? λ§κ² function overloading

        Uniform λ³??? ?΄λ¦? λ¬Έμ?΄? ?΅?΄? ? κ·Όν?€
     */
    void SetUniform(const std::string& name, unsigned int value) const;
    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, bool value) const;
    void SetUniform(const std::string& name, float value) const;

    void SetUniform(const std::string& name, const glm::vec2& value) const;
    void SetUniform(const std::string& name, const glm::vec3& value) const;
    void SetUniform(const std::string& name, const glm::vec4& value) const;

    void SetUniform(const std::string& name, const glm::mat4& value) const;

private:
    Program() {}
    // compile ? GL shader ?€? Link => ?¨? ? GL program ? λ§λ ?€
    bool Link(const std::vector<ShaderPtr>& shaders);

    // Program Object id
    uint32_t m_program { 0 };
};

#endif // __PROGRAM_H__