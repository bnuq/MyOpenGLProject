#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)
class Texture
{
public:
    /*
        �ؽ�ó�� ����� ��� 2����
        1. �׳� ����ų�
        2. �̹����� ���ؼ� ����ų�
     */
    static TextureUPtr Create(int width, int height, uint32_t format, uint32_t type = GL_UNSIGNED_BYTE);
    static TextureUPtr CreateFromImage(const Image* image);

    ~Texture();

    void Bind() const;

    /* Getter, Setter */
    const uint32_t Get() const { return m_texture; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    uint32_t GetType() const { return m_type; }
    uint32_t GetFormat() const { return m_format; }


    /* Setter */
    /* 
        �ؽ�ó�� �𵨿� ������ ��
        �ؽ�ó�� ���� ũ�� ���̰� �ʹ� ���� ���� ��� -> ��� ó���� ���ΰ�

        �̶� ����ϴ� �� Texture Filtering
        Texture Filtering Setting
     */
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    /* 
        �ؽ�ó�� �𵨿� ������ ��
        �� ������ �ؽ�ó ��ǥ��, [0,1] �� ����� ���
        �ؽ�ó ��ǥ�� �����Ǵ� �ؽ�ó�� ���� -> ��� ó���� ���ΰ�

        �̶� ����ϴ� �� Texture Wrapping
        Texture Wrapping Setting
     */
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;
    void SetTextureFormat(int width, int height, uint32_t format, uint32_t type);
    void SetBorderColor(const glm::vec4& color) const;

private:
    Texture() {}
    // �ʱ�ȭ �Լ� => ������ ����
    void CreateTexture();
    // �̹��� ������ => �ؽ�ó�� ����
    void SetTextureFromImage(const Image* image);


    uint32_t m_texture { 0 };               // gl texture obj id
    int m_width { 0 };                      // texture width
    int m_height { 0 };                     // texture height
    uint32_t m_format { GL_RGBA };          // texel �� �����ϴ� ���� => ���� ���� channel �� �̷���� �ִ�
    uint32_t m_type { GL_UNSIGNED_BYTE };   // �� ä���� �����ϴ� data type
};




CLASS_PTR(CubeTexture)
class CubeTexture
{
public:
    static CubeTextureUPtr CreateFromImages(const std::vector<Image*>& images);
    ~CubeTexture();

    void Bind() const;

    const uint32_t Get() const { return m_texture; }

private:
    CubeTexture() {}
    bool InitFromImages(const std::vector<Image*>& images);

    uint32_t m_texture { 0 };       // cube map �� cube texture ��� gl texture �� �����Ѵ�
};

#endif // __TEXTURE_H__