#pragma once
#include "BlackPearl/Entity/Entity.h"
#include "BlackPearl/Component/BaseComponent.h"
#include "BlackPearl/Component/Component.h"

#include <string>
namespace BlackPearl {

	class Object :public Entity
	{
	public:
		Object(EntityManager* manager, Entity::Id id, std::string name = "")
			:Entity(manager, id) {
			m_FrontName = (name == "") ? "EmptyObject" : name;
			m_Name = (name == "") ? "EmptyObject(" + std::to_string(id.index()) + ")" : name+ "(" + std::to_string(id.index()) + ")";
			m_TransformToParent = m_InvTransformToParent = glm::mat3(1.0);

		};

		virtual ~Object() {
			m_ComponentMask.reset();
			if (m_EntityManager != nullptr)
				m_EntityManager->DestroyEntity(m_Id);
			m_Components.clear();
			
		};

		inline std::string GetName() { return m_Name; }
		inline std::string GetFrontName() { return m_FrontName; }

		inline void SetName(std::string name) { m_Name = name; }
		inline bool Vaild() {
			return m_EntityManager && m_EntityManager->Valid(m_Id);
		}
		Configuration::ComponentMask GetComponentMask()const;
		std::unordered_map<BaseComponent::Family, std::shared_ptr<BaseComponent>> GetComponentList() { return m_Components; }

		template<typename C, typename ...Args>
		std::shared_ptr<C> AddComponent(Args ...args)
		{
			//GE_ASSERT(Valid(), "invalid entity" + std::to_string(m_Id.index())); //TODO::https://bbs.csdn.net/topics/391862079
			//C* component = m_EntityManager->AddComponent(m_Id, args);

			const BaseComponent::Family family = GetComponentFamliy<C>();
			//Returns whether the bit at family pos is set (i.e., whether it is one).
			GE_ASSERT(!m_ComponentMask.test(family), "Component already existed!")


				m_ComponentMask.set(family);

			//所有的component都用Create实例化-->方便子类继承！
			//C* component = C::Create(m_EntityManager, m_Id, std::forward<Args>(args)...);
			//C* component = DBG_NEW C(m_EntityManager, m_Id, std::forward<Args>(args)...);
			std::shared_ptr<C> componentSharePtr;
			componentSharePtr.reset(DBG_NEW C(m_EntityManager, m_Id, std::forward<Args>(args)...));
			m_Components.insert(std::make_pair(C::Famliy(), componentSharePtr));
			return componentSharePtr;

		}

		template<typename C>
		C* GetComponent() 
		{
			GE_ASSERT(this!=nullptr, "this object is nullptr!");
			std::unordered_map<BaseComponent::Family, std::shared_ptr<BaseComponent>>::const_iterator it = m_Components.find(C::Famliy());
			GE_ASSERT(HasComponent<C>() && it != m_Components.end(), "Entity dose not have component C!")
				//	return ComponentHandle<C>(m_EntityManager, m_Id, m_Components[C::Famliy()]);
			//Component<C> * component = std::dynamic_pointer_cast<Component<C>>(it->second);
			   // C* result = std::dynamic_cast<C*>(it->second);
				return std::dynamic_pointer_cast<C>(it->second).get();
		}

		template<typename C>
		bool HasComponent() const
		{
			//GE_ASSERT(Valid(), "invalid entity" + m_Id);
			size_t famliy = GetComponentFamliy<C>();
			if (m_ComponentMask[famliy])
				return true;
			else
				return false;
		}

		template<typename C>
		void RemoveComponent()
		{
			GE_ASSERT(Valid(), "invalid entity" + m_Id);
			GE_ASSERT(HasComponent<C>(), "Entity does not has component C!");//TODO
			const BaseComponent::Family famliy = GetComponentFamliy<C>();
			m_ComponentMask.reset(famliy);
			m_Components[C::Famliy()] = nullptr;

		}

		template<typename C>
		static BaseComponent::Family  GetComponentFamliy()
		{
			return Component<typename std::remove_const<C>::type>::Famliy();
			//return C::Famliy();
		}

		virtual void Destroy() override;


		void SetPosition(glm::vec3 pos);
		void SetRotation(glm::vec3 rotate);
		void SetScale(glm::vec3 scale);

		void SetParent(Object* parent) { m_ParentObj = parent; }
		Object* GetParent()const { return m_ParentObj; }

		void SetTransformToParent(glm::mat4 transformToParent) {
			m_TransformToParent    = transformToParent; 
			m_InvTransformToParent = glm::inverse(m_TransformToParent);
		}
		glm::mat4 GetTransformToParent()const { return m_TransformToParent; }
		glm::mat4 GetInvTransformToParent()const { return m_InvTransformToParent; }

		/*Child Objs*/
		void AddChildObj(Object* obj);
		std::vector<Object*> GetChildObjs()const { return m_ChildObjs; }
		Object* GetChildByFrontName(std::string name)const;
	private:
		std::unordered_map<BaseComponent::Family, std::shared_ptr<BaseComponent>> m_Components;
		Configuration::ComponentMask m_ComponentMask;
		std::string m_Name;
		std::string m_FrontName;

		glm::mat4 m_TransformToParent;
		glm::mat4 m_InvTransformToParent;
		Object* m_ParentObj = nullptr;
		std::vector<Object*> m_ChildObjs;
	};


}

