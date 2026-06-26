// AI acknowledgment: Generative AI (Claude) was used to assist with structuring
// the lighting, material, and scene layout code. Reviewed and verified by Luca Formento.

///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// manage the loading and rendering of 3D scenes
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//  Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

namespace
{
	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";
}

SceneManager::SceneManager(ShaderManager* pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
	for (int i = 0; i < 16; i++)
	{
		m_textureIDs[i].tag = "/0";
		m_textureIDs[i].ID = -1;
	}
	m_loadedTextures = 0;
}

SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
	DestroyGLTextures();
}

bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0, height = 0, colorChannels = 0;
	GLuint textureID = 0;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* image = stbi_load(filename, &width, &height, &colorChannels, 0);
	if (image)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0);
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;
		return true;
	}
	return false;
}

void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
		glGenTextures(1, &m_textureIDs[i].ID);
}

int SceneManager::FindTextureID(std::string tag)
{
	for (int i = 0; i < m_loadedTextures; i++)
		if (m_textureIDs[i].tag.compare(tag) == 0) return m_textureIDs[i].ID;
	return -1;
}

int SceneManager::FindTextureSlot(std::string tag)
{
	for (int i = 0; i < m_loadedTextures; i++)
		if (m_textureIDs[i].tag.compare(tag) == 0) return i;
	return -1;
}

bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	for (int i = 0; i < (int)m_objectMaterials.size(); i++)
	{
		if (m_objectMaterials[i].tag.compare(tag) == 0)
		{
			material = m_objectMaterials[i];
			return true;
		}
	}
	return false;
}

void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees, float YrotationDegrees, float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	glm::mat4 scale = glm::scale(scaleXYZ);
	glm::mat4 rotX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1, 0, 0));
	glm::mat4 rotY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0, 1, 0));
	glm::mat4 rotZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0, 0, 1));
	glm::mat4 trans = glm::translate(positionXYZ);
	glm::mat4 modelView = trans * rotX * rotY * rotZ * scale;
	if (NULL != m_pShaderManager)
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
}

void SceneManager::SetShaderColor(float r, float g, float b, float a)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, glm::vec4(r, g, b, a));
	}
}

void SceneManager::SetShaderTexture(std::string textureTag)
{
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, FindTextureSlot(textureTag));
	}
}

void SceneManager::SetTextureUVScale(float u, float v)
{
	if (NULL != m_pShaderManager)
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
}

/***********************************************************
 *  SetShaderMaterial()
 *  Passes Phong material properties to the fragment shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		if (FindMaterial(materialTag, material))
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/***********************************************************
 *  DefineObjectMaterials()
 *  Phong surface materials for every object in the living
 *  room: wood floor, metal lamp, fabric shade, dark TV,
 *  and painted wood furniture.
 ***********************************************************/
