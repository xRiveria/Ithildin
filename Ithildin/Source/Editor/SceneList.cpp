#include "SceneList.h"
#include "Resources/Material.h"
#include "Resources/Model.h"
#include "Resources/Texture.h"
#include <functional>
#include <random>

const std::vector<std::pair<std::string, std::function<SceneAssets(SceneList::CameraInitialState& initialState)>>> SceneList::s_AllScenes =
{
	{ "Cube And Spheres", CubeAndSpheres },
    { "Ray Tracing In One Weekend", RayTracingInOneWeekend },
	{ "Planets In One Weekend", PlanetsInOneWeekend },
	{ "Lucy In One Weekend", LucyInOneWeekend },
	{ "Cornell Box", CornellBox },
	{ "Cornell Box & Lucy", CornellBoxLucy },
};

SceneAssets SceneList::CubeAndSpheres(CameraInitialState& cameraState)
{
	// Basic test scene.
	cameraState.m_ModelView = glm::translate(glm::mat4(1), glm::vec3(0, 0, -2));
	cameraState.m_FieldOfView = 90;
	cameraState.m_Aperture = 0.05f;
	cameraState.m_FocusDistance = 2.0f;
	cameraState.m_ControlSpeed = 2.0f;
	cameraState.m_GammaCorrection = false;
	cameraState.m_HasSky = true;

	std::vector<Resources::Model> models;
	std::vector<Resources::Texture> textures;

	models.push_back(Resources::Model::LoadModel("../Assets/Models/cube_multi.obj"));
	models.push_back(Resources::Model::CreateSphere(glm::vec3(1, 0, 0), 0.5, Resources::Material::Metallic(glm::vec3(0.7f, 0.5f, 0.8f), 0.2f), true));
	models.push_back(Resources::Model::CreateSphere(glm::vec3(-1, 0, 0), 0.5, Resources::Material::Dielectric(1.5f), true));
	models.push_back(Resources::Model::CreateSphere(glm::vec3(0, 1, 0), 0.5, Resources::Material::Lambertian(glm::vec3(1.0f), 0), true));

	textures.push_back(Resources::Texture::LoadTexture("../Assets/Textures/land_ocean_ice_cloud_2048.png", Vulkan::SamplerConfiguration()));

	return std::forward_as_tuple(std::move(models), std::move(textures));
}

SceneAssets SceneList::RayTracingInOneWeekend(CameraInitialState& cameraState)
{
	// Final scene from Ray Tracing In One Weekend book.
	cameraState.m_ModelView = glm::lookAt(glm::vec3(13, 2, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	cameraState.m_FieldOfView = 20;
	cameraState.m_Aperture = 0.1f;
	cameraState.m_FocusDistance = 10.0f;
	cameraState.m_ControlSpeed = 5.0f;
	cameraState.m_GammaCorrection = true;
	cameraState.m_HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	auto random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Resources::Model> models;

	models.push_back(Resources::Model::CreateSphere(glm::vec3(0, -1000, 0), 1000, Resources::Material::Lambertian(glm::vec3(0.5f, 0.5f, 0.5f)), isProc));

	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			const float chooseMat = random();
			const glm::vec3 center(a + 0.9f * random(), 0.2f, b + 0.9f * random());

			if (glm::length(center - glm::vec3(4, 0.2f, 0)) > 0.9)
			{
				if (chooseMat < 0.8f) // Diffuse
				{
					models.push_back(Resources::Model::CreateSphere(center, 0.2f, Resources::Material::Lambertian(glm::vec3(
						random() * random(),
						random() * random(),
						random() * random())),
						isProc));
				}
				else if (chooseMat < 0.95f) // Metal
				{
					models.push_back(Resources::Model::CreateSphere(center, 0.2f, Resources::Material::Metallic(
						glm::vec3(0.5f * (1 + random()), 0.5f * (1 + random()), 0.5f * (1 + random())),
						0.5f * random()),
						isProc));
				}
				else // Glass
				{
					models.push_back(Resources::Model::CreateSphere(center, 0.2f, Resources::Material::Dielectric(1.5f), isProc));
				}
			}
		}
	}

	models.push_back(Resources::Model::CreateSphere(glm::vec3(0, 1, 0), 1.0f, Resources::Material::Dielectric(1.5f), isProc));
	models.push_back(Resources::Model::CreateSphere(glm::vec3(-4, 1, 0), 1.0f, Resources::Material::Lambertian(glm::vec3(0.4f, 0.2f, 0.1f)), isProc));
	models.push_back(Resources::Model::CreateSphere(glm::vec3(4, 1, 0), 1.0f, Resources::Material::Metallic(glm::vec3(0.7f, 0.6f, 0.5f), 0.0f), isProc));

	return std::forward_as_tuple(std::move(models), std::vector<Resources::Texture>());
}

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

