#include "pch.h"
#include "PointLight.h"






void PointLight::Init()
{

	//data
	float lightVertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
	};


	std::shared_ptr<VertexBuffer> pointLightVertexBuffer;
	pointLightVertexBuffer.reset(new VertexBuffer(lightVertices, sizeof(lightVertices)));
	m_Shader.reset(new Shader("assets/shaders/PointLight.glsl"));
	m_VertexArray.reset(new VertexArray());
	pointLightVertexBuffer->SetBufferLayout({
		{ElementDataType::Float3,"aPos",false}
		});
	m_VertexArray->AddVertexBuffer(pointLightVertexBuffer);

	m_Shader->Bind();
	m_Shader->SetUniformVec3f("u_LightColor", this->GetLightProps().color);

	m_LightProp.ambient = { 0.2f, 0.2f, 0.2f };
	m_LightProp.diffuse = { 0.5f, 0.5f, 0.5f };
	m_LightProp.specular = { 1.0f, 1.0f,1.0f };
	

}
