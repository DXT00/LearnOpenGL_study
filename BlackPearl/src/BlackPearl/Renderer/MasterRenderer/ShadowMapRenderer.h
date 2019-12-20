#pragma once
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Buffer.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Renderer/ShadowBox/ShadowBox.h"
#include "BlackPearl/Object/Object.h"
namespace BlackPearl {
	//ShadowMap的Render操作 输入数据(Objs)--->ShadowMapRenderer--->渲染ShadowMap
	class ShadowMapRenderer:public BasicRenderer
	{
	public:
		ShadowMapRenderer(Object *CameraObj) {
			m_ShadowBox.reset(DBG_NEW ShadowBox(glm::mat4(1.0f), CameraObj->GetComponent<PerspectiveCamera>()));
			m_FrameBuffer.reset(DBG_NEW FrameBuffer(s_ShadowMapWidth, s_ShadowMapHeight,
				{ FrameBuffer::Attachment::DepthTexture }, true));

		};
		~ShadowMapRenderer();
		void Render(const std::vector<Object*>&objs,  ParallelLight* sun,const std::vector<Object*>&exceptObjs);
		virtual void PrepareShaderParameters(Mesh &mesh, glm::mat4 transformMatrix , std::shared_ptr<Shader> shader,bool isLight = false)override;
		
		
		void UpdateLightOrthoProjectionMatrix(float boxWidth,float boxHeight,float boxLength);
		void UpdateLightViewMatrix(glm::vec3 lightDirection, glm::vec3 boxCenter);
		
		glm::mat4 GetLightProjectionViewMatrix() const { return m_LightProjectionViewMatrix; }
		std::shared_ptr<FrameBuffer> GetFrameBuffer() const{ return m_FrameBuffer; }
		static float s_NearPlane;
		static float s_FarPlane;
	private:
		std::shared_ptr<ShadowBox> m_ShadowBox;
		std::shared_ptr<FrameBuffer> m_FrameBuffer;
		glm::mat4 m_LightProjectionViewMatrix;
		glm::mat4 m_LightProjection, m_LightView;
		glm::vec3 m_LightPos;
		static int s_ShadowMapWidth;
		static int s_ShadowMapHeight;
		
	};
}