SceneAssets SceneList::PlanetsInOneWeekend(CameraInitialState& cameraState)
{
	// Same as RayTracingInOneWeekend but using textures.
	cameraState.m_ModelView = glm::lookAt(glm::vec3(13, 2, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	cameraState.m_FieldOfView = 20;
	cameraState.m_Aperture = 0.1f;
	cameraState.m_FocusDistance = 10.0f;
	cameraState.m_ControlSpeed = 5.0f;
	cameraState.m_GammaCorrection = true;
	cameraState.m_HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	auto random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Resources::Model> models;
	std::vector<Resources::Texture> textures;

	models.push_back(Resources::Model::CreateSphere(glm::vec3(0, -1000, 0), 1000, Resources::Material::Lambertian(glm::vec3(0.5f, 0.5f, 0.5f)), isProc));

	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			const float chooseMat = random();
			const glm::vec3 center(a + 0.9f * random(), 0.2f, b + 0.9f * random());

			if (glm::length(center - glm::vec3(4, 0.2f, 0)) > 0.9)
			{
				if (chooseMat < 0.8f) // Diffuse
				{
					models.push_back(Resources::Model::CreateSphere(center, 0.2f, Resources::Material::Lambertian(glm::vec3(
						random() * random(),
						random() * random(),
						random() * random())),
						isProc));
				}
				else if (chooseMat < 0.95f) // Metal
				{
					models.push_back(Resources::Model::CreateSphere(center, 0.2f, Resources::Material::Metallic(
						glm::vec3(0.5f * (1 + random()), 0.5f * (1 + random()), 0.5f * (1 + random())),
						0.5f * random()),
						isProc));
				}
				else // Glass
				{
					models.push_back(Resources::Model::CreateSphere(center, 0.2f, Resources::Material::Dielectric(1.5f), isProc));
				}
			}
		}
	}

	models.push_back(Resources::Model::CreateSphere(glm::vec3(0, 1, 0), 1.0f, Resources::Material::Metallic(glm::vec3(1.0f), 0.1f, 2), isProc));
	models.push_back(Resources::Model::CreateSphere(glm::vec3(-4, 1, 0), 1.0f, Resources::Material::Lambertian(glm::vec3(1.0f), 0), isProc));
	models.push_back(Resources::Model::CreateSphere(glm::vec3(4, 1, 0), 1.0f, Resources::Material::Metallic(glm::vec3(1.0f), 0.0f, 1), isProc));

	textures.push_back(Resources::Texture::LoadTexture("../Assets/Textures/2k_mars.jpg", Vulkan::SamplerConfiguration()));
	textures.push_back(Resources::Texture::LoadTexture("../Assets/Textures/2k_moon.jpg", Vulkan::SamplerConfiguration()));
	textures.push_back(Resources::Texture::LoadTexture("../Assets/Textures/land_ocean_ice_cloud_2048.png", Vulkan::SamplerConfiguration()));

	return std::forward_as_tuple(std::move(models), std::move(textures));
}

