#include "texture.h"

TextureUPtr Texture::Create(int width, int height, uint32_t format, uint32_t type)
{
    auto texture = TextureUPtr(new Texture());
    
    texture->CreateTexture();
    texture->SetTextureFormat(width, height, format, type);

    // �̹��� ���� texture �� ����� ��� => mipmap �� ����
    texture->SetFilter(GL_LINEAR, GL_LINEAR);

    return std::move(texture);
}


TextureUPtr Texture::CreateFromImage(const Image* image)
{
    auto texture = TextureUPtr(new Texture());

    texture->CreateTexture();
    texture->SetTextureFromImage(image);

    return std::move(texture);
}


void Texture::CreateTexture()
{
    glGenTextures(1, &m_texture);
    Bind();
        /* �ش� �ؽ�ó�� �𵨿� ������ �� �ʿ��� �������� �����Ѵ� */
        /*
            1. �ؽ�ó�� ���� ���� ũ�Ⱑ ������ �ٸ� �� ~ �� ����ؼ�
            Texture Filtering ������ �����Ѵ�
        */
        SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
        /* 
            2. ���� ���� ���� �� ������ �ؽ�ó ��ǥ�� [0, 1] �� ����� ��츦 ����ؼ�
            Texture Wrapping ������ �����Ѵ�
        */
        SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}


Texture::~Texture()
{
    if (m_texture)
    {
        // gl texture obj ���ҽ� ����
        glDeleteTextures(1, &m_texture);
    }
}


void Texture::Bind() const
{
    /* 
        ���ݺ��� ���� ���, �ش� �ؽ�ó�� ����ϰڴٰ� ����

        �ش� �ؽ�ó�� ������ ��, GL texture handle id �� �ƴ� �뵵�� ���ص�
        ������ �ش� �ؽ�ó�� ����ϰ� �ȴ�

        ���⼭��, ���ݺ��� m_texture �� GL_TEXTURE_2D �� ����ϰڴ�~ �� ��
            m_texture �� ������ �ؽ�ó => GL_TEXTURE_2D ��� �̸����� ����ϰڴ�
     */
    glBindTexture(GL_TEXTURE_2D, m_texture);
}


