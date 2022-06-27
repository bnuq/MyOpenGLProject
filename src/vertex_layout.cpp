#include "vertex_layout.h"

VertexLayoutUPtr VertexLayout::Create()
{
    auto vertexLayout = VertexLayoutUPtr(new VertexLayout());
    vertexLayout->Init();
    return std::move(vertexLayout);
}

void VertexLayout::Init()
{
    glGenVertexArrays(1, &m_vertexArrayObject);
    Bind();
}

void VertexLayout::Bind() const
{
    glBindVertexArray(m_vertexArrayObject);
}

VertexLayout::~VertexLayout()
{
    if (m_vertexArrayObject)
    {
        glDeleteVertexArrays(1, &m_vertexArrayObject);
    }
}


void VertexLayout::SetAttrib // ?��?�� ?��?��?�� �? ?��?��?�� Attribute ?�� ????�� ?��보�?? �??��
(
    uint32_t attribIndex,   // Attribute Index = GL VAO ?�� �? 번째 attribute pointer ?�� ????��?�� ?�� �?
    int count,              // ?��?�� attribute �? �? 개의 ?��?��?���? 구성?��?�� �?
    uint32_t type,          // ?��?��?��?�� ????��??? 무엇?���?
    bool normalized,        // attribute �? normalized ?��?�� �?
    size_t stride,          // ?��?�� ?��?��?�� attribute �? ?���? ?��?��?��, ?��마나 ?��?��?��?�� ?��?�� �?
    uint64_t offset         // 처음 ?��?��?�� attribute �? ?��?��?���?, ?��?��?���??�� ?��?��?�� ?��?�� �?
) const
{
    /*
        glEnableVertexAttribArray
            ?��?�� bind ?�� GL VAO ?��?�� 
            �? 번째 attrubute pointer �? ?��?��?��겠다?�� 것을 ?��린다

        ?��?��?��고자 ?��?�� attribute pointer 번호�? �??��
        ?��?�� attribute pointer �? ?��?��?��겠다�? ?���?
     */
    glEnableVertexAttribArray(attribIndex);
    /* 
        ?��?�� attribute pointer ?�� vertex attribute �? ?��?�� 방법?�� ?��?��
        ?��?�� 버퍼?��?��, �? ?��?��?��마다 �?�?�? ?��?�� vertex attribute �? ?��?��?�� ?�� ?��?��
     */
    glVertexAttribPointer(attribIndex, count, type, normalized, stride, (const void*)offset);
}