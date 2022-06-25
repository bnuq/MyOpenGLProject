#include "buffer.h"

/* PUBLIC */
BufferUPtr Buffer::CreateWithData
(
    uint32_t bufferType,
    uint32_t usage,
    const void* data,
    size_t stride,
    size_t count
)
{
    auto buffer = BufferUPtr(new Buffer());

    if (!buffer->Init(bufferType, usage, data, stride, count))
        return nullptr;

    return std::move(buffer);
}

Buffer::~Buffer() 
{
    if (m_buffer)
    {
        glDeleteBuffers(1, &m_buffer);
    }
}

void Buffer::Bind() const
{   
    /* 
        ARRAY BUFFER    ���� ������ ����
        ELEMENT BUFFER  �ε��� ���� ����
     */
    glBindBuffer(m_bufferType, m_buffer);
}


/* PRIVATE */
bool Buffer::Init
(
    uint32_t bufferType,    // GL_ARRAY_BUFFER | GL_ELEMENT_BUFFER , ������� ���� ���� ����
    uint32_t usage,         /* 
                                ������ �����͸� �󸶳� �ٲٴ� ��
                                STATIC, DYNAMIC, STREAM

                                ��� ������ ��������
                                DRAW, COPY, READ
                            */
    const void* data,       // GPU ���ۿ� ��������, CPU �� ����� ������ �ּ�
    size_t stride,
    size_t count
)
{
    m_bufferType = bufferType;
    m_usage = usage;
    m_stride = stride;
    m_count = count;
    glGenBuffers(1, &m_buffer);

    Bind();

    glBufferData(m_bufferType, m_stride * m_count, data, usage);

    return true;
}