#pragma once
#include "BlackPearl/Entity/Entity.h"
#include "BlackPearl/Component/BaseComponent.h"
#include "BlackPearl/Component/Component.h"
namespace BlackPearl {

	class Object :public Entity
	{
	public:
		Object(EntityManager* manager, Entity::Id id)
		:Entity(manager,id){};
	
		virtual ~Object() {
			Destroy();
		};

		 bool Vaild()  {
			return m_EntityManager && m_EntityManager->Valid(m_Id);
		}
		ComponentMask GetComponentMask()const;

		
		template<typename C, typename ...Args>
		C* AddComponent(Args ...args)
		{
			//GE_ASSERT(Valid(), "invalid entity" + std::to_string(m_Id.index())); //TODO::https://bbs.csdn.net/topics/391862079
			//C* component = m_EntityManager->AddComponent(m_Id, args);

			const BaseComponent::Family family = GetComponentFamliy<C>();
			//Returns whether the bit at family pos is set (i.e., whether it is one).
			GE_ASSERT(!m_ComponentMask.test(family), "Component already existed!")


				m_ComponentMask.set(family);

			//所有的component都用Create实例化-->方便子类继承！
			//C* component = C::Create(m_EntityManager, m_Id, std::forward<Args>(args)...);
			C* component = DBG_NEW C(m_EntityManager, m_Id, std::forward<Args>(args)...);
			std::shared_ptr<C> componentSharePtr;
			componentSharePtr.reset(component);
			m_Components.insert(std::make_pair(C::Famliy(), componentSharePtr));
			return component;

		}

		template<typename C>
		C* GetComponent() const
		{
			//GE_ASSERT(Valid(), "invalid entity" + m_Id);
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

	private:
		std::unordered_map<BaseComponent::Family, std::shared_ptr<BaseComponent>> m_Components;
		ComponentMask m_ComponentMask;

	};


}

