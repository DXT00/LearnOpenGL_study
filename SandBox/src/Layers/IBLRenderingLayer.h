#pragma once

#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class IBLRenderingLayer :public BlackPearl::Layer {
public:

	IBLRenderingLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
	{

		m_CameraObj = CreateCamera();
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		m_CameraPosition = cameraComponent->GetPosition();
		m_CameraRotation.Yaw = cameraComponent->Yaw();
		m_CameraRotation.Pitch = cameraComponent->Pitch();

		//Shader
		m_BackGroundShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/ibl/background.glsl"));
		m_DebugQuadShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/QuadDebug.glsl"));
		//Scene
		m_SphereObj = CreateSphere(0.5, 64, 64);
		m_SphereObjIron = CreateSphere(0.5, 64, 64);
		m_SphereObjRust = CreateSphere(0.5, 64, 64);
		m_SphereObjStone = CreateSphere(0.5, 64, 64);
		m_SphereObjPlastic = CreateSphere(0.5, 64, 64);
		//skybox and quad for BrdfLUTMap
		m_CubeObj = CreateCube();
		m_BrdfLUTQuadObj = CreateQuad();
		m_DebugQuad = CreateQuad();


		BlackPearl::Renderer::Init();
		glDepthFunc(GL_LEQUAL);
		//Renderer
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();
		m_PBRRenderer = DBG_NEW BlackPearl::PBRRenderer();
		m_IBLRenderer = DBG_NEW BlackPearl::IBLRenderer();




		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_PBRRenderer->GetShader());

		//textures spheres
		std::shared_ptr<BlackPearl::Texture> RustalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/rustSphere/rustediron2_basecolor.png"));
		std::shared_ptr<BlackPearl::Texture> RustaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/rustSphere/rustediron2_ao.png"));
		std::shared_ptr<BlackPearl::Texture> RustroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/rustSphere/rustediron2_roughness.png"));
		std::shared_ptr<BlackPearl::Texture> RustmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/rustSphere/rustediron2_metallic.png"));
		std::shared_ptr<BlackPearl::Texture> RustnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/rustSphere/rustediron2_normal.png"));

		
		std::shared_ptr<BlackPearl::Texture> IronalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_basecolor.png"));
		std::shared_ptr<BlackPearl::Texture> IronaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_ao.png"));
		std::shared_ptr<BlackPearl::Texture> IronroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_roughness.png"));
		std::shared_ptr<BlackPearl::Texture> IronmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_metallic.png"));
		std::shared_ptr<BlackPearl::Texture> IronnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/IronScuffed/Iron-Scuffed_normal.png"));

		std::shared_ptr<BlackPearl::Texture> StonealbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_albedo.png"));
		std::shared_ptr<BlackPearl::Texture> StoneaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_ao.png"));
		std::shared_ptr<BlackPearl::Texture> StoneroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_roughness.png"));
		std::shared_ptr<BlackPearl::Texture> StonementallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_metallic.png"));
		std::shared_ptr<BlackPearl::Texture> StonenormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/cobblestone/cobblestone-curved_2_normal-dx.png"));

		std::shared_ptr<BlackPearl::Texture> PlasticalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/pbr/plasticSphere/scuffed-plastic4-alb.png"));
		std::shared_ptr<BlackPearl::Texture> PlasticaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-ao.png"));
		std::shared_ptr<BlackPearl::Texture> PlasticroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-rough.png"));
		std::shared_ptr<BlackPearl::Texture> PlasticmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-metal.png"));
		std::shared_ptr<BlackPearl::Texture> PlasticnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/texture/pbr/plasticSphere/scuffed-plastic-normal.png"));

		m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronnormalTexture);
		m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronalbedoTexture);
		m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronaoTexture);
		m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronroughnessTexture);
		m_SphereObjIron->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(IronmentallicTexture);

		m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustnormalTexture);
		m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustalbedoTexture);
		m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustaoTexture);
		m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustroughnessTexture);
		m_SphereObjRust->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RustmentallicTexture);

		m_SphereObjRust->GetComponent<BlackPearl::Transform>()->SetPosition({ 1.5,0,0 });

		m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonenormalTexture);
		m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonealbedoTexture);
		m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StoneaoTexture);
		m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StoneroughnessTexture);
		m_SphereObjStone->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(StonementallicTexture);

		m_SphereObjStone->GetComponent<BlackPearl::Transform>()->SetPosition({ -1.5,0,0 });

		m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticnormalTexture);
		m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticalbedoTexture);
		m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticaoTexture);
		m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticroughnessTexture);
		m_SphereObjPlastic->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(PlasticmentallicTexture);
		
		m_SphereObjPlastic->GetComponent<BlackPearl::Transform>()->SetPosition({ -3.0,0,0 });
		/*Draw CubeMap from hdrMap and Create environment IrrdianceMap*/
		m_IBLRenderer->Init(m_CubeObj, m_BrdfLUTQuadObj);

		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);

	}

	virtual ~IBLRenderingLayer() {

		DestroyObjects();
		delete m_BasicRenderer;
		delete m_IBLRenderer;
		delete m_PBRRenderer;

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		/*render scene*/
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		/*Draw Lights*/
		m_BasicRenderer->DrawLightSources(GetLightSources());
		
		/*IBL rendering*/
		//m_IBLRenderer->RenderSpheres(m_SphereObj);

		m_IBLRenderer->RenderTextureSphere(m_SphereObjIron);
		m_IBLRenderer->RenderTextureSphere(m_SphereObjRust);
		m_IBLRenderer->RenderTextureSphere(m_SphereObjStone);
		m_IBLRenderer->RenderTextureSphere(m_SphereObjPlastic);



		/*Draw SkyBox*/
		m_BackGroundShader->Bind();
		m_BackGroundShader->SetUniform1i("cubeMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_IBLRenderer->GetHdrCubeMapID());
		m_BasicRenderer->DrawObject(m_CubeObj, m_BackGroundShader);
	
		/*draw BRDFLUTTextureID in a quad*/
		glViewport(0, 0, 120, 120);
		m_DebugQuadShader->Bind();
		m_DebugQuadShader->SetUniform1i("u_BRDFLUTMap",5);
		glActiveTexture(GL_TEXTURE0 + 5);
		glBindTexture(GL_TEXTURE_2D, m_IBLRenderer->GetBRDFLUTTextureID());
		m_BasicRenderer->DrawObject(m_DebugQuad, m_DebugQuadShader);


		//m_IBLRenderer->DrawBRDFLUTMap();

	}

	void OnAttach() override {


	}
	void InputCheck(float ts)
	{
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_W)) {
			m_CameraPosition += cameraComponent->Front() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_S)) {
			m_CameraPosition -= cameraComponent->Front()* m_CameraMoveSpeed * ts;
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_A)) {
			m_CameraPosition -= cameraComponent->Right() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_D)) {
			m_CameraPosition += cameraComponent->Right() * m_CameraMoveSpeed * ts;
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_E)) {
			m_CameraPosition += cameraComponent->Up() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_Q)) {
			m_CameraPosition -= cameraComponent->Up() * m_CameraMoveSpeed * ts;
		}
		// ---------------------Rotation--------------------------------------

		float posx = BlackPearl::Input::GetMouseX();
		float posy = BlackPearl::Input::GetMouseY();
		if (BlackPearl::Input::IsMouseButtonPressed(BP_MOUSE_BUTTON_RIGHT)) {


			if (BlackPearl::Input::IsFirstMouse()) {
				BlackPearl::Input::SetFirstMouse(false);
				m_LastMouseX = posx;
				m_LastMouseY = posy;
			}
			float diffx = posx - m_LastMouseX;
			float diffy = -posy + m_LastMouseY;

			m_LastMouseX = posx;
			m_LastMouseY = posy;
			m_CameraRotation.Yaw += diffx * m_CameraRotateSpeed * ts;
			m_CameraRotation.Pitch += diffy * m_CameraRotateSpeed * ts;

			if (m_CameraRotation.Pitch > 89.0f)
				m_CameraRotation.Pitch = 89.0f;
			if (m_CameraRotation.Pitch < -89.0f)
				m_CameraRotation.Pitch = -89.0f;

			cameraComponent->SetRotation(m_CameraRotation.Yaw, m_CameraRotation.Pitch);

		}
		else {

			m_LastMouseX = posx;//lastMouseʱ�̼�¼��ǰ����λ�ã���ֹ�ٴε���Ҽ�ʱ������������
			m_LastMouseY = posy;
		}


		cameraComponent->SetPosition(m_CameraPosition);
	}
