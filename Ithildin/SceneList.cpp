#include "SceneList.h"
#include "Resources/Material.h"
#include "Resources/Model.h"
#include "Resources/Texture.h"
#include <functional>
#include <random>

const std::vector<std::pair<std::string, std::function<SceneAssets(SceneList::CameraInitialState& initialState)>>> SceneList::s_AllScenes =
{
	{ "Cornell Box", CornellBox }
};

SceneAssets SceneList::CornellBox(CameraInitialState& camera)
{
	camera.m_ModelView = glm::lookAt(glm::vec3(278, 278, 800), glm::vec3(278, 278, 0), glm::vec3(0, 1, 0));
	camera.m_FieldOfView = 40;
	camera.m_Aperture = 0.0f;
	camera.m_FocusDistance = 10.0f;
	camera.m_ControlSpeed = 500.0f;
	camera.m_GammaCorrection = true;
	camera.m_HasSky = false;

	const auto i = glm::mat4(1.0f);
	const auto white = Resources::Material::Lambertian(glm::vec3(0.73f, 0.73f, 0.73f));

	auto box0 = Resources::Model::CreateBox(glm::vec3(0, 0, -165), glm::vec3(165, 165, 0), white);
	auto box1 = Resources::Model::CreateBox(glm::vec3(0, 0, -165), glm::vec3(165, 330, 0), white);

	box0.SetTransform(glm::rotate(glm::translate(i, glm::vec3(555 - 130 - 165, 0, -65)), glm::radians(-18.0f), glm::vec3(0, 1, 0)));
	box1.SetTransform(glm::rotate(glm::translate(i, glm::vec3(555 - 265 - 165, 0, -295)), glm::radians(15.0f), glm::vec3(0, 1, 0)));

	std::vector<Resources::Model> models;
	models.push_back(Resources::Model::CreateCornellBox(555));
	models.push_back(box0);
	models.push_back(box1);

	return std::make_tuple(std::move(models), std::vector<Resources::Texture>());
}
