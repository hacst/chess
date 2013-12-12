#ifndef ASSIMPHELPER_H
#define ASSIMPHELPER_H

#include "Mesh.h"

#include <vector>
#include <memory>

class AssimpHelper {
public:
	void importScene(std::string filename);
	~AssimpHelper();
	void drawScene();

private:
	// Assimp Szene und Importer
	const aiScene* scene;
	Assimp::Importer* importer;

	// Die Meshes/Modelle in unserer Szene
	std::vector<Mesh*> meshes;

	void drawMesh(Mesh* mesh);
};

using AssimpHelperPtr = std::shared_ptr<AssimpHelper>;

#endif // ASSIMPHELPER_H