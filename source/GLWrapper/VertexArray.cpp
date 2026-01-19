#include "VertexArray.h"

GL::VertexArray::VertexArray()
{
    this->ID = 0;
}
GL::VertexArray::VertexArray(std::string name)
{
    glGenVertexArrays(1, &ID);
    this->name = name;
    this->Bind();
    glObjectLabel(GL_VERTEX_ARRAY, ID, -1, this->name.c_str());
    this->Unbind();
}
GL::VertexArray::~VertexArray()
{
    if(this->ID != 0) {
        glDeleteVertexArrays(1, &ID);
        ID = 0;
    }
}

void GL::VertexArray::Bind() const
{
    if(ID == 0){
        Debug::LogFatal("Attempt to bind uninitialized GLVertexArray: " + this->name);
    }
    
    glBindVertexArray(ID);
}

void GL::VertexArray::Unbind() const
{
    glBindVertexArray(0);
}
