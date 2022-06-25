#include "image.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

/* PUBLIC */
ImageUPtr Image::Load(const std::string& filepath, bool flipVertical)
{
    auto image = ImageUPtr(new Image());
    if (!image->LoadWithStb(filepath, flipVertical))
        return nullptr;
    return std::move(image);
}

ImageUPtr Image::Create(int width, int height, int channelCount)
{
    auto image = ImageUPtr(new Image());
    if (!image->Allocate(width, height, channelCount))
        return nullptr;
    return std::move(image);
}

ImageUPtr Image::CreateSingleColorImage(int width, int height, const glm::vec4& color)
{
    glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f); // 0-1 κ°μ 0-255 κ°μΌλ‘? λ³?κ²?
    uint8_t rgba[4] =
    {
        (uint8_t)clamped.r,
        (uint8_t)clamped.g, 
        (uint8_t)clamped.b, 
        (uint8_t)clamped.a, 
    };
    auto image = Create(width, height, 4);
    for (int i = 0; i < width * height; i++)
    {
        memcpy(image->m_data + 4 * i, rgba, 4);
    }
    return std::move(image);
}

// ?΄λ―Έμ?? λ©λͺ¨λ¦¬λ?? μ§μ  ? ?Ή? ?, Check Image λ₯? μ§μ  λ§λ? ?¨?
void Image::SetCheckImage(int gridX, int gridY)
{
    for (int j = 0; j < m_height; j++)
    {
        for (int i = 0; i < m_width; i++)
        {
            int pos = (j * m_height + i) * m_channelCount;          // ??? ??????? m_channelCount λ§νΌ ?¬κΈ°λ?? μ°¨μ????€
            bool even = ((i / gridX) + (j / gridY)) % 2 == 0;       // κ²©μ
            uint8_t value = even ? 255 : 0;                         // ?°? or κ²?????
            
            for (int k = 0; k < m_channelCount; k++)
                m_data[pos + k] = value;                            // ????? (1,1,1) ? ?£κ±°λ (0,0,0) ? ?£??€

            if (m_channelCount > 3) m_data[3] = 255;                // ?? κ°μ΄ ?°λ‘? ??Όλ©?, λΆν¬λͺνκ²? ?€? 
        }
    }
}

Image::~Image()
{
    if (m_data)
    {
        stbi_image_free(m_data);
    }
}

/* PRIVATE */
// ?ΈλΆ? ?Ό?΄λΈλ¬λ¦¬μΈ stb λ₯? ??©?΄? ?΄λ―Έμ?? ??Ό? λ‘λ?? ?¨?
bool Image::LoadWithStb(const std::string& filepath, bool flipVertical)
{
    /* 
        ?΄λ―Έμ??? μ’μ?¨?΄ ?? ?Έ?°, opengl ??? μ’ν?¨?΄ ?? ?΄?Ό κΈ°μ??? ?΄?€λ₯΄λ€
            ?΄λ―Έμ??κ°? ?€μ§μ΄? Έ? λ³΄μ
        ?΄λ―Έμ?? λ‘λ© ? , ?€μ§μ μ§? ?¬λΆ?λ₯? ??Έ??€
     */
    stbi_set_flip_vertically_on_load(flipVertical);
    /* 
        ?΄λ―Έμ?? ??Ό? λ‘λ© => ?΄λ―Έμ?? ??Ό? κ°? ?????΄ κ°?μ§?κ³? ?? RGB κ°λ€?΄ 1μ°¨μ λ°°μ΄ ??λ‘? ????₯??΄ ???°
                            κ·? ?΄λ―Έμ?? ??Ό ?°?΄?° λ°°μ΄? CPU λ©λͺ¨λ¦¬μ ????₯?κ³?
                            κ°?λ¦¬ν€? μ£Όμλ₯? λ¦¬ν΄ == m_data
        ? ????? r,g,b κ°κ° 8λΉνΈ μ°¨μ?? => κ·Έλ? m_data κ°? 8λΉνΈλ₯? κ°?λ¦¬ν€? ?¬?Έ?°
        ?΄λ―Έμ??λ₯? λ‘λ©?λ©΄μ, ?΄λ―Έμ??? ? λ³΄λ€? ???Όλ‘? ?»?΄?Ό ? ??€
            λ©€λ² λ³??? ????₯
     */
    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if (!m_data)
    {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }
    return true;
}

bool Image::Allocate(int width, int height, int channelCount)
{
    m_width = width;
    m_height = height;
    m_channelCount = channelCount;  // ? ?½???? ?΄λ£¨λ ?°?΄?°? ???? ? λ³?
    // μ§μ  Heap λ©λͺ¨λ¦? ? ?Ή
    // R or G κ°μ?? ??? ?? = 8λΉνΈ = signed char = 1 byte
    m_data = (uint8_t*)malloc(m_width * m_height * m_channelCount);
    return m_data ? true : false;
}