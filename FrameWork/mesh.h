#ifndef __MESH_H__
#define __MESH_H__

#include "common.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "program.h"


// �� ������ ����� ����
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};



// �� Mesh �� ������ �ؽ�ó�� = Material
CLASS_PTR(Material)
class Material
{
public:
    static MaterialUPtr Create() { return MaterialUPtr(new Material()); }
    void SetTextures(TexturePtr diff, TexturePtr spec)
    {
        diffuse = diff;
        specular = spec;
    }

    /* 
        Material �� ����Ƿ��� Fragment Shader �� ���޵Ǿ�� �Ѵ�
        ����� Fragment Shader -> �� ���Ե� Program �� �����ؼ� ���
     */
    void SetToProgram(const Program* program) const;

    
    // 2 ���� �ؽ�ó��� ����
    TexturePtr diffuse;         // 1. Diffuse Texture
    TexturePtr specular;        // 2. Specular Texture
    float shininess { 32.0f };

private:
    Material() {}
};



// �ϳ��� �� = �ϳ��� Mesh
CLASS_PTR(Mesh)
class Mesh
{
public:
    static MeshUPtr Create
    (
        const std::vector<Vertex>& vertices,        // Mesh �� �����ϴ� ���� �迭
        const std::vector<uint32_t>& indices,       // ���� �迭�� ������ ��Ÿ���� �ε��� �迭
        uint32_t primitiveType                      // Mesh �� �����ϴ� �⺻ ������ ��������
    );
    static MeshUPtr CreateBox();
    static MeshUPtr CreatePlane();

    void Draw(const Program*) const;
    void GPUInstancingDraw(const Program* program, int num) const;


    const VertexLayout* GetVertexLayout() const { return m_vertexLayout.get(); }
    BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
    BufferPtr GetIndexBuffer() const { return m_indexBuffer; }
    MaterialPtr GetMaterial() const { return m_material; }
    void SetMaterial(MaterialPtr material) { m_material = material; }

private:
    Mesh() {}

    void Init
    (
        const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices,
        uint32_t primitiveType
    );


    uint32_t m_primitiveType { GL_TRIANGLES };      // ������ �̷�� �⺻ ����
    VertexLayoutUPtr m_vertexLayout;                // �� ������ ����� Attributes �� �б� ���� VAO
    BufferPtr m_vertexBuffer;                       // Mesh �� �����鿡 ����� �����͵��� �����ϴ� ����, VBO
    BufferPtr m_indexBuffer;                        // Mesh �� �ε��� �迭�� ����� �����͵��� �����ϴ� ����, EBO

    MaterialPtr m_material;                         // Mesh �� ����Ǵ� �ؽ�ó��
};

#endif // __MESH_H__