void SceneManager::DefineObjectMaterials()
{
	// warm rustic wood floor
	OBJECT_MATERIAL woodMaterial;
	woodMaterial.ambientColor = glm::vec3(0.25f, 0.18f, 0.10f);
	woodMaterial.ambientStrength = 0.3f;
	woodMaterial.diffuseColor = glm::vec3(0.45f, 0.32f, 0.18f);
	woodMaterial.specularColor = glm::vec3(0.15f, 0.12f, 0.08f);
	woodMaterial.shininess = 8.0f;
	woodMaterial.tag = "wood";
	m_objectMaterials.push_back(woodMaterial);

	// brushed stainless lamp body
	OBJECT_MATERIAL metalMaterial;
	metalMaterial.ambientColor = glm::vec3(0.20f, 0.20f, 0.22f);
	metalMaterial.ambientStrength = 0.4f;
	metalMaterial.diffuseColor = glm::vec3(0.40f, 0.40f, 0.45f);
	metalMaterial.specularColor = glm::vec3(0.70f, 0.70f, 0.75f);
	metalMaterial.shininess = 48.0f;
	metalMaterial.tag = "metal";
	m_objectMaterials.push_back(metalMaterial);

	// soft fabric lamp shade
	OBJECT_MATERIAL shadeMaterial;
	shadeMaterial.ambientColor = glm::vec3(0.40f, 0.34f, 0.25f);
	shadeMaterial.ambientStrength = 0.5f;
	shadeMaterial.diffuseColor = glm::vec3(0.60f, 0.52f, 0.38f);
	shadeMaterial.specularColor = glm::vec3(0.05f, 0.05f, 0.05f);
	shadeMaterial.shininess = 2.0f;
	shadeMaterial.tag = "shade";
	m_objectMaterials.push_back(shadeMaterial);

	// dark matte TV screen
	OBJECT_MATERIAL tvMaterial;
	tvMaterial.ambientColor = glm::vec3(0.05f, 0.05f, 0.05f);
	tvMaterial.ambientStrength = 0.1f;
	tvMaterial.diffuseColor = glm::vec3(0.08f, 0.08f, 0.08f);
	tvMaterial.specularColor = glm::vec3(0.40f, 0.40f, 0.40f);
	tvMaterial.shininess = 64.0f;
	tvMaterial.tag = "tv";
	m_objectMaterials.push_back(tvMaterial);

	// painted wood furniture (TV stand, bookshelf)
	OBJECT_MATERIAL furnitureMaterial;
	furnitureMaterial.ambientColor = glm::vec3(0.20f, 0.14f, 0.08f);
	furnitureMaterial.ambientStrength = 0.25f;
	furnitureMaterial.diffuseColor = glm::vec3(0.40f, 0.28f, 0.14f);
	furnitureMaterial.specularColor = glm::vec3(0.20f, 0.16f, 0.10f);
	furnitureMaterial.shininess = 12.0f;
	furnitureMaterial.tag = "furniture";
	m_objectMaterials.push_back(furnitureMaterial);
}

/***********************************************************
 *  SetupSceneLights()
 *  Warm point light at the lamp (colored), cool fill light
 *  from front-right keeps everything out of shadow.
 ***********************************************************/
void SceneManager::SetupSceneLights()
{
	// Light 0: warm point light at the lamp shade position
	m_pShaderManager->setVec3Value("lightSources[0].position", -7.0f, 5.7f, 2.0f);
	m_pShaderManager->setVec3Value("lightSources[0].ambientColor", 0.06f, 0.05f, 0.03f);
	m_pShaderManager->setVec3Value("lightSources[0].diffuseColor", 1.00f, 0.82f, 0.55f);
	m_pShaderManager->setVec3Value("lightSources[0].specularColor", 0.60f, 0.50f, 0.35f);
	m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 24.0f);
	m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.4f);

	// Light 1: cool blue-white fill, front-right and high
	m_pShaderManager->setVec3Value("lightSources[1].position", 8.0f, 9.0f, 8.0f);
	m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 0.04f, 0.04f, 0.06f);
	m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.38f, 0.40f, 0.50f);
	m_pShaderManager->setVec3Value("lightSources[1].specularColor", 0.30f, 0.30f, 0.38f);
	m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 16.0f);
	m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.25f);

	m_pShaderManager->setBoolValue("bUseLighting", true);
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/**************************************************************/

/***********************************************************
 *  PrepareScene()
 *  Loads textures, defines materials, sets up lights, and
 *  loads every mesh used in the living room scene.
 ***********************************************************/
void SceneManager::PrepareScene()
{
	CreateGLTexture("../../Utilities/textures/rusticwood.jpg", "rusticwood");
	CreateGLTexture("../../Utilities/textures/stainless.jpg", "stainless");
	CreateGLTexture("../../Utilities/textures/abstract.jpg", "abstract");
	CreateGLTexture("../../Utilities/textures/drywall.jpg", "drywall");

	BindGLTextures();
	DefineObjectMaterials();
	SetupSceneLights();

	// primitive types: plane, cylinder, tapered cylinder, box (4 types, hits rubric)
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadTaperedCylinderMesh();
	m_basicMeshes->LoadBoxMesh();
}

