//
// Created by klemen on 19/12/2020.
//

#include "Mesh.h"
#include "../Core.h"

void Mesh::bind() const
{
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IBO);
}
void Mesh::unbind() const
{
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::dispose()
{
    bind();
    if (VBO)
        glDeleteBuffers(1, &VBO);
    if (IBO)
        glDeleteBuffers(1, &IBO);
    if (VAO)
        glDeleteVertexArrays(1, &VAO);

    unbind();

    if (indices)
        delete[] this->indices;
    if (vertices)
        delete[] this->vertices;


}
