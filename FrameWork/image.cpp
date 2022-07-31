#include "image.h"
/* 
    stb library �� �Լ� ���Ǵ� ������Ͽ� ������, ���� �ű⼭ ���ǵ��� �ʴ´�
        �ֳ��ϸ� ���Ǵ� ��� ���Ͽ� ������ �ȵǰ�, �ҽ� ���Ͽ� ���־�� �����ǵǴ� ���� ���� �� �����ϱ�

    stb library �Լ� ���Ǹ� �� �ҽ����Ͽ��� ������ ���ε�, ��ũ�θ� �̿��ؼ� ���� �ڵ带 �����´�
    �� ��ũ�ΰ� ���ǵǾ� ������, ������Ͽ��� ���� �ڵ� �κ��� Ȱ��ȭ�ȴ� => �Լ� ��� ����
    �� ��ũ�ΰ� ������ => ���� ���� + �ҽ� ���� ���� => ��Ŀ ����
 */
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
    glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f); // 0-1 ���� 0-255 ������ ����
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


void Image::SetCheckImage(int gridX, int gridY)
{
    for (int j = 0; j < m_height; j++)
    {
        for (int i = 0; i < m_width; i++)
        {
            int pos = (j * m_height + i) * m_channelCount;          // �ϳ��� �ؼ��� m_channelCount ��ŭ ũ�⸦ �����Ѵ�
            bool even = ((i / gridX) + (j / gridY)) % 2 == 0;       // ����
            uint8_t value = even ? 255 : 0;                         // ��� or ������
            
            for (int k = 0; k < m_channelCount; k++)
                m_data[pos + k] = value;                            // �ؼ��� (1,1,1) �� �ְų� (0,0,0) �� �ִ´�

            if (m_channelCount > 3) m_data[3] = 255;                // ���� ���� ���� ������, �������ϰ� ����
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
// �ܺ� ���̺귯���� stb �� Ȱ���ؼ� �̹��� ������ �ε��ϴ� �Լ�
bool Image::LoadWithStb(const std::string& filepath, bool flipVertical)
{
    /* 
        �̹����� �»���� �����ε�, opengl �� ���ϴ��� �����̶� �������̴ٸ���
            �̹����� ���������� ����
        �̹��� �ε� ��, ������ �� ���θ� Ȯ���Ѵ�
     */
    stbi_set_flip_vertically_on_load(flipVertical);
    /* 
        �̹��� ������ �ε� => �̹��� ������ �� �ؼ��� ������ �ִ� RGB ������ 1���� �迭 ���·� ����Ǿ� �ִµ�
                            �� �̹��� ���� ������ �迭�� CPU �޸𸮿� �����ϰ�
                            ����Ű�� �ּҸ� ���� == m_data
        �� �ؼ��� r,g,b ���� 8��Ʈ ���� => �׷��� m_data �� 8��Ʈ�� ����Ű�� ������
        �̹����� �ε��ϸ鼭, �̹����� �����鵵 �ڵ����� �� �� �ִ�
            ��� ������ ����
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
    m_channelCount = channelCount;
    
    // ���� Heap �޸� �Ҵ�
    // R or G ���� �ϳ��� ��� = 8��Ʈ = signed char = 1 byte
    m_data = (uint8_t*)malloc(m_width * m_height * m_channelCount);
    return m_data ? true : false;
}