SceneAssets SceneList::LucyInOneWeekend(CameraInitialState& cameraState)
{
	// Same as RayTracingInOneWeekend but using the Lucy 3D model.
	cameraState.m_ModelView = glm::lookAt(glm::vec3(13, 2, 3), glm::vec3(0, 1.0, 0), glm::vec3(0, 1, 0));
	cameraState.m_FieldOfView = 20;
	cameraState.m_Aperture = 0.05f;
	cameraState.m_FocusDistance = 10.0f;
	cameraState.m_ControlSpeed = 5.0f;
	cameraState.m_GammaCorrection = true;
	cameraState.m_HasSky = true;

	const bool isProc = true;

	std::mt19937 engine(42);
	auto random = std::bind(std::uniform_real_distribution<float>(), engine);

	std::vector<Resources::Model> models;

	models.push_back(Resources::Model::CreateSphere(glm::vec3(0, -1000, 0), 1000, Resources::Material::Lambertian(glm::vec3(0.5f, 0.5f, 0.5f)), isProc));

	for (int a = -11; a < 11; ++a)
	{
		for (int b = -11; b < 11; ++b)
		{
			const float chooseMat = random();
			const glm::vec3 center(a + 0.9f * random(), 0.2f, b + 0.9f * random());

			if (glm::length(center - glm::vec3(4, 0.2f, 0)) > 0.9)
			{
				if (chooseMat < 0.8f) // Diffuse
				{
					models.push_back(Resources::Model::CreateSphere(center, 0.2f, Resources::Material::Lambertian(glm::vec3(
						random() * random(),
						random() * random(),
						random() * random())),
						isProc));
				}
				else if (chooseMat < 0.95f) // Metal
				{
					models.push_back(Resources::Model::CreateSphere(center, 0.2f, Resources::Material::Metallic(
						glm::vec3(0.5f * (1 + random()), 0.5f * (1 + random()), 0.5f * (1 + random())),
						0.5f * random()),
						isProc));
				}
				else // Glass
				{
					models.push_back(Resources::Model::CreateSphere(center, 0.2f, Resources::Material::Dielectric(1.5f), isProc));
				}
			}
		}
	}

	auto lucy0 = Resources::Model::LoadModel("../Assets/Models/lucy.obj");
	auto lucy1 = lucy0;
	auto lucy2 = lucy0;

	const auto i = glm::mat4(1);
	const float scaleFactor = 0.0035f;

	lucy0.SetTransform(
		glm::rotate(
			glm::scale(
				glm::translate(i, glm::vec3(0, -0.08f, 0)),
				glm::vec3(scaleFactor)),
			glm::radians(90.0f), glm::vec3(0, 1, 0)));

	lucy1.SetTransform(
		glm::rotate(
			glm::scale(
				glm::translate(i, glm::vec3(-4, -0.08f, 0)),
				glm::vec3(scaleFactor)),
			glm::radians(90.0f), glm::vec3(0, 1, 0)));

	lucy2.SetTransform(
		glm::rotate(
			glm::scale(
				glm::translate(i, glm::vec3(4, -0.08f, 0)),
				glm::vec3(scaleFactor)),
			glm::radians(90.0f), glm::vec3(0, 1, 0)));

	lucy0.SetMaterial(Resources::Material::Dielectric(1.5f));
	lucy1.SetMaterial(Resources::Material::Lambertian(glm::vec3(0.4f, 0.2f, 0.1f)));
	lucy2.SetMaterial(Resources::Material::Metallic(glm::vec3(0.7f, 0.6f, 0.5f), 0.05f));

	models.push_back(std::move(lucy0));
	models.push_back(std::move(lucy1));
	models.push_back(std::move(lucy2));

	return std::forward_as_tuple(std::move(models), std::vector<Resources::Texture>());
}

SceneAssets SceneList::CornellBoxLucy(CameraInitialState& cameraState)
{
	cameraState.m_ModelView = glm::lookAt(glm::vec3(278, 278, 800), glm::vec3(278, 278, 0), glm::vec3(0, 1, 0));
	cameraState.m_FieldOfView = 40;
	cameraState.m_Aperture = 0.0f;
	cameraState.m_FocusDistance = 10.0f;
	cameraState.m_ControlSpeed = 500.0f;
	cameraState.m_GammaCorrection = true;
	cameraState.m_HasSky = false;

	const auto i = glm::mat4(1);
	const auto sphere = Resources::Model::CreateSphere(glm::vec3(555 - 130, 165.0f, -165.0f / 2 - 65), 80.0f, Resources::Material::Dielectric(1.5f), true);
	auto lucy0 = Resources::Model::LoadModel("../Assets/Models/lucy.obj");

	lucy0.SetTransform(
		glm::rotate(
			glm::scale(
				glm::translate(i, glm::vec3(555 - 300 - 165 / 2, -9, -295 - 165 / 2)),
				glm::vec3(0.6f)),
			glm::radians(75.0f), glm::vec3(0, 1, 0)));

	std::vector<Resources::Model> models;
	models.push_back(Resources::Model::CreateCornellBox(555));
	models.push_back(sphere);
	models.push_back(lucy0);

	return std::forward_as_tuple(std::move(models), std::vector<Resources::Texture>());
}
