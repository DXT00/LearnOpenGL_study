#pragma once
#include"GameEngine/Renderer/VertexArray.h"
#include "GameEngine/Renderer/Shader.h"



class LightSource
{
public:
	LightSource(const glm::vec3 &position = {2.2f,1.0f,2.0f}, const glm::vec3 &lightColor = { 1.0f,1.0f,1.0f }, const glm::vec3 &blubColor = { 1.0f,0.5f,0.31f })
		:m_Position(position),m_LightColor(lightColor),m_BlubColor(blubColor){
		Init();
	}
	inline void SetEmitColor(const glm::vec3& color) { m_LightColor = color; }
	inline void SetBlubColor(const glm::vec3& color) { m_BlubColor = color; }

	inline glm::vec3 GetPosition() { return m_Position; }
	inline glm::vec3 GetLightColor() const { return m_LightColor; }
	inline glm::vec3 GetBlubColor() const { return  m_BlubColor; }
	inline std::shared_ptr<VertexArray> GetVertexArray() const { return m_VertexArray; }
	inline std::shared_ptr<Shader> GetShader() const { return m_Shader; }
	void Init();
private:
	std::shared_ptr<VertexArray> m_VertexArray;
	std::shared_ptr<Shader> m_Shader;
	glm::vec3 m_Position;
	glm::vec3 m_LightColor;//发射的光的颜色
	glm::vec3 m_BlubColor;//灯泡颜色

};

