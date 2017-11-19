// Copyright (C) 2015-2016 Wei@OHK, Hiroshima University.
// This file is part of the "bulwark framework".
// For conditions of distribution and use, see copyright notice in bulwark.h

#ifndef _BUL_DYNAMICS_ACTOR_H
#define _BUL_DYNAMICS_ACTOR_H

#include <type_traits>

#include <map>

#include "node.h"
#include "../common/datapool.h"
#include "../common/container.h"

namespace bul {
namespace dynamics {
/// Base class for those who can be active or inactive.
class _Actable {
public:
	struct _Configuration {
		_Configuration() { }
		virtual ~_Configuration() { }

		bool Active = true;
	};

	_Actable(_Configuration* __conf) : _M_active(__conf->Active) { }
	virtual ~_Actable() { }

	/// Getter and Setter.
	bool IsActive() const {
		return _M_active;
	}

	void SetActive(bool __active) {
		_M_active = __active;
	}

private:
	bool _M_active;
};

/// An actor is composed of some components and can perform some actions.
class Actor : public Node, public _Actable {
public:
	/// Forward-declaration.
	class Component;

	/// Defines some types.
	typedef common::DataPool<int, float, void*, false> datapool_type;
	typedef common::Container<Component*, common::Key<std::size_t>,
				common::Tag<std::size_t, unsigned int>, std::map> storage_type;

	/// Configuration for an actor.
	struct Configuration : public Node::Configuration, public _Actable::_Configuration {
		Configuration() : Node::Configuration(Node_Type::Actor) { }
		virtual ~Configuration() { }

		std::size_t DataPoolSize = 128;
	};

	/// An actor is composed of some components.
	class Component : public Node, public _Actable {
	public:
		///
		struct Configuration : public Node::Configuration, public _Actable::_Configuration {
			Configuration() : Node::Configuration(Node_Type::Actor_Component) { }
			virtual ~Configuration() { }

			std::size_t Priority = 0;
			Actor* Parent = nullptr;
		};

		Component(Configuration* __conf) : Node(__conf), _Actable(__conf),
				_M_priority(__conf->Priority), _M_actor(__conf->Parent) { }
		virtual ~Component() { }

		/// Getters.
		std::size_t GetPriority() const {
			return _M_priority;
		}

		Actor* GetActor() {
			return _M_actor;
		}

		const Actor* GetActor() const {
			return _M_actor;
		}

	protected:
		/// Called in each time step only if the component is active
		virtual void Act() = 0;

		/// Called in each time step.
		virtual void Act_Anyway() = 0;

		/// Get and set shared data.
		template<typename _Tp>
		_Tp const& GetSharedData(std::size_t __index) const {
			return _M_actor->GetDataPool().Get<_Tp>(__index);
		}

		template<typename _Tp>
		void SetSharedData(std::size_t __index, _Tp const& __value) {
			_M_actor->GetDataPool().Set<_Tp>(__index, __value);
		}

	private:
		friend class Actor;

		std::size_t const _M_priority;

		Actor* const _M_actor;
	}; /* End of class Component. */

	Actor(Configuration* __conf) : Node(__conf), _Actable(__conf) {
		_M_datapool.Resize(__conf->DataPoolSize);
	}
	virtual ~Actor() {
		auto iter_end = _M_component.EndByKey<0>();
		for(auto iter = _M_component.BeginByKey<0>(); iter != iter_end;) {
			auto tmp = iter;
			iter++;
			auto component = (*tmp).second;
			RemoveComponent(component);
		}
	}

	/// Add a component.
	template<typename _Tp>
	_Tp* AddComponent(typename _Tp::Configuration* __conf) {
		static_assert(std::is_base_of<Component, _Tp>::value,
				"bul::dynamics::Actor::AddComponent(...): Type '_Tp' must be a derived type of bul::dynamics::Actor::Component.");

		__conf -> SceneManager = GetSceneMgr();
		__conf -> Parent = this;

		_Tp* component = new _Tp(__conf);
		_M_component.Insert(component, __conf->Id, __conf->Priority, __conf->Tag);

		return component;
	}

	/// Remove a component.
	void RemoveComponent(Component* __component) {
		_M_component.EraseByValue(__component);
		delete __component;
	}

	/// Get component(s) by id / priority / tag.
	typename storage_type::value_type GetComponentById(std::size_t __id) {
		return _M_component.GetByKey<0>(__id);
	}

	typename storage_type::value_list_type const& GetComponentsByPriority(std::size_t __priority) {
		return _M_component.GetByTag<0>(__priority);
	}

	typename storage_type::value_list_type const& GetComponentsByTag(unsigned int __tag) {
		return _M_component.GetByTag<1>(__tag);
	}

	/// Count component(s) by id / priority / tag.
	std::size_t CountComponentById(std::size_t __id) const {
		return _M_component.CountKey<0>(__id);
	}

	std::size_t CountComponentsByPriority(std::size_t __priority) const {
		return _M_component.CountTag<0>(__priority);
	}

	std::size_t CountComponentsByTag(unsigned int __tag) const {
		return _M_component.CountTag<1>(__tag);
	}

	/// Get reference to the shared data pool.
	datapool_type & GetDataPool() {
		return _M_datapool;
	}

	datapool_type const& GetDataPool() const {
		return _M_datapool;
	}

protected:
	/// Actions before components act.
	virtual void PreAct() = 0;

	/// Actions after components act.
	virtual void PostAct() = 0;

	/// Call components if they are active.
	void _M_Act() {
		for(auto iter = _M_component.BeginByTag<0>(); iter != _M_component.EndByTag<0>(); iter++) {
			auto& component_list = (*iter).second;
			for(auto iter_c = component_list.begin(); iter_c != component_list.end(); iter_c++) {
				auto component = *iter_c;
				if(component->IsActive()) {
					component -> Act();
				}
			}
		}
	}

	/// Call components anyway.
	void _M_Act_Anyway() {
		for(auto iter = _M_component.BeginByKey<0>(); iter != _M_component.EndByKey<0>(); iter++) {
			auto component = (*iter).second;
			component -> Act_Anyway();
		}
	}

private:
	friend class manager::SceneMgr;

	datapool_type _M_datapool;
	storage_type _M_component;
};

} /* namespace dynamics */
} /* namespace bul */

#endif /* _BUL_DYNAMICS_ACTOR_H */
