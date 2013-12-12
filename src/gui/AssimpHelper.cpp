#include <iostream>

#include "AssimpHelper.h"

using namespace std;

//Import die angegebene 3D-Szene mit dem Assimp importer und speichert die Vertex-Daten in den
void AssimpHelper::importScene(string filename) {
	importer = new Assimp::Importer();
	scene = importer->ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_ImproveCacheLocality |
		aiProcess_SortByPType);

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		Mesh* tempMesh = new Mesh(scene->mMeshes[i]->mNumVertices,
			scene->mMeshes[i]->mVertices,
			scene->mMeshes[i]->mNormals,
			scene->mMeshes[i]->mNumFaces,
			scene->mMeshes[i]->mFaces);

		meshes.push_back(tempMesh);
	}
}

void AssimpHelper::drawScene() {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		drawMesh(meshes[i]);
	}
}

//In der Draw-Methode werden die Objekte in den Backbuffer gezeichnet.
void AssimpHelper::drawMesh(Mesh* mesh) {
	//Zeichnet das Mesh in mittels Vertex array.

	//Aktiviert die Arrays für Vertices und Normals.
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	//Setzt den Pointer auf den Normal- und Vertex-Array
	glNormalPointer(GL_FLOAT, 0, mesh->normals);
	glVertexPointer(3, GL_FLOAT, 0, mesh->vertices);

	//Zeichnet die Elemente als Dreiecke anhand der Indices
	glDrawElements(GL_TRIANGLES, mesh->numIndices, GL_UNSIGNED_INT, mesh->indices);

	//Deaktiviert danach wieder die Arrays.
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

AssimpHelper::~AssimpHelper() {
	importer->FreeScene();
	delete importer;
	for (unsigned int i = 0; i < meshes.size(); i++) {
		delete meshes[i];
	}
}