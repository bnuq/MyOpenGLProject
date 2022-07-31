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
    /* 
        glBindVertexArray
            ���ݺ��� ����ϰ��� �ϴ� GL VAO �� �����ϴ� �Լ�
            ���� ������ �ʾƵ� ���ݺ��ʹ� �̰��� ����ϰڴ� => Bind �Ѵٰ� ���Ѵ�

        VertexLayout ��ü�� �����ϴ� GL VAO �� ���ݺ��� ��� �ϰڴ�
        
        ����� GL VAO �� ���� ���س��� ���� ������ �����ؾ� �Ѵ�
        
        �ڿ������� ������ �����ϴ� GL VBO ��, �� GL VAO �� ���ؼ� �ؼ��Ǵ� ���۰� �ȴ�
            �ݵ�� vao �� ���� ������ ��, vbo �� �����ؾ� �Ѵ�

        GL VBO = �ٱ���, �����͸� ����
                 ���� �ϳ��� ���� �پ��� �����͵��� �׳� �Ѳ����� ����Ǿ� �ִ�
                 �׸��� �����鿡 ���� �����Ͱ� ���о���, �Ϸķ� ����Ǿ� �ִ�
                 �� ������ ����� ���� ������ �� = Attributes �� ����
                 Vertex Attribute �� �Ѵ�

        GL VAO = ����, �ٱ��Ͽ� ��� �����͸� �ؼ��ϴ� ���
                 Attribute �� �д� ����� �˷��ش�
     */
    glBindVertexArray(m_vertexArrayObject);
}


VertexLayout::~VertexLayout()
{
    if (m_vertexArrayObject)
    {
        glDeleteVertexArrays(1, &m_vertexArrayObject);
    }
}


/*
    Attribute Format �� �˷��ִ� �Լ�
    �˷��� Format �� GL VAO �� Attribute Pointer �� �����Ͽ�
    ���ۿ��� �� vertex attribute �� ���� �� �ֵ��� �Ѵ�
 */
void VertexLayout::SetAttrib // ���� ������ �� �ϳ��� Attribute �� ���� ������ ����
(
    uint32_t attribIndex,   // Attribute Index = GL VAO �� �� ��° attribute pointer �� ������ �� ��
    int count,              // �ش� attribute �� �� ���� �����ͷ� �����Ǵ� ��
    uint32_t type,          // �������� Ÿ���� ��������
    bool normalized,        // attribute �� normalized �ƴ� ��
    size_t stride,          // ���� ������ attribute �� �б� ���ؼ�, �󸶳� �̵��ؾ� �ϴ� ��
    uint64_t offset         // ó�� ������ attribute �� ��������, ��𼭺��� �о�� �ϴ� ��
) const
{
    /*
        glEnableVertexAttribArray
            ���� bind �� GL VAO ���� 
            �� ��° attrubute pointer �� ����ϰڴٴ� ���� �˸���

        �����ϰ��� �ϴ� attribute pointer ��ȣ�� ����
        �ش� attribute pointer �� ����ϰڴٰ� �˸�
     */
    glEnableVertexAttribArray(attribIndex);
    /* 
        �ش� attribute pointer �� vertex attribute �� �д� ����� ����
        ���� ���ۿ���, �� �����鸶�� ������ �ִ� vertex attribute �� �о �� �ִ�
     */
    glVertexAttribPointer(attribIndex, count, type, normalized, stride, (const void*)offset);
}