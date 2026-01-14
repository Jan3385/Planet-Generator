#pragma once

#include "GLWrapper/Mesh.h"
#include "Component/Essential/Mesh/BaseMeshComponent.h"
#include <glad/glad.h>

namespace Component{
class GLMesh : public BaseMesh {
public:
    GLMesh(Object::BaseObject* owner) : BaseMesh(owner) {};
    ~GLMesh() override = default;

    bool IsEmpty() const override;

    std::vector<float> GetVerticies() const override;
    std::vector<unsigned int> GetIndices() const;
    std::vector<GL::VertexObj> GetGLVerticies() const { return this->mesh.vertices; }
    void SetMeshData(const GL::Mesh& mesh);

    void GLBind() const { this->mesh.Bind(); }
    void GLDraw() const { glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->GetIndices().size()), GL_UNSIGNED_INT, 0); }
protected:
    GL::Mesh mesh;
private:
};
}