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
        ??€μ²λ?? λͺ¨λΈ? ? ?©?  ?
        ??€μ²μ?? λͺ¨λΈ? ?¬κΈ? μ°¨μ΄κ°? ?λ¬? λ§μ΄ ?? κ²½μ° -> ?΄?»κ²? μ²λ¦¬?  κ²μΈκ°?

        ?΄? ?¬?©?? κ²? Texture Filtering
        Texture Filtering Setting
     */
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    /* 
        ??€μ²λ?? λͺ¨λΈ? ? ?©?  ?
        λͺ¨λΈ ? ? ? ??€μ²? μ’νκ°?, [0,1] ? λ²μ΄?? κ²½μ°
        ??€μ²? μ’ν? ?????? ??€μ²κ?? ??€ -> ?΄?»κ²? μ²λ¦¬?  κ²μΈκ°?

        ?΄? ?¬?©?? κ²? Texture Wrapping
        Texture Wrapping Setting
     */
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;

private:
    Texture() {}
    // μ΄κΈ°? ?¨? => λ¬΄μ‘°κ±? ?±κ³?
    void CreateTexture();
    // ?΄λ―Έμ?? ?°?΄?° => ??€μ²λ?? ??±
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