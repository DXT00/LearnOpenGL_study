#pragma once
#include "BlackPearl/Renderer/LightComponent/Light.h"
#include "BlackPearl/Layer.h"
#include "BlackPearl/LayerStack.h"
#include "BlackPearl/ImGui/ImGuiLayer.h"
#include "BlackPearl/Timestep/Timestep.h"
#include "BlackPearl/Entity/Entity.h"


namespace BlackPearl {

	class Scene {
	public:
		Scene() {
			EntityManager *ImGuiEntityMgr = DBG_NEW EntityManager();
			m_ImGuiLayer = DBG_NEW ImGuiLayer("ImGuiLayer", ImGuiEntityMgr);
			PushOverLayer(m_ImGuiLayer);
		}
		~Scene();

		void OnUpdateLayers(Timestep ts);
		void PushLayer(Layer *layer);
		void PushOverLayer(Layer *overlay);

	private:

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer; //每个Scene都有一个ImGuiLayer
	};



}
