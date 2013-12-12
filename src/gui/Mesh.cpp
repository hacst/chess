// Mesh.cpp - Implementiert die Mesh-Klasse und dessen Konstruktor
// und Destruktor.
#include "Mesh.h"

// übergibt die Werte des Meshes an die eigenen Variablen
// und extrahiert die Indices aus den einzelnen Faces.
Mesh::Mesh(unsigned int numVertices, aiVector3D* vertices, aiVector3D* normals, unsigned int numFaces, aiFace* faces) {
	this->numVertices = numVertices;
	this->vertices = vertices;
	this->normals = normals;
	this->numIndices = numFaces * 3;
	this->indices = new GLuint[this->numIndices];

	for (unsigned int i = 0; i < numFaces; i++) {
		indices[i * 3] = (GLuint)faces[i].mIndices[0];
		indices[i * 3 + 1] = (GLuint)faces[i].mIndices[1];
		indices[i * 3 + 2] = (GLuint)faces[i].mIndices[2];
	}
}

// Löscht die gespeicherten Indices
Mesh::~Mesh(void) {
	delete[] indices;
}