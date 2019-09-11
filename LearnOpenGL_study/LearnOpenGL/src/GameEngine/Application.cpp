#include "pch.h"
#include "Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer/Texture/Texture.h"
#include "Renderer/Camera/PerspectiveCamera.h"
#include "Renderer/Buffer.h"
#include "Renderer/Renderer.h"

#include "Core.h"
#include "Input.h"
#include "Event/Event.h"
#include "Event/MouseEvent.h"

Application* Application::s_Instance = nullptr;
Application::Application()
{
	GE_ASSERT(!s_Instance, "Application's Instance already exist!")
	s_Instance = this;
	m_Window.reset(new Window());
	m_Window->SetCallBack(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	m_Camera.reset(Camera::Create(Camera::Perspective, { 45.0f, 800.0f, 600.0f, 0.1f, 100.0f }));
	m_LightSource.reset(new LightSource());
	Renderer::Init();

	float vertices[] = {
		// positions          // texture coords  //normal
		 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
										    
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
										    
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,
										    
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
										    
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.0f, -1.0f, 0.0f,
										    
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f, 1.0f, 0.0f
	};
	
	m_VertexArray.reset(new VertexArray());

	std::shared_ptr<VertexBuffer> vertexBuffer;
	vertexBuffer.reset(new VertexBuffer(vertices, sizeof(vertices)));


	VertexBufferLayout layout = {
		{ElementDataType::Float3,"aPos",false},
		{ElementDataType::Float2,"aTexCoord",false},
		{ElementDataType::Float3,"aNormal",false}
	};
	vertexBuffer->SetBufferLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);




	// load and create a texture 
	m_Texture1.reset(new Texture("F:\\OpenGL\\LearnOpenGL\\LearnOpenGL\\src\\GameEngine\\Renderer\\Texture\\container.jpg"));
	m_Texture2.reset(new Texture("F:\\OpenGL\\LearnOpenGL\\LearnOpenGL\\src\\GameEngine\\Renderer\\Texture\\1.jpg"));


	//Shader
	const std::string vertexSrc = R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;
		layout(location = 1) in vec2 aTexCoord;
		layout(location = 2) in vec3 aNormal;
		
		out vec2 v_TexCoord;
		out vec3 v_Normal;
		out vec3 v_FragPos;

		uniform mat4 u_Model;
		uniform mat4 u_TranInverseModel;//transpose(inverse(u_Model))-->最好在cpu运算完再传进来!
		uniform mat4 u_ProjectionView;
	

		void main()
		{
			gl_Position = u_ProjectionView * u_Model * vec4(aPos,1.0);
			v_FragPos = vec3(u_Model* vec4(aPos,1.0));
			v_TexCoord = vec2(aTexCoord.x,aTexCoord.y);
			v_Normal =  mat3(u_TranInverseModel)* aNormal;//vec3(u_Model * vec4(aNormal,1.0));
		}

	)";

	const std::string fragmentSrc = R"(
		#version 330 core
		out vec4 FragColor;
		
		float ambientStrength = 0.1f;
		float specularStrength = 0.8f;

		in vec2 v_TexCoord;
		in vec3 v_Normal;		
		in vec3 v_FragPos;		

		uniform sampler2D u_Texture1;
		uniform sampler2D u_Texture2;
		uniform float u_MixValue;

		uniform vec3 u_LightColor;
		uniform vec3 u_LightPos;
		uniform vec3 u_CameraViewPos;

		vec3 ambient = ambientStrength * u_LightColor;

		vec3 lightDir = normalize(u_LightPos-v_FragPos);
		vec3 norm = normalize(v_Normal);
		float theta = max(dot(lightDir,norm),0.0f);
		vec3 diffuse =  theta * u_LightColor;
		
		vec3 reflectDir = normalize(reflect(-lightDir,norm));
		vec3 viewDir = normalize(u_CameraViewPos-v_FragPos);
		float spec = pow(max(dot(reflectDir,viewDir),0.0),32);
		vec3 specular = specularStrength * spec * u_LightColor;
		

		void main(){
			FragColor = vec4((diffuse + ambient + specular),1.0) * mix(texture(u_Texture1, v_TexCoord), texture(u_Texture2, vec2(1.0 - v_TexCoord.x, v_TexCoord.y)), u_MixValue);
		}
	)";

	m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
	m_Shader->Bind();
	m_Shader->SetUniform1i("u_Texture1", 0);
	m_Shader->SetUniform1i("u_Texture2", 1);
	m_Shader->SetUniform1f("u_MixValue", 0.5);
	m_Shader->SetUniformVec3f("u_LightColor", m_LightSource->GetLightColor());
	m_Shader->SetUniformVec3f("u_LightPos", m_LightSource->GetPosition());

	m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
	m_CameraPosition = m_Camera->GetPosition();
	m_CameraRotation.Yaw = m_Camera->Yaw();
	m_CameraRotation.Pitch = m_Camera->Pitch();


}

