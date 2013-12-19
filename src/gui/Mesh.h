#ifndef MESH_H
#define MESH_H

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>
#include <assimp/Importer.hpp>	// C++ importer interface
#include <assimp/scene.h>		// Output data structure
#include <assimp/postprocess.h>	// Post processing flags

class Mesh {
public:
	// Konstruktor, der alle wichtigen Werte des Meshes übernimmt.
	Mesh(unsigned int numVertices, aiVector3D* vertices,
		aiVector3D* normals, unsigned int numFaces, aiFace* faces);

	// Destruktor, der am Ende des Programms aufräumt.
	~Mesh(void);

	// Die geladenen Vertices, Normals und Texturkoordinaten als Vektoren
	aiVector3D* vertices;
	aiVector3D* normals;
	aiVector3D* textureCoords;
	GLuint* indices;

	// Die Anzahl der Vertices bzw. Indices
	GLuint numVertices;
	GLuint numIndices;
};

#endif // MESH_H
