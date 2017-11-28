// Copyright (C) 2015-2016 Wei@OHK, Hiroshima University.
// This file is part of the "bulwark framework".
// For conditions of distribution and use, see copyright notice in bulwark.h

#ifndef _BUL_MANAGER_SCENEMGR_H
#define _BUL_MANAGER_SCENEMGR_H

#include <type_traits>
#include <stdexcept>

#include <unordered_map>
#include <set>

#include "../common/container.h"
#include "../dynamics/actor.h"
#include "../dynamics/object.h"
#include "../dynamics/trigger.h"
#include "monitor.h"

namespace bul {
namespace manager {
/// A SceneMgr managers all objects, actors and triggers.
class SceneMgr {
public:
	/// Defines some types.
	typedef common::Container<dynamics::Node*, common::Key<std::size_t>,
				common::Tag<dynamics::Node_Type, unsigned int>, std::unordered_map> storage_type;

	/// Configuration for a scene manager.
	struct Configuration {
		std::size_t MaxStep = 7200;
	};

	SceneMgr(Configuration* __conf) : _M_max_step(__conf->MaxStep) {
		_M_current_step = 0;
		_M_terminated = false;
	}
	virtual ~SceneMgr() {
		auto iter_end = _M_node.EndByKey<0>();
		for(auto iter = _M_node.BeginByKey<0>(); iter != iter_end;) {
			auto tmp = iter;
			iter++;
			auto node = (*tmp).second;
			RemoveNode(node);
		}
	}

	/// Accept monitors and run the simulation.
	template<typename... _Tpls>
	void Run(_Tpls*... __tpls) {
		if(IsTerminated()) {
			throw std::logic_error("bul::manager::SceneMgr::Run(...) : the simulation has been already performed.");
		}
		_M_Run<0, _Tpls...>(__tpls...);
	}

	/// Add a node.
	template<typename _Tp>
	_Tp* AddNode(typename _Tp::Configuration* __conf) {
		static_assert(std::is_base_of<dynamics::Object, _Tp>::value ||
					  std::is_base_of<dynamics::Actor, _Tp>::value ||
					  std::is_base_of<dynamics::Trigger, _Tp>::value,
				"bul::manager::SceneMgr::AddObject(...): Illegal node type.");

		__conf -> SceneManager = this;

		_Tp* node = new _Tp(__conf);
		_M_node.Insert(node, __conf->Id, __conf->NodeType, __conf->Tag);

		return node;
	}

	/// Remove a node.
	void RemoveNode(dynamics::Node* __node) {
		_M_node.EraseByValue(__node);
		delete __node;
	}

	/// Terminate the simulation (after the current step is completely done).
	void Terminate() {
		_M_terminated = true;
	}

	/// Get node(s) by id / type / tag.
	typename storage_type::value_type GetNodeById(std::size_t __id) {
		return _M_node.GetByKey<0>(__id);
	}

	typename storage_type::value_list_type const& GetNodesByType(dynamics::Node_Type __type) {
		return _M_node.GetByTag<0>(__type);
	}

	typename storage_type::value_list_type const& GetNodesByTag(unsigned int __tag) {
		return _M_node.GetByTag<1>(__tag);
	}

	/// Count node(s) by id / type / tag / pointer.
	std::size_t CountNodeById(std::size_t __id) const {
		return _M_node.CountKey<0>(__id);
	}

	std::size_t CountNodesByType(dynamics::Node_Type __type) const {
		return _M_node.CountTag<0>(__type);
	}

	std::size_t CountNodesByTag(unsigned int __tag) const {
		return _M_node.CountTag<1>(__tag);
	}

	std::size_t CountNodeByPointer(dynamics::Node* __node) const {
		return _M_node.CountValue(__node);
	}

	/// Get max / current step.
	std::size_t GetMaxStep() const {
		return _M_max_step;
	}

	std::size_t GetCurrentStep() const {
		return _M_current_step;
	}

	/// Is the simulation done?
	bool IsTerminated() const {
		return _M_terminated || _M_current_step >= _M_max_step;
	}

protected:
	/// Actions before actors and triggers act.
	virtual void PreStep() = 0;

	/// Actions after actors and triggers act.
	virtual void PostStep() = 0;

	/// Call actors and triggers.
	void _M_Step() {
		if(_M_node.CountTag<0>(dynamics::Node_Type::Actor) > 0) {
			auto& actor_list = _M_node.GetByTag<0>(dynamics::Node_Type::Actor);
			for(auto iter = actor_list.begin(); iter !=  actor_list.end(); iter++) {
				auto actor = static_cast<dynamics::Actor*>(*iter);
				if(actor -> IsActive()) {
					actor -> PreAct();
					actor -> _M_Act();
					actor -> PostAct();
				}
				actor -> _M_Act_Anyway();
			}
		}

		if(_M_node.CountTag<0>(dynamics::Node_Type::Trigger) > 0) {
			auto& trigger_list = _M_node.GetByTag<0>(dynamics::Node_Type::Trigger);
			for(auto iter = trigger_list.begin(); iter != trigger_list.end(); iter++) {
				auto trigger = static_cast<dynamics::Trigger*>(*iter);
				trigger -> Act();
			}
		}
	}

	/// Run the simulation.
	template<std::size_t _PH>
	void _M_Run() {
		for(auto monitor : _M_monitor) {
			monitor ->_M_scenemgr = this;
			monitor -> Initialize();
		}

		while(!IsTerminated()) {
			PreStep();
			_M_Step();
			PostStep();
			for(auto monitor : _M_monitor) {
				monitor -> Step();
			}
			_M_current_step++;
		}

		for(auto monitor : _M_monitor) {
			monitor -> Finalize();
		}
	}

	/// Add monitors.
	template<std::size_t _PH, typename _Head, typename... _Tail>
	void _M_Run(_Head* __head, _Tail*... __tail) {
		static_assert(std::is_base_of<Monitor, _Head>::value,
				"SceneMgr::Run() : Only accept monitor arguments");
		_M_monitor.insert(__head);

		_M_Run<_PH, _Tail...>(__tail...);
	}

private:
	std::size_t const _M_max_step;
	std::size_t _M_current_step;

	bool _M_terminated;

	storage_type _M_node;

	std::set<Monitor*> _M_monitor;
};

} /* namespace manager */
} /* namespace bul */

#endif /* _BUL_MANAGER_SCENEMGR_H */