private:
	//Scene

	BlackPearl::Object* m_CameraObj = nullptr;

	BlackPearl::Object* m_CubeObj = nullptr;
	BlackPearl::Object* m_DebugQuad = nullptr;
	BlackPearl::Object* m_SphereObj = nullptr;
	BlackPearl::Object* m_SphereObjIron = nullptr;
	BlackPearl::Object* m_SphereObjRust = nullptr;
	BlackPearl::Object* m_SphereObjStone = nullptr;
	BlackPearl::Object* m_SphereObjPlastic = nullptr;

	BlackPearl::Object* m_BrdfLUTQuadObj = nullptr;
	int m_Rows = 4;
	int m_Colums = 4;
	float m_Spacing = 1.5;

	//Camera
	glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
	struct CameraRotation {
		float Yaw;
		float Pitch;

	};
	CameraRotation m_CameraRotation;
	float m_LastMouseX;
	float m_LastMouseY;
	float m_CameraMoveSpeed = 3.0f;
	float m_CameraRotateSpeed = 3.0f;

	//Shader
	std::shared_ptr<BlackPearl::Shader> m_BackGroundShader;
	std::shared_ptr<BlackPearl::Shader> m_DebugQuadShader;
	//Renderer
	BlackPearl::PBRRenderer* m_PBRRenderer;
	BlackPearl::IBLRenderer* m_IBLRenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;


};