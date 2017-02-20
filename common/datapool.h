// Copyright (C) 2015-2016 Wei@OHK, Hiroshima University.
// This file is part of the "bulwark framework".
// For conditions of distribution and use, see copyright notice in bulwark.h

#ifndef _BUL_COMMON_DATAPOOL_H
#define _BUL_COMMON_DATAPOOL_H

#include <type_traits>
#include <stdexcept>

#include <vector>

namespace bul {
namespace common {
/// Base of DataPool, uses union / struct to store data.
template<typename _T1, typename _T2, typename _T3, bool _U>
struct _DataPool_Base;

template<typename _T1, typename _T2, typename _T3>
struct _DataPool_Base<_T1, _T2, _T3, true> {
	union _Data_Node {
		_T1 v_1;
		_T2 v_2;
		_T3 v_3;
	};
	typedef _Data_Node _node_type;
};

template<typename _T1, typename _T2, typename _T3>
struct _DataPool_Base<_T1, _T2, _T3, false> {
	struct _Data_Node {
		_T1 v_1;
		_T2 v_2;
		_T3 v_3;
	};
	typedef _Data_Node _node_type;
};

/// A data pool which offers fixed time access to elements in any order.
template<typename _T1, typename _T2, typename _T3, bool _U>
class DataPool final : protected _DataPool_Base<_T1, _T2, _T3, _U> {
	typedef typename _DataPool_Base<_T1, _T2, _T3, _U>::_node_type node_type;

	typedef _T1 data_type_1;
	typedef _T2 data_type_2;
	typedef _T3 data_type_3;

public:
	DataPool() { }
	~DataPool() { }

	/// Provides read-only access to the data contained in the pool.
	template<typename _Tp>
	_Tp const& Get(std::size_t __index) const {
		static_assert(std::is_same<_Tp, data_type_1>::value ||
				std::is_same<_Tp, data_type_2>::value ||
				std::is_same<_Tp, data_type_3>::value, "undefined data type for DataPool::Get(...).");
		if(__index >= Size()) {
			throw std::out_of_range("bul::common::DataPool<...>::Get(...)");
		}
		return Get_Helper(__index, static_cast<_Tp*>(nullptr));
	}

	/// Provides write access to the data contained in the pool.
	template<typename _Tp>
	void Set(std::size_t __index, _Tp const& __value) {
		static_assert(std::is_same<_Tp, data_type_1>::value ||
				std::is_same<_Tp, data_type_2>::value ||
				std::is_same<_Tp, data_type_3>::value, "undefined data type for DataPool::Set(...).");
		if(__index >= Size()) {
			throw std::out_of_range("bul::common::DataPool<...>::Set(...)");
		}
		Set_Helper(__index, __value);
	}

	/// Returns the number of elements in the pool.
	std::size_t Size() const {
		return _M_pool.size();
	}

	/// Resizes the pool to the specified number of elements.
	void Resize(std::size_t __size) {
		if (__size > _M_pool.max_size()) {
			throw std::length_error("bul::common::DataPool<...>::Resize(...)");
		}
		_M_pool.resize(__size);
	}

	/// Returns the total number of elements that the pool can hold before needing to allocate more memory.
	std::size_t Capacity() const {
		return _M_pool.capacity();
	}

	/// Attempt to preallocate enough memory for specified number of elements.
	void Reserve(std::size_t __size) {
		if (__size > _M_pool.max_size()) {
			throw std::length_error("bul::common::DataPool<...>::Reserve(...)");
		}
		_M_pool.reserve(__size);
	}

protected:
	/// Overloaded helper functions for DataPool::Get(...).
	data_type_1 const& Get_Helper(std::size_t __index,
			const data_type_1* const __func_specifier) const {
		return _M_pool[__index].v_1;
	}
	data_type_2 const& Get_Helper(std::size_t __index,
			const data_type_2* const __func_specifier) const {
		return _M_pool[__index].v_2;
	}
	data_type_3 const& Get_Helper(std::size_t __index,
			const data_type_3* const __func_specifier) const {
		return _M_pool[__index].v_3;
	}

	/// Overloaded helper functions for DataPool::Set(...).
	void Set_Helper(std::size_t __index, data_type_1 const& __value) {
		_M_pool[__index].v_1 = __value;
	}
	void Set_Helper(std::size_t __index, data_type_2 const& __value) {
		_M_pool[__index].v_2 = __value;
	}
	void Set_Helper(std::size_t __index, data_type_3 const& __value) {
		_M_pool[__index].v_3 = __value;
	}

private:
	std::vector<node_type> _M_pool;
};

} /* namespace common */
} /* namespace bul */

#endif /* _BUL_COMMON_DATAPOOL_H */
