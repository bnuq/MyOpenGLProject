#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "common.h"

CLASS_PTR(Image)
class Image
{
public:
    static ImageUPtr Load(const std::string& filepath, bool flipVertical = true);
    static ImageUPtr Create(int width, int height, int channelCount = 4);
    static ImageUPtr CreateSingleColorImage(int width, int height, const glm::vec4& color);
    void SetCheckImage(int gridX, int gridY);
    ~Image();
      
    const uint8_t* GetData() const { return m_data; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannelCount() const { return m_channelCount; }

private:
    Image() {};
    // stb library 이용 => 이미지 로드
    bool LoadWithStb(const std::string& filepath, bool flipVertical);
    // 직접 메모리에 이미지 데이터를 할당 => 이미지 파일을 직접 만든다
    bool Allocate(int width, int height, int channelCount);

    int m_width { 0 };
    int m_height { 0 };
    int m_channelCount { 0 };
    uint8_t* m_data { nullptr };
};

#endif // __IMAGE_H__