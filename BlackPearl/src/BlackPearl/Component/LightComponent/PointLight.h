#pragma once
#include "Light.h"
#include <glm/glm.hpp>
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
namespace BlackPearl {

	class PointLight :public Light
	{
	public:
		struct Attenuation {
			unsigned int maxDistance;
			float constant;
			float linear;
			float quadratic;
			Attenuation() :maxDistance(50), constant(1.0f), linear(0.09f), quadratic(0.032) {}
			Attenuation(unsigned int maxDistance) :maxDistance(maxDistance) {
				switch (maxDistance)
				{
				case 7:
					constant = 1.0f, linear = 0.7f, quadratic = 1.8f;
					break;
				case 13:
					constant = 1.0f, linear = 0.35f, quadratic = 0.44f;
					break;
				case 20:
					constant = 1.0f, linear = 0.22f, quadratic = 0.20f;
					break;
				case 32:
					constant = 1.0f, linear = 0.14f, quadratic = 0.07f;
					break;
				case 50:
					constant = 1.0f, linear = 0.09f, quadratic = 0.032f;
					break;
				case 65:
					constant = 1.0f, linear = 0.07f, quadratic = 0.017f;
					break;
				case 100:
					constant = 1.0f, linear = 0.045f, quadratic = 0.0075f;
					break;
				case 160:
					constant = 1.0f, linear = 0.0027f, quadratic = 0.0028f;
					break;
				case 200:
					constant = 1.0f, linear = 0.022f, quadratic = 0.0019f;
					break;
				case 325:
					constant = 1.0f, linear = 0.014f, quadratic = 0.0007f;
					break;
				case 600:
					constant = 1.0f, linear = 0.007f, quadratic = 0.0002f;
					break;
				case 3250:
					constant = 1.0f, linear = 0.0014f, quadratic = 0.000007f;
					break;
				default:
					GE_CORE_ERROR("Undefined maxDistance! Failed to construct Attenuation!")
						break;
				}

			}
			//Ĭ�Ͼ���50 constant(1.0f),linear(0.09f),quadratic(0.032){}
			//Ĭ�Ͼ���3250constant(1.0f),linear(0.0014f),quadratic(0.000007)
			//�����https://learnopengl-cn.github.io/02%20Lighting/05%20Light%20casters/
		};
		PointLight(EntityManager * entityManager, Entity::Id id, Props props = Props())
			:Light(entityManager,id) {
			SetProps(props);
			Init();
		}
		virtual ~PointLight() = default;
		virtual void Init() override;

		void UpdateMesh(Props props) {
			SetProps(props);
			std::shared_ptr<Material> lightMaterial = m_Mesh.GetMaterial();
			lightMaterial->SetMaterialColor({ m_LightProp.ambient,m_LightProp.diffuse,m_LightProp.specular ,m_LightProp .emission});


		}
		inline void SetAttenuation(const Attenuation& attenuation) { m_Attenuation = attenuation; }
		inline void SetAttenuation(unsigned int maxdistance) { m_Attenuation.maxDistance = maxdistance; }

		inline Attenuation GetAttenuation() const { return m_Attenuation; }
		virtual inline LightType GetType() override { return LightType::PointLight; }


		inline Mesh GetMeshes()const { return m_Mesh; }

	private:
		Mesh m_Mesh;
		Attenuation m_Attenuation;
	};

}