Application::~Application()
{

}

void Application::Run()
{
	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	//render loop
	while (!glfwWindowShouldClose(m_Window->GetNativeWindow())) {

		float time = (float)glfwGetTime();
		Timestep ts = time - m_LastFrameTime;
		m_LastFrameTime = time;

		InputCheck(ts);
		if (glfwGetKey(m_Window->GetNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(m_Window->GetNativeWindow(), true);
		// render
		// ------
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		m_Texture1->Bind();
		glActiveTexture(GL_TEXTURE1);
		m_Texture2->Bind();


		
		Renderer::BeginScene(*m_Camera);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_LightSource->GetPosition());
		model = glm::scale(model, glm::vec3(0.5f));
		Renderer::Submit(m_LightSource->GetVertexArray(), m_LightSource->GetShader(), model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		for (unsigned int i = 0; i < 10; i++)
		{

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(1.0f, (float)i * 20, 0.0f));//(float)(glfwGetTime())
			m_Shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
			m_Shader->SetUniformVec3f("u_CameraViewPos", m_Camera->GetPosition());
			Renderer::Submit(m_VertexArray, m_Shader, model);
			glDrawArrays(GL_TRIANGLES, 0, 36);

		}


		glfwSwapBuffers(m_Window->GetNativeWindow());
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &m_VertexArrayID);
	glDeleteBuffers(1, &m_VertexBufferID);
	glDeleteBuffers(1, &m_IndexBufferID);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}

void Application::InputCheck(float ts)
{
	if (Input::IsKeyPressed(GLFW_KEY_W)) {
		m_CameraPosition += m_Camera->Front() * m_CameraMoveSpeed * ts;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_S)) {
		m_CameraPosition -= m_Camera->Front()* m_CameraMoveSpeed * ts;
	}
	if (Input::IsKeyPressed(GLFW_KEY_A)) {
		m_CameraPosition -= m_Camera->Right() * m_CameraMoveSpeed * ts;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_D)) {
		m_CameraPosition += m_Camera->Right() * m_CameraMoveSpeed * ts;

	}
	if (Input::IsKeyPressed(GLFW_KEY_Q)) {
		m_CameraPosition -= m_Camera->Up() * m_CameraMoveSpeed * ts;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_E)) {
		m_CameraPosition += m_Camera->Up() * m_CameraMoveSpeed * ts;

	}
	// ---------------------Rotation--------------------------------------

	float posx = Input::GetMouseX();
	float posy = Input::GetMouseY();
	if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {


		if (Input::IsFirstMouse()) {
			Input::SetFirstMouse(false);
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

		m_Camera->SetRotation(m_CameraRotation.Yaw, m_CameraRotation.Pitch);

	}
	else {

		m_LastMouseX = posx;//lastMouse时刻记录当前坐标位置，防止再次点击右键时，发生抖动！
		m_LastMouseY = posy;
	}


	m_Camera->SetPosition(m_CameraPosition);
}

void Application::OnEvent(Event& event)
{
	EventDispacher dispacher(event);

	dispacher.Dispatch<MouseMovedEvent>(std::bind(&Application::OnCameraRotate, this, std::placeholders::_1));



}

bool Application::OnCameraRotate(MouseMovedEvent & e)
{
	float posx = e.GetMouseX();
	float posy = e.GetMouseY();

	return true;
}
