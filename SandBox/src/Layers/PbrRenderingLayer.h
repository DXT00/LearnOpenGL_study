#pragma once
#pragma once
#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class PbrRenderingLayer :public BlackPearl::Layer {
public:

	PbrRenderingLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
	{

		m_CameraObj = CreateCamera();
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		m_CameraPosition = cameraComponent->GetPosition();
		m_CameraRotation.Yaw = cameraComponent->Yaw();
		m_CameraRotation.Pitch = cameraComponent->Pitch();
		m_SphereObj = CreateSphere(0.5, 64, 64);
		//Scene
		m_PBRRenderer = DBG_NEW BlackPearl::PBRRenderer();
		
		m_SphereObj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_PBRRenderer->GetShader());
		
		BlackPearl::Renderer::Init();
	}

	virtual ~PbrRenderingLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

	//	m_VoxelConeTracingRenderer->Render(m_ObjectsList, GetLightSources(), BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight, m_CurrentRenderingMode);
		m_PBRRenderer->GetShader()->SetUniformVec3f("u_albedo", { 0.5f, 0.0f, 0.0f });
		m_PBRRenderer->GetShader()->SetUniform1f("u_ao", 1.0f);

		for (int row = 0; row < m_Rows; row++) {
			for (int col = 0; col < m_Colums; col++) {

				m_SphereObj->GetComponent<BlackPearl::Transform>()->SetPosition({
					(col - (m_Colums / 2)) * m_Spacing,
					(row - (m_Rows / 2)) * m_Spacing,
					0.0f });
				m_PBRRenderer->GetShader()->SetUniform1f("u_metallic", 2.0*(float)row / (float)m_Rows);
				m_PBRRenderer->GetShader()->SetUniform1f("u_roughness", glm::clamp((float)col / (float)m_Colums, 0.05f, 1.0f));
				m_PBRRenderer->Render(m_SphereObj);
			}
		}


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

			m_LastMouseX = posx;//lastMouse时刻记录当前坐标位置，防止再次点击右键时，发生抖动！
			m_LastMouseY = posy;
		}


		cameraComponent->SetPosition(m_CameraPosition);
	}
private:
	//Scene
	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_CameraObj;
	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_SphereObj;

	int m_Rows = 7;
	int m_Colums = 7;
	float m_Spacing = 2.5;

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

	//BackGround Color
	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };

	//Renderer
	BlackPearl::PBRRenderer* m_PBRRenderer;
	
};
