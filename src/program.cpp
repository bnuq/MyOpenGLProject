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
    Vertex Shader ??? Fragment Shader ?ŒŒ?¼ ?´ë¦„ë§Œ ë°›ê³ 
    Shader -> GL shader ?ƒ?„±
    ?´?›„
    Program -> GL program ?ƒ?„±
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
    shader ?˜ uniform ë³??ˆ˜?— ê°’ì„ ? „?‹¬?•˜?Š” ?•¨?ˆ˜

    Uniform ë³??ˆ˜ë¥? ?„£?„ ?•Œ, ?–´?–¤ ?…°?´?” ?ŒŒ?¼?— ?„£?Š” ì§??Š” ì¤‘ìš”?•˜ì§? ?•Š?‹¤
    GL program ?‹¨?œ„ë¡? ê´?ë¦¬ë˜ê¸? ?•Œë¬¸ì—, ?”„ë¡œê·¸?¨ ?‚´?— ?…°?´?”ê°? ì¡´ì¬?•˜?Š” ì§?ë¥? ?ŒŒ?•…?•´?•¼ ?•œ?‹¤

    GL program ?— ?‚¬?š©?•˜? ¤?Š” uniform ë³??ˆ˜?˜ ?´ë¦„ìœ¼ë¡? ? ‘ê·?
    ?•´?‹¹ uniform ?˜ ?œ„ì¹˜ë?? ?•Œ?•„?‚´ê³?
    ?œ„ì¹˜ì— ê°’ì„ ì§‘ì–´ ?„£?Š”?‹¤
 */
void Program::SetUniform(const std::string& name, int value) const
{
    // ì£¼ì–´ì§? ?´ë¦„ì„ ê°?ì§??Š” Uniform ë³??ˆ˜?˜ ?œ„ì¹˜ë?? ?ŒŒ?•…?•˜ê³?
    auto loc = glGetUniformLocation(m_program, name.c_str());
    // ?•´?‹¹ ?œ„ì¹˜ì— ê°’ì„ ì§‘ì–´ ?„£?Š”?‹¤
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
            OpenGL program object ë¥? ë¦¬ì†Œ?Š¤?— ?• ?‹¹?•´?„œ ë§Œë“¤ê³?
            ?•¸?“¤ ë²ˆí˜¸, ? ‘ê·¼í•  ?ˆ˜ ?ˆ?Š” id ë¥? ë¦¬í„´?•œ?‹¤
        
        ë§Œë“  GL program ?— ì»´íŒŒ?¼ ?œ ?…°?´?”?“¤?„ ?•©ì¹? ê²ƒì´?‹¤
     */
    m_program = glCreateProgram();
    
    for (auto& shader: shaders) // ?…? ¥ë°›ì?? ëª¨ë“  gl shaders ?— ????•´?„œ
        /* 
            glAttachShader
                GL program object ?— ì»´íŒŒ?¼ ?œ ?—¬?Ÿ¬ ê°œì˜ GL shader objects ë¥? ë¶™ì¸?‹¤
                program = shader ?—¬?Ÿ¬ ê°œê?? ?—°ê²°ëœ ?ŒŒ?´?”„?¼?¸
        */
        glAttachShader(m_program, shader->Get());


    /* 
        glLinkProgram
            ì»´íŒŒ?¼ ?œ GL shader objects ë¥? ë§í¬
            shader ?“¤?„ ?„œë¡? ?—°ê²°í•œ?‹¤ => ?˜¨? „?•œ gl program ?ƒ?„±
     */
    glLinkProgram(m_program);


    // Link ?„±ê³? ?—¬ë¶? ?™•?¸
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