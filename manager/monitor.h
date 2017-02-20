// Copyright (C) 2015-2016 Wei@OHK, Hiroshima University.
// This file is part of the "bulwark framework".
// For conditions of distribution and use, see copyright notice in bulwark.h

#ifndef _BUL_MANAGER_MONITOR_H
#define _BUL_MANAGER_MONITOR_H

namespace bul {
namespace manager {
/// Forward-declaration.
class SceneMgr;

/// Monitor a candidate in a simulation.
class Monitor {
public:
	Monitor() {
		_M_scenemgr = nullptr;
	}
	virtual ~Monitor() { }

	const SceneMgr* GetSceneMgr() const {
		return _M_scenemgr;
	}

protected:
	/// Initialize the monitor before simulation starts.
	virtual void Initialize() = 0;

	/// Monitor the simulation in this step (after simulation step).
	virtual void Step() = 0;

	/// Finalize the Monitor.
	virtual void Finalize() = 0;

private:
	friend class SceneMgr;

	const SceneMgr* _M_scenemgr;
};

} /* namespace manager */
} /* namespace bul */

#endif /* _BUL_MANAGER_MONITOR_H */
