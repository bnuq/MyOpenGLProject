#ifndef __MODEL_H__
#define __MODEL_H__

#include "common.h"
#include "mesh.h"
#include "program.h"

// assimp ���̺귯���� Ȱ��
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CLASS_PTR(Model)
class Model
{
public:
    static ModelUPtr Load(const std::string& filename);
    void Draw(const Program*) const;

    int GetMeshCount() const { return (int)m_meshes.size(); }
    MeshPtr GetMesh(int index) const { return m_meshes[index]; }

private:
    Model() {}
    bool LoadByAssimp(const std::string& filename);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    void ProcessNode(aiNode* node, const aiScene* scene);
        
    std::vector<MeshPtr> m_meshes;                  // �� => �������� Mesh �� �������� ����
    std::vector<MaterialPtr> m_materials;           // �� Mesh ���� Material �� ������ �ִ�
};

#endif // __MODEL_H__