void Texture::SetFilter(uint32_t minFilter, uint32_t magFilter) const
{
    /* 
        1. Texture Filtering ����
        �ؽ�ó�� �����ؾ� �ϴ� mesh �� ũ�� ���̰� �� ��, ��� �ؽ�ó���� �� ���ΰ�
        ���� GL_TEXTURE_2D �� ���ؼ� Bind �� GL_TEXTURE_2D �ؽ�ó�� ����
            GL_TEXTURE_MIN_FILTER
                �ؽ�ó�� �޽��� ���ؼ� �ʹ� ũ��, �ؽ�ó�� �ٿ��� �ϴ� ���

                [ MipMapping �� ������� �ʴ� ��� ]
                GL_NEAREST
                    ������ level 0 �ؽ�ó����, ������ �ؽ�ó ��ǥ���� ���� ����� �ؼ� ���� ������
                GL_NEAREST
                    ������ level 0 �ؽ�ó����, ���ߺ����� ���� �ֺ� �ؼ��� �߰� ���� ������

                [ MipMapping �� ����ϴ� ��� ]
                    1. ������ ���� ������ �ؼ��� ��� ������ �� 
                    2. MipMap ������ ��� level �� ������ ��
                    ��� �����Ѵ�
                GL_NEAREST_MIPMAP_NEAREST
                                  �׸����� �޽��� ũ�Ⱑ ���� ����� ������ ������ ��
                    �ش� �������� ���� ����� �ؼ��� �����´�
                GL_LINEAR_MIPMAP_NEAREST
                                  �׸����� �޽��� ũ�Ⱑ ���� ����� ������ ������ ��
                    �ش� �������� �ؼ��� linear ����� ���� ���� �����´�

                GL_LINEAR_MIPMAP_LINEAR
                                 �ؽ�ó�� �Ϸ��� �޽��� ũ�Ⱑ ���� ����� ������ 2�� �����Ѵ�
                   �� �������� linear ����� ���� ���� ������ �� (�� �������� ���ߺ���)
                                 �� �������� ���� ����� �ٽ� linear ���� �� ���� ���� �����Ѵ�
                                 = Trilinear Interpolation, ���� ����

                GL_NEAREST_MIPMAP_LINEAR
                                  �ؽ�ó�� �Ϸ��� �޽��� ��Ű�� ���� ����� ������ 2�� �����Ѵ�
                    �� �������� ������ �ؽ�ó ��ǥ�� ���� ����� �ؼ��� �����ϰ�
                                  �� ���� ����� linear ���� �� ���� ���� �����Ѵ�

            GL_TEXTURE_MAG_FILTER
                �ؽ�ó�� �޽��� ���ؼ� �ʹ� �۾�, �ؽ�ó�� Ȯ���ؾ� �ϴ� ���
                GL_NEAREST 
                    ������ �ؽ�ó ��ǥ����, �׳��� ���� �����̿� �ִ� �ؼ��� ������ �����´�
                GL_LINEAR
                    ������ �ؽ�ó ��ǥ�� �̿�, ���������� ���ؼ� �ֺ� �ؼ����� �߰� ���� �����´�
                    ���� ����
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}


void Texture::SetWrap(uint32_t sWrap, uint32_t tWrap) const
{
    /* 
        Texture Wrapping ����
        �ؽ�ó ��ǥ�� 0~1 �� ��� ��, ��� ó���� ���ΰ�
        < ������ ���� >
            GL_REPEAT
            GL_MIRRORED_REPEAT
            GL_CLAMP_TO_EDGE
            GL_CLAMP_TO_BORDER

        ���� Bind �� GL_TEXTURE_2D �ؽ�ó�� ����
            GL_TEXTURE_WRAP_S
                �ؽ�ó�� ���� ����, S �࿡ ���ؼ� �ؽ�ó ��ǥ��
                ������ ����� ��� ��� ó���� ������ ����
            GL_TEXTURE_WRAP_T
                �ؽ�ó�� ���� ����, T �࿡ ���ؼ� �ؽ�ó ��ǥ�� 
                ������ ����� ��� ��� ó���� ������ ����
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}


// gl Texture �� ����⸸ �ϰ�, ���� �̹��� �����ʹ� ����
void Texture::SetTextureFormat(int width, int height, uint32_t format, uint32_t type)
{
    m_width = width;
    m_height = height;
    m_format = format;
    m_type = type;

    glTexImage2D                // specify a two-dimensional texture image
    (
        GL_TEXTURE_2D,          // target
        // texture data
        0,                      // level
        m_format,               // inter format => �ؼ��� ����Ǵ� ������ ����
        m_width,
        m_height,

        0,                      // boarder, must be zero

        m_format,               // image data �� ����, ���⼭�� �����ϱ� ������ ���˰� �����ϰ� ����
        m_type,                 // ������ �� ä�ο� �Ҵ�Ǵ� data type
        // format * type => �� �ؼ��� ����� �����ȴ�, ���⼭�� �̹����� ���� ������
        // Ÿ���� �����Ѵ�� �ؼ��� ũ�Ⱑ �Ҵ�� ���̴�
        
        nullptr                 // data �� ����, �޸𸮸� �Ҵ��ϰ� Copy �� ���� �ʴ´�
    );
}


// �̹��� �����͸�, GL texture �� ä���
void Texture::SetTextureFromImage(const Image* image)
{
    // �⺻ channel count
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount())
    {
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
        default: break;
    }

    m_width = image->GetWidth();
    m_height = image->GetHeight();
    m_format = format;
    m_type = GL_UNSIGNED_BYTE;     // �̹��� ������ ���, �ȼ��� ��� �����ʹ� ������ UNSIGNED_BYTE type �̴�

    // gl Texture �� �޸𸮸� �Ҵ��Ѵ�
    glTexImage2D
    (
        GL_TEXTURE_2D, 
        0,              // Target, texture level
        // GPU ��, �޴� GL texture obj ����
        m_format,            // internal format ~ �̹����� ������ �ؽ�ó�� � �������� ������ ���ΰ�
        m_width,
        m_height,

        0,                  // boarder

        // CPU ��, �ִ� �̹��� �������� ����
        format,             // format
        m_type,             // type
        image->GetData()    // *data
    );
    // �̹����� ���� �ؽ�ó�� ����� ��쿡��, mipmap �� �����
    glGenerateMipmap(GL_TEXTURE_2D);
}


void Texture::SetBorderColor(const glm::vec4& color) const
{
    // texture �� boader color �� ����
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
}



/* ***************** */



// ť�� �� = 2D �̹��� * 6��
CubeTextureUPtr CubeTexture::CreateFromImages(const std::vector<Image*>& images)
{
    auto texture = CubeTextureUPtr(new CubeTexture());
    if (!texture->InitFromImages(images))
        return nullptr;
    return std::move(texture);
}


CubeTexture::~CubeTexture()
{
    if (m_texture)
    {
        glDeleteTextures(1, &m_texture);
    }
}


void CubeTexture::Bind() const
{
    // ���� �����ϴ� �ؽ�ó�� CUBE MAP ���� ����ϰڴ�
    // GL ���� �����ϴ� cube map texture �� ���� �����Ѵ�
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
}


bool CubeTexture::InitFromImages(const std::vector<Image*>& images)
{
    glGenTextures(1, &m_texture);
    Bind();

        // �ؽ�ó �Ķ���� ����
        // Filtering
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Wrapping
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


        // 6���� �̹����� ����
        for (uint32_t i = 0; i < (uint32_t)images.size(); i++)
        {
            auto image = images[i];
            GLenum format = GL_RGBA;
            switch (image->GetChannelCount())
            {
                default: break;
                case 1: format = GL_RED; break;
                case 2: format = GL_RG; break;
                case 3: format = GL_RGB; break;
            }


            // cube map texture ����
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,     // GL_TEXTURE_CUBE_MAP �� 6 ���� �����Ǿ� �ִ�
                                                        // bind �� �ѹ��� �ϰ� 6 ���� ���� ����
                0, 
                GL_RGB,                     // RGB �����͸� ����
                image->GetWidth(), 
                image->GetHeight(),
                
                0,

                format,
                GL_UNSIGNED_BYTE,           // �̹��� �ȼ� => �� ä�� �����ʹ� �׻� unsigned byte �� ����ִ�

                image->GetData()
            );
        }


    return true;
}