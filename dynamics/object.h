// Copyright (C) 2015-2016 Wei@OHK, Hiroshima University.
// This file is part of the "bulwark framework".
// For conditions of distribution and use, see copyright notice in bulwark.h

#ifndef _BUL_DYNAMICS_OBJECT_H
#define _BUL_DYNAMICS_OBJECT_H

#include "node.h"

namespace bul {
namespace dynamics {
/// An object does not perform any action during the simulation.
class Object : public Node {
public:
	/// Configuration for an object.
	struct Configuration : public Node::Configuration {
		Configuration() : Node::Configuration(Node_Type::Object) { }
		virtual ~Configuration() { }
	};

	Object(Configuration* __conf) : Node(__conf) { }
	virtual ~Object() { }
};

} /* namespace dynamics */
} /* namespace bul */

#endif /* _BUL_DYNAMICS_OBJECT_H */
