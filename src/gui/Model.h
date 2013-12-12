#ifndef MODEL_H
#define MODEL_H

#include "gui/AssimpHelper.h"

class Model {
public:
	struct Position {
		int x;
		int y;
		int z;
	};

	enum Color {
		BLACK,
		WHITE
	};

	Model(std::string file);

	// interface methods
	void loadScene();
	void draw();
	void setCorrectionValues(float localX, float localY, float localZ, float scaleFactor, float rotateX, float rotateY, float rotateZ);
	void rotateY(int degree);
	void setColor(Color color);
	void setPosition(int globalX, int globalY, int globalZ);

private:
	std::string m_file;
	Color m_color;
	int m_rotateY;
	Position m_position;

	float m_correctScaling;
	Position m_correctPosition, m_correctRotation;

	AssimpHelperPtr model;
};

using ModelPtr = std::shared_ptr<Model>;

#endif // MODEL_H