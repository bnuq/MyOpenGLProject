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
    Vertex Shader ??? Fragment Shader ?åå?ùº ?ù¥Î¶ÑÎßå Î∞õÍ≥†
    Shader -> GL shader ?Éù?Ñ±
    ?ù¥?õÑ
    Program -> GL program ?Éù?Ñ±
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
    shader ?ùò uniform Î≥??àò?óê Í∞íÏùÑ ?†Ñ?ã¨?ïò?äî ?ï®?àò

    Uniform Î≥??àòÎ•? ?Ñ£?ùÑ ?ïå, ?ñ¥?ñ§ ?Ö∞?ù¥?çî ?åå?ùº?óê ?Ñ£?äî Ïß??äî Ï§ëÏöî?ïòÏß? ?ïä?ã§
    GL program ?ã®?úÑÎ°? Í¥?Î¶¨ÎêòÍ∏? ?ïåÎ¨∏Ïóê, ?îÑÎ°úÍ∑∏?û® ?Ç¥?óê ?Ö∞?ù¥?çîÍ∞? Ï°¥Ïû¨?ïò?äî Ïß?Î•? ?åå?ïÖ?ï¥?ïº ?ïú?ã§

    GL program ?óê ?Ç¨?ö©?ïò?†§?äî uniform Î≥??àò?ùò ?ù¥Î¶ÑÏúºÎ°? ?†ëÍ∑?
    ?ï¥?ãπ uniform ?ùò ?úÑÏπòÎ?? ?ïå?ïÑ?Ç¥Í≥?
    ?úÑÏπòÏóê Í∞íÏùÑ ÏßëÏñ¥ ?Ñ£?äî?ã§
 */
void Program::SetUniform(const std::string& name, int value) const
{
    // Ï£ºÏñ¥Ïß? ?ù¥Î¶ÑÏùÑ Í∞?Ïß??äî Uniform Î≥??àò?ùò ?úÑÏπòÎ?? ?åå?ïÖ?ïòÍ≥?
    auto loc = glGetUniformLocation(m_program, name.c_str());
    // ?ï¥?ãπ ?úÑÏπòÏóê Í∞íÏùÑ ÏßëÏñ¥ ?Ñ£?äî?ã§
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
    
    // Î≤°ÌÑ∞?Çò ?ñâ?†¨?ùÑ ?Ñ£?ùÑ ?ïå?äî Í∞íÏùÑ Î≥µÏÇ¨?ïòÏß? ?ïäÍ≥? ?è¨?ù∏?Ñ∞Î•? ?ÑòÍ∏¥Îã§ => value_ptr ?Ç¨?ö©
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
            OpenGL program object Î•? Î¶¨ÏÜå?ä§?óê ?ï†?ãπ?ï¥?Ñú ÎßåÎì§Í≥?
            ?ï∏?ì§ Î≤àÌò∏, ?†ëÍ∑ºÌï† ?àò ?ûà?äî id Î•? Î¶¨ÌÑ¥?ïú?ã§
        
        ÎßåÎì† GL program ?óê Ïª¥Ìåå?ùº ?êú ?Ö∞?ù¥?çî?ì§?ùÑ ?ï©Ïπ? Í≤ÉÏù¥?ã§
     */
    m_program = glCreateProgram();
    
    for (auto& shader: shaders) // ?ûÖ?†•Î∞õÏ?? Î™®Îì† gl shaders ?óê ????ï¥?Ñú
        /* 
            glAttachShader
                GL program object ?óê Ïª¥Ìåå?ùº ?êú ?ó¨?ü¨ Í∞úÏùò GL shader objects Î•? Î∂ôÏù∏?ã§
                program = shader ?ó¨?ü¨ Í∞úÍ?? ?ó∞Í≤∞Îêú ?åå?ù¥?îÑ?ùº?ù∏
        */
        glAttachShader(m_program, shader->Get());


    /* 
        glLinkProgram
            Ïª¥Ìåå?ùº ?êú GL shader objects Î•? ÎßÅÌÅ¨
            shader ?ì§?ùÑ ?ÑúÎ°? ?ó∞Í≤∞Ìïú?ã§ => ?ò®?†Ñ?ïú gl program ?Éù?Ñ±
     */
    glLinkProgram(m_program);


    // Link ?Ñ±Í≥? ?ó¨Î∂? ?ôï?ù∏
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