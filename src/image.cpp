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
    glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f);
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

// ?��미�?? 메모리�?? 직접 ?��?��?�� ?��, Check Image �? 직접 만드?�� ?��?��
void Image::SetCheckImage(int gridX, int gridY)
{
    for (int j = 0; j < m_height; j++)
    {
        for (int i = 0; i < m_width; i++)
        {
            int pos = (j * m_height + i) * m_channelCount;          // ?��?��?�� ?��?????? m_channelCount 만큼 ?��기�?? 차�???��?��
            bool even = ((i / gridX) + (j / gridY)) % 2 == 0;       // 격자
            uint8_t value = even ? 255 : 0;                         // ?��?�� or �?????��
            
            for (int k = 0; k < m_channelCount; k++)
                m_data[pos + k] = value;                            // ?��????�� (1,1,1) ?�� ?��거나 (0,0,0) ?�� ?��?��?��

            if (m_channelCount > 3) m_data[3] = 255;                // ?��?�� 값이 ?���? ?��?���?, 불투명하�? ?��?��
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
// ?���? ?��?��브러리인 stb �? ?��?��?��?�� ?��미�?? ?��?��?�� 로드?��?�� ?��?��
bool Image::LoadWithStb(const std::string& filepath, bool flipVertical)
{
    /* 
        ?��미�???�� 좌상?��?�� ?��?��?��?��, opengl ??? 좌하?��?�� ?��?��?��?�� 기�???��?��?��르다
            ?��미�??�? ?��집어?��?�� 보임
        ?��미�?? 로딩 ?��, ?��집을 �? ?���?�? ?��?��?��?��
     */
    stbi_set_flip_vertically_on_load(flipVertical);
    /* 
        ?��미�?? ?��?��?�� 로딩 => ?��미�?? ?��?��?�� �? ?��????�� �?�?�? ?��?�� RGB 값들?�� 1차원 배열 ?��?���? ????��?��?�� ?��?��?��
                            �? ?��미�?? ?��?�� ?��?��?�� 배열?�� CPU 메모리에 ????��?���?
                            �?리키?�� 주소�? 리턴 == m_data
        ?�� ?��????�� r,g,b 각각 8비트 차�?? => 그래?�� m_data �? 8비트�? �?리키?�� ?��?��?��
        ?��미�??�? 로딩?��면서, ?��미�???�� ?��보들?�� ?��?��?���? ?��?��?�� ?�� ?��?��
            멤버 �??��?�� ????��
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
    m_channelCount = channelCount;  // ?�� ?��????�� ?��루는 ?��?��?��?�� ????�� ?���?
    // 직접 Heap 메모�? ?��?��
    // R or G 같�?? ?��?��?�� ?��?�� = 8비트 = signed char = 1 byte
    m_data = (uint8_t*)malloc(m_width * m_height * m_channelCount);
    return m_data ? true : false;
}