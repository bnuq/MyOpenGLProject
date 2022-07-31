#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "texture.h"

CLASS_PTR(Framebuffer)
class Framebuffer
{
public:
    static FramebufferUPtr Create(const TexturePtr colorAttachment);
    FramebufferUPtr ReCreate(const TexturePtr colorAttachment);
    
    ~Framebuffer();
    
    static void BindToDefault();
    void Bind() const;
    
    const uint32_t Get() const { return m_framebuffer; }
    const TexturePtr GetColorAttachment() const { return m_colorAttachment; }

private:
    Framebuffer() {}
    bool InitWithColorAttachment(const TexturePtr colorAttachment);

    /* 
        �����ӹ���
        = color buffer + depth buffer + stencil buffer
        = gl frame buffer �� ����

            color buffer                        => texture �� ����
            depth buffer + stencil buffer       => gl redner buffer �� ����
     */
    uint32_t m_framebuffer { 0 };           // ���� ��� ��ü�� �����ϴ� Frame Buffer
    uint32_t m_depthStencilBuffer { 0 };    // Render Buffer, single image, ���⼭�� ���ٽǰ� ���̸� �����ϴ� �뵵�� ���
    TexturePtr m_colorAttachment;           // frame buffer ~ color buffer �� ���� �ؽ�ó
                                            // �ؽ�ó�� ���� �����ϴ� �������, color buffer ó�� ����Ѵ�
};

#endif // __FRAMEBUFFER_H__