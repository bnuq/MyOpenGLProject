#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)
class Texture
{
public:
    static TextureUPtr Create(int width, int height, uint32_t format);
    static TextureUPtr CreateFromImage(const Image* image);
    void SetTextureFormat(int width, int height, uint32_t format);
    ~Texture();
    void Bind() const;

    const uint32_t Get() const { return m_texture; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    uint32_t GetFormat() const { return m_format; }

    /* 
        ?��?��처�?? 모델?�� ?��?��?�� ?��
        ?��?��처�?? 모델?�� ?���? 차이�? ?���? 많이 ?��?�� 경우 -> ?��?���? 처리?�� 것인�?

        ?��?�� ?��?��?��?�� �? Texture Filtering
        Texture Filtering Setting
     */
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    /* 
        ?��?��처�?? 모델?�� ?��?��?�� ?��
        모델 ?��?��?�� ?��?���? 좌표�?, [0,1] ?�� 벗어?��?�� 경우
        ?��?���? 좌표?�� ????��?��?�� ?��?��처�?? ?��?�� -> ?��?���? 처리?�� 것인�?

        ?��?�� ?��?��?��?�� �? Texture Wrapping
        Texture Wrapping Setting
     */
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;

private:
    Texture() {}
    // 초기?�� ?��?�� => 무조�? ?���?
    void CreateTexture();
    // ?��미�?? ?��?��?�� => ?��?��처�?? ?��?��
    void SetTextureFromImage(const Image* image);
    
    uint32_t m_texture { 0 }; // gl texture obj id
    int m_width { 0 };
    int m_height { 0 };
    uint32_t m_format { GL_RGBA };
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

    uint32_t m_texture { 0 };
};

#endif // __TEXTURE_H__