/***********************************************************
 *  RenderScene()
 *  Draws the living room: floor, lamp (3 shapes), TV stand,
 *  TV, and bookshelf. Five objects, four primitive types,
 *  four textures, two lights, full Phong on every surface.
 ***********************************************************/
void SceneManager::RenderScene()
{
	glm::vec3 scaleXYZ;
	float XrotationDegrees = 0.0f;
	float YrotationDegrees = 0.0f;
	float ZrotationDegrees = 0.0f;
	glm::vec3 positionXYZ;

	// OBJECT 1: FLOOR PLANE - tiled rustic wood
	scaleXYZ = glm::vec3(20.0f, 1.0f, 14.0f);
	XrotationDegrees = 0.0f; YrotationDegrees = 0.0f; ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderTexture("rusticwood"); SetTextureUVScale(4.0f, 4.0f);
	SetShaderMaterial("wood");
	m_basicMeshes->DrawPlaneMesh();

	// OBJECT 2: LAMP BASE - stainless metal, wide short cylinder
	scaleXYZ = glm::vec3(1.5f, 0.3f, 1.5f);
	XrotationDegrees = 0.0f; YrotationDegrees = 0.0f; ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-7.0f, 0.0f, 2.0f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderTexture("stainless"); SetTextureUVScale(1.0f, 1.0f);
	SetShaderMaterial("metal");
	m_basicMeshes->DrawCylinderMesh();

	// LAMP POLE - stainless metal, thin tall cylinder
	scaleXYZ = glm::vec3(0.18f, 4.5f, 0.18f);
	XrotationDegrees = 0.0f; YrotationDegrees = 0.0f; ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-7.0f, 0.3f, 2.0f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderTexture("stainless"); SetTextureUVScale(1.0f, 2.0f);
	SetShaderMaterial("metal");
	m_basicMeshes->DrawCylinderMesh();

	// LAMP SHADE - abstract texture tiled, soft fabric material
	scaleXYZ = glm::vec3(1.8f, 1.6f, 1.8f);
	XrotationDegrees = 0.0f; YrotationDegrees = 0.0f; ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(-7.0f, 4.1f, 2.0f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderTexture("abstract"); SetTextureUVScale(2.0f, 2.0f);
	SetShaderMaterial("shade");
	m_basicMeshes->DrawTaperedCylinderMesh();

	// OBJECT 3: TV STAND - low wide box, painted furniture material
	scaleXYZ = glm::vec3(8.0f, 1.5f, 2.5f);
	XrotationDegrees = 0.0f; YrotationDegrees = 0.0f; ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(2.0f, 0.75f, -3.0f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderTexture("drywall"); SetTextureUVScale(2.0f, 1.0f);
	SetShaderMaterial("furniture");
	m_basicMeshes->DrawBoxMesh();

	// OBJECT 4: TV - flat dark box, glossy screen material
	scaleXYZ = glm::vec3(7.0f, 4.0f, 0.3f);
	XrotationDegrees = 0.0f; YrotationDegrees = 0.0f; ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(2.0f, 3.5f, -3.5f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderColor(0.05f, 0.05f, 0.05f, 1.0f);
	SetShaderMaterial("tv");
	m_basicMeshes->DrawBoxMesh();

	// OBJECT 5: BOOKSHELF - tall narrow box, painted furniture material
	scaleXYZ = glm::vec3(3.0f, 7.0f, 2.0f);
	XrotationDegrees = 0.0f; YrotationDegrees = 0.0f; ZrotationDegrees = 0.0f;
	positionXYZ = glm::vec3(8.0f, 3.5f, -1.0f);
	SetTransformations(scaleXYZ, XrotationDegrees, YrotationDegrees, ZrotationDegrees, positionXYZ);
	SetShaderTexture("drywall"); SetTextureUVScale(1.0f, 2.0f);
	SetShaderMaterial("furniture");
	m_basicMeshes->DrawBoxMesh();
}