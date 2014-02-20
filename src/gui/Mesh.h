/*
    Taken from http://www.hightech-journal.net/opengl-tutorial-03-3d-modelle-zeichnen-beleuchten-assimp-importieren
*/

#ifndef MESH_H
#define MESH_H

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

#include <assimp/Importer.hpp>	// C++ importer interface
#include <assimp/scene.h>		// Output data structure
#include <assimp/postprocess.h>	// Post processing flags

/**
 * @brief Wrapper class for the Assimp library.
 */
class Mesh {
public:
    /**
     * @brief Creates a new Mesh object.
     * @param numVertices The number of model vertices.
     * @param vertices The model's vertices itself.
     * @param normals The model's normals itself.
     * @param numFaces The number of model faces.
     * @param faces The model's faces itself.
     */
    Mesh(unsigned int numVertices, aiVector3D* vertices, aiVector3D* normals, unsigned int numFaces, aiFace* faces);
    ~Mesh(void);

    //! The model's vertices
    aiVector3D* m_vertices;

    //! The model's normals
    aiVector3D* m_normals;

    //! The model's texture coordinates
    aiVector3D* m_textureCoords;

    //! The model's indices
    GLuint* m_indices;

    //! The number of vertices
    GLuint m_numVertices;

    //! The number of indices
    GLuint m_numIndices;
};

#endif // MESH_H