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
        ?Öç?ä§Ï≤òÎ?? Î™®Îç∏?óê ?†Å?ö©?ï† ?ïå
        ?Öç?ä§Ï≤òÏ?? Î™®Îç∏?ùò ?Å¨Í∏? Ï∞®Ïù¥Í∞? ?ÑàÎ¨? ÎßéÏù¥ ?Çò?äî Í≤ΩÏö∞ -> ?ñ¥?ñªÍ≤? Ï≤òÎ¶¨?ï† Í≤ÉÏù∏Í∞?

        ?ù¥?ïå ?Ç¨?ö©?ïò?äî Í≤? Texture Filtering
        Texture Filtering Setting
     */
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    /* 
        ?Öç?ä§Ï≤òÎ?? Î™®Îç∏?óê ?†Å?ö©?ï† ?ïå
        Î™®Îç∏ ?†ï?†ê?ùò ?Öç?ä§Ï≤? Ï¢åÌëúÍ∞?, [0,1] ?ùÑ Î≤óÏñ¥?Çò?äî Í≤ΩÏö∞
        ?Öç?ä§Ï≤? Ï¢åÌëú?óê ????ùë?êò?äî ?Öç?ä§Ï≤òÍ?? ?óÜ?ã§ -> ?ñ¥?ñªÍ≤? Ï≤òÎ¶¨?ï† Í≤ÉÏù∏Í∞?

        ?ù¥?ïå ?Ç¨?ö©?ïò?äî Í≤? Texture Wrapping
        Texture Wrapping Setting
     */
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;

private:
    Texture() {}
    // Ï¥àÍ∏∞?ôî ?ï®?àò => Î¨¥Ï°∞Í±? ?Ñ±Í≥?
    void CreateTexture();
    // ?ù¥ÎØ∏Ï?? ?ç∞?ù¥?Ñ∞ => ?Öç?ä§Ï≤òÎ?? ?Éù?Ñ±
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