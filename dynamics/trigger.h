// Copyright (C) 2015-2016 Wei@OHK, Hiroshima University.
// This file is part of the "bulwark framework".
// For conditions of distribution and use, see copyright notice in bulwark.h

#ifndef _BUL_DYNAMICS_TRIGGER_H
#define _BUL_DYNAMICS_TRIGGER_H

#include "node.h"

namespace bul {
namespace dynamics {
/// A trigger performs some actions in each step.
class Trigger : public Node {
public:
	/// Configuration for a trigger.
	struct Configuration : public Node::Configuration {
		Configuration() : Node::Configuration(Node_Type::Trigger) { }
		virtual ~Configuration() { }
	};

	Trigger(Configuration* __conf) : Node(__conf) { }
	virtual ~Trigger() { }

protected:
	/// Called in each time step.
	virtual void Act() = 0;

private:
	friend class manager::SceneMgr;
};

} /* namespace dynamics */
} /* namespace bul */

#endif /* _BUL_DYNAMICS_TRIGGER_H */
