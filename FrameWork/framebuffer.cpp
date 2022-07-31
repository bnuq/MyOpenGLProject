#include "framebuffer.h"

/* PUBLIC */
FramebufferUPtr Framebuffer::Create(const TexturePtr colorAttachment)
{
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if (!framebuffer->InitWithColorAttachment(colorAttachment))
        return nullptr;
    return std::move(framebuffer);
}


// color buffer �� ����ϴ� �ؽ�ó�� ��ü
// render buffer, framebuffer �� ��� ���� �Ҵ��Ѵ�
FramebufferUPtr Framebuffer::ReCreate(const TexturePtr another)
{
    glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    glDeleteFramebuffers(1, &m_framebuffer);

    return Create(another);
}


Framebuffer::~Framebuffer()
{
    if (m_depthStencilBuffer)
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    if (m_framebuffer)
        glDeleteFramebuffers(1, &m_framebuffer);
}


void Framebuffer::BindToDefault()
{
    // Default Frame Buffer => ȭ�鿡 ����ϴ� ��
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Framebuffer::Bind() const
{
    // Frame Buffer �� Bind �ϸ�, ������ render buffer �� texture �� ��� �ڵ����� ����� �� �ִ�
    // �Ѵ� Frame Buffer �Ʒ��� ���ϴϱ�
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}



/* PRIVATE */
bool Framebuffer::InitWithColorAttachment(const TexturePtr colorAttachment)
{
    m_colorAttachment = colorAttachment;    // = color buffer �� ���� �ؽ�ó

    glGenFramebuffers(1, &m_framebuffer);               // FBO ����
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);   // FBO Bind

        // ������ ���� -> Color Buffer �� �ؽ�ó ���̱�
        glFramebufferTexture2D
        (
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
            colorAttachment->Get(),     // return texture handle id
            0                           // mipmap level
        );

    // Render Buffer = depth + stencil buffer ����
    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
        glRenderbufferStorage
        (
            GL_RENDERBUFFER,
            GL_DEPTH24_STENCIL8,                // depth=>24bits, stencil=>8bits ���
            colorAttachment->GetWidth(),        // color buffer �� ���� �ʺ�, ������ �̹��� ���
            colorAttachment->GetHeight()
        );
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    glFramebufferRenderbuffer                   // Frame Buffer �� Render Buffer ���̱�
    (
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,            // depth + stencil ��ġ�� �����Ѵ�
        GL_RENDERBUFFER,
        m_depthStencilBuffer                    // ���̷��� Render Buffer �� id
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