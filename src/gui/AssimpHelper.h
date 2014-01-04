#ifndef ASSIMPHELPER_H
#define ASSIMPHELPER_H

#include "Mesh.h"

#include <vector>
#include <memory>

/**
 * Assimp wrapper class to handle scene modeling in an more comfortable way.
 */
class AssimpHelper {
public:
	/**
	 * @brief Imports the scene by filename.
	 * @param filename The filename of the scene to import.
	 */
	void importScene(std::string filename);
	~AssimpHelper();

	/**
	 * @brief Draws the scene.
	 */
	void drawScene();

private:
	//! The scene object
	const aiScene* scene;

	//! The Assimp Importer pointer
	Assimp::Importer* importer;

	//! The scene Meshes
	std::vector<Mesh*> meshes;

	/**
	 * @brief Draws the given mesh.
	 * @param mesh The mesh to draw.
	 */
	void drawMesh(Mesh* mesh);
};

using AssimpHelperPtr = std::shared_ptr<AssimpHelper>; // Shared pointer for better garbage handling.

#endif // ASSIMPHELPER_H