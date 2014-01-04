#ifndef MODEL_H
#define MODEL_H

#include "gui/AssimpHelper.h"

/**
 * @brief Representing a chess figure (e.g. King, Queen, ...).
 */
class Model {
public:
	//! Possible model colors.
	enum Color {
		BLACK,
		WHITE
	};

	/**
	 * @brief Loads the model from the filesystem.
	 * @param file The filename with directory relative to the game's executable file.
	 */
	Model(std::string file);

	/**
	 * @brief Imports the model from filesystem.
	 */
	void loadScene();

	/**
	 * @brief Corrects the model positioning if the model (in the given file) is not proper
	 * located at 0/0/0 local space coordinates, the rotation or the scaling factor is wrong.
	 * @note This should only be used if there's no proper model file available.
	 * @param localX Sets the local x coordinate.
	 * @param localY Sets the local y coordinate.
	 * @param localZ Sets the local z coordinate.
	 * @param scaleFactor The scaling factor to shrink or enlarge.
	 * @param rotateX The rotation in degree along the x axis.
	 * @param rotateY The rotation in degree along the y axis.
	 * @param rotateZ The rotation in degree along the z axis.
	 */
	void setCorrectionValues(int localX, int localY, int localZ, float scaleFactor, int rotateX, int rotateY, int rotateZ);

	/**
	 * @brief Sets the models color.
	 * @param color The color of the model.
	 */
	void setColor(Color color);

	/**
	 * @brief Sets a new global position (world coordinates) for the model.
 	 * @param globalX The global x coordinate.
	 * @param globalY The global y coordinate.
	 * @param globalZ The global z coordinate.
	 */
	void setPosition(int globalX, int globalY, int globalZ);

	/**
	 * @brief Draws the model at configured world coordinates.
	 */
	void draw();

private:
	//! Filename (an path) relative to the executable, which contains the meshes.
	std::string m_file;

	//! Color of the model.
	Color m_color;

	//! Structure for the model's world coordinates.
	struct Position {
		int x;
		int y;
		int z;
	};

	//! The model's position in world coordinates.
	Position m_position;

	//! Scaling factor of the model.
	float m_correctScaling;

	//! The local correction values for correcting unproper model coordinates.
	Position m_correctPosition;

	//! The local correction values for correcting unproper model rotation.
	Position m_correctRotation;

	//! Pointer to the AssimpHelper, which holds the meshes.
	AssimpHelperPtr model;
};

using ModelPtr = std::shared_ptr<Model>; // Shared pointer for better garbage handling.

#endif // MODEL_H
