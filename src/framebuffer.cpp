#include "framebuffer.h"

/* PUBLIC */
FramebufferUPtr Framebuffer::Create(const TexturePtr colorAttachment)
{
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->InitWithColorAttachment(colorAttachment))
        return nullptr;
    return std::move(framebuffer);
}

// FramebufferUPtr Framebuffer::ReCreate(const TexturePtr another)
// {
//     glDeleteRenderbuffers(1, &m_depthStencilBuffer);
//     glDeleteFramebuffers(1, &m_framebuffer);

//     return Create(another);
// }

Framebuffer::~Framebuffer()
{
    if (m_depthStencilBuffer)
    {
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    }
    if (m_framebuffer)
    {
        glDeleteFramebuffers(1, &m_framebuffer);
    }

    SPDLOG_INFO("Destruct FrameBuffer");
}

void Framebuffer::BindToDefault()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

/* PRIVATE */
bool Framebuffer::InitWithColorAttachment(const TexturePtr colorAttachment)
{
    m_colorAttachment = colorAttachment;    // = color buffer �� ���� �ؽ�ó

    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    // ������ ���� -> Color Buffer �� �ؽ�ó ���̱�
    glFramebufferTexture2D
    (
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        colorAttachment->Get(),     // return texture handle id
        0                           // mipmap level
    );

    // depth + stencil buffer ����
    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    glRenderbufferStorage
    (
        GL_RENDERBUFFER,
        GL_DEPTH24_STENCIL8,        // depth=>24bits, stencil=>8bits ���
        colorAttachment->GetWidth(),
        colorAttachment->GetHeight()
    );
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer
    (
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT, // depth, stencil buffer �� ���δ�
        GL_RENDERBUFFER,
        m_depthStencilBuffer
    );
    // frame buffer �� ���� üũ
    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to create framebuffer: {}", result);
        return false;
    }   
    // ȭ�鿡 �׸��� ������ ���۷� �ٽ� �ʱ�ȭ �Ѵ�
    BindToDefault();
    return true;
}