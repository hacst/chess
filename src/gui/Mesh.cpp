/*
    Taken from http://www.hightech-journal.net/opengl-tutorial-03-3d-modelle-zeichnen-beleuchten-assimp-importieren
*/

#include "Mesh.h"

Mesh::Mesh(unsigned int numVertices, aiVector3D* vertices, aiVector3D* normals, unsigned int numFaces, aiFace* faces)
    : m_numVertices(numVertices)
    , m_vertices(vertices)
    , m_normals(normals)
    , m_numIndices(numFaces * 3) {

    m_indices = new GLuint[m_numIndices];

    for (unsigned int i = 0; i < numFaces; i++) {
        m_indices[i * 3] = (GLuint)faces[i].mIndices[0];
        m_indices[i * 3 + 1] = (GLuint)faces[i].mIndices[1];
        m_indices[i * 3 + 2] = (GLuint)faces[i].mIndices[2];
    }
}

Mesh::~Mesh(void) {
    delete[] m_indices;
}