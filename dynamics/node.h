// Copyright (C) 2015-2016 Wei@OHK, Hiroshima University.
// This file is part of the "bulwark framework".
// For conditions of distribution and use, see copyright notice in bulwark.h

#ifndef _BUL_DYNAMICS_NODE_H
#define _BUL_DYNAMICS_NODE_H

namespace bul {
namespace manager {
/// Forward-declaration.
class SceneMgr;

} /* namespace manager */

namespace dynamics {
/// Node types.
enum class Node_Type {
	Actor,
	Actor_Component,
	Object,
	Trigger
};

/// Base class for those elements used in a scene.
class Node {
public:
	/// Configuration for a node.
	struct Configuration {
		Configuration(Node_Type __node_type) : NodeType(__node_type) { }
		virtual ~Configuration() { }

		std::size_t Id = 0;
		unsigned int Tag = 0;
		
		unsigned int Flag = 0x0;
		void* UserData = nullptr;

		Node_Type const NodeType;
		manager::SceneMgr* SceneManager = nullptr;
	};

	Node(Configuration* __conf) : _M_type(__conf->NodeType), _M_id(__conf->Id),
			_M_tag(__conf->Tag), _M_flag(__conf->Flag), _M_user_data(__conf->UserData), 
					_M_scenemgr(__conf->SceneManager) { }
	virtual ~Node() { }

	/// Getters.
	Node_Type GetType() const {
		return _M_type;
	}

	std::size_t GetId() const {
		return _M_id;
	}

	unsigned int GetTag() const {
		return _M_tag;
	}

	unsigned int GetFlag() const {
		return _M_flag;
	}

	void SetFlag(unsigned int __flag) {
		_M_flag = __flag;
	}

	void AddFlag(unsigned int __flag) {
		_M_flag = _M_flag | __flag;
	}

	void RemoveFlag(unsigned int __flag) {
		_M_flag = _M_flag & (!__flag);
	}

	bool CheckFlag(unsigned int __flag) const {
		return _M_flag&__flag == __flag;
	}

	void* GetUserData() {
		return _M_user_data;
	}

	const void* GetUserData() const {
		return _M_user_data;
	}

	void SetUserData(void* __user_data) {
		_M_user_data = __user_data;
	}

	manager::SceneMgr* GetSceneMgr() {
		return _M_scenemgr;
	}

	const manager::SceneMgr* GetSceneMgr() const {
		return _M_scenemgr;
	}

private:
	Node_Type const _M_type;

	std::size_t const _M_id;
	unsigned int const _M_tag;

	unsigned int _M_flag;
	void* _M_user_data;

	manager::SceneMgr* const _M_scenemgr;
};

} /* namespace dynamics */
} /* namespace bul */

namespace std {
/// Specialization for std::hash<Node_Type>.
template<>
struct hash<bul::dynamics::Node_Type> {
	size_t operator()(bul::dynamics::Node_Type __node) const noexcept {
		size_t ret = 0;
		switch(__node) {
		case bul::dynamics::Node_Type::Actor:
			ret = 0;
			break;
		case bul::dynamics::Node_Type::Object:
			ret = 1;
			break;
		case bul::dynamics::Node_Type::Trigger:
			ret = 2;
			break;
		default:
			ret = 3;
			break;
		}
		return ret;
	}
};

} /* namespace std */

#endif /* _BUL_DYNAMICS_NODE_H */
