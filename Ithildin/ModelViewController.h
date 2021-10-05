#pragma once
#include "Math/Math.h"

class ModelViewController final
{
public:
	void Reset(const glm::mat4& modelView);
	glm::mat4 GetModelView() const;

	bool OnKey(int key, int scanCode, int action, int mods);
	bool OnCursorPosition(double xPosition, double yPosition);
	bool OnMouseButton(int button, int action, int mods);

	bool UpdateCamera(double speed, double deltaTime);

private:
	void MoveForward(float d);
	void MoveRight(float d);
	void MoveUp(float d);
	void Rotate(float y, float x);
	void UpdateVectors();

private:
	// Matrices and Vectors.
	glm::mat4 orientation_{};

	glm::vec4 position_{};
	glm::vec4 right_{ 1, 0, 0, 0 };
	glm::vec4 up_{ 0, 1, 0, 0 };
	glm::vec4 forward_{ 0, 0, -1, 0 };

	// Control states.
	bool cameraMovingLeft_{};
	bool cameraMovingRight_{};
	bool cameraMovingBackward_{};
	bool cameraMovingForward_{};
	bool cameraMovingDown_{};
	bool cameraMovingUp_{};

	float cameraRotX_{};
	float cameraRotY_{};
	float modelRotX_{};
	float modelRotY_{};

	double mousePosX_{};
	double mousePosY_{};

	bool mouseLeftPressed_{};
	bool mouseRightPressed_{};
};