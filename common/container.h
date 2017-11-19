// Copyright (C) 2015-2016 Wei@OHK, Hiroshima University.
// This file is part of the "bulwark framework".
// For conditions of distribution and use, see copyright notice in bulwark.h

#ifndef _BUL_COMMON_CONTAINER_H
#define _BUL_COMMON_CONTAINER_H

#include <type_traits>
#include <stdexcept>

#include <list>
#include <tuple>

namespace bul {
namespace common {
/// A set of keys.
template<typename... _Keys>
struct Key;

/// A set of tags.
template<typename... _Tags>
struct Tag;

/// Base of Container, defines basic types.
template<typename _Tp, typename _KeySet, typename _TagSet, template<typename...> class _Map_Container>
struct _Container_Base;

template<typename _Tp, typename... _Keys, typename... _Tags, template<typename...> class _Map_Container>
struct _Container_Base<_Tp*, Key<_Keys...>, Tag<_Tags...>, _Map_Container> {
	typedef _Tp* _value_type;
	typedef std::list<_value_type> _value_list_type;

	typedef std::tuple<_Map_Container<_Keys, _value_type>...> _key_type;
	typedef std::tuple<_Map_Container<_Tags, _value_list_type>...> _tag_type;

	typedef std::tuple<typename _Map_Container<_Keys, _value_type>::iterator...> _key_locator_type;
	typedef std::tuple<typename _Map_Container<_Tags, _value_list_type>::iterator...> _tag_major_locator_type;
	typedef std::tuple<typename std::conditional<true, typename _value_list_type::iterator
			, _Tags>::type...> _tag_minor_locator_type;
	struct _locator {
		_key_locator_type _key;
		_tag_major_locator_type _tag_major;
		_tag_minor_locator_type _tag_minor;
	};
	typedef _Map_Container<_value_type, _locator> _locator_type;
};

/// Help erase elements by key.
template<std::size_t _Index, typename _Locator, typename _Storage>
struct _Erase_Key_Helper {
	void operator()(_Storage & __key_storage, _Locator const& __locator) {
		std::get<_Index - 1>(__key_storage).erase(std::get<_Index - 1>(__locator._key));

		_Erase_Key_Helper<_Index - 1, _Locator, _Storage> helper;
		helper(__key_storage, __locator);
	}
};

template<typename _Locator, typename _Storage>
struct _Erase_Key_Helper<0, _Locator, _Storage> {
	void operator()(_Storage & __key_storage, _Locator const& __locator) { }
};

/// Help erase elements by tag.
template<std::size_t _Index, typename _Locator, typename _Storage>
struct _Erase_Tag_Helper {
	void operator()(_Storage & __tag_storage, _Locator const& __locator) {
			(*std::get<_Index - 1>(__locator._tag_major)).second.erase(std::get<_Index - 1>(__locator._tag_minor));
			if((*std::get<_Index - 1>(__locator._tag_major)).second.size() == 0) {
				std::get<_Index - 1>(__tag_storage).erase(std::get<_Index - 1>(__locator._tag_major));
			}

			_Erase_Tag_Helper<_Index - 1, _Locator, _Storage> helper;
			helper(__tag_storage, __locator);
		}
};

template<typename _Locator, typename _Storage>
struct _Erase_Tag_Helper<0, _Locator, _Storage> {
	void operator()(_Storage & __tag_storage, _Locator const& __locator) { }
};

/// A container which offers quick read-only access to elements by either key or tag.
template<typename _Tp, typename _KeySet, typename _TagSet, template<typename...> class _Map_Container>
class Container;

template<typename _Tp, typename... _Keys, typename... _Tags, template<typename...> class _Map_Container>
class Container<_Tp*, Key<_Keys...>, Tag<_Tags...>, _Map_Container> final :
		protected _Container_Base<_Tp*, Key<_Keys...>, Tag<_Tags...>, _Map_Container> {
	typedef _Container_Base<_Tp*, Key<_Keys...>, Tag<_Tags...>, _Map_Container> _Base;

public:
	typedef typename _Base::_value_type value_type;
	typedef typename _Base::_value_list_type value_list_type;

private:
	typedef typename _Base::_key_type key_type;
	typedef typename _Base::_tag_type tag_type;
	typedef typename _Base::_locator locator;
	typedef typename _Base::_locator_type locator_type;

public:
	Container() { }
	~Container() { }

	/// Inserts an element.
	void Insert(value_type const& __value, _Keys const&... __keys, _Tags const&... __tags) {
		if(CountValue(__value)) {
			throw std::logic_error("bul::common::Container<...>::Insert(...) : Value already exists.");
		}
		locator& locator = _M_locator_storage[__value];

		_M_Insert_Keys<0, _Keys...>(locator, __value, __keys...);
		_M_Insert_Tags<0, _Tags...>(locator, __value, __tags...);
	}

	/// Erases elements by key or tag or value.
	template<std::size_t _Index>
	void EraseByKey(typename std::tuple_element<_Index, key_type>::type::key_type const& __key) {
		if(!CountKey<_Index>(__key)) {
			throw std::out_of_range("bul::common::Container<...>::EraseByKey(...) : Key does not exist.");
		}
		_M_EraseByValue(GetByKey<_Index>(__key));
	}

	template<std::size_t _Index>
	void EraseByTag(typename std::tuple_element<_Index, tag_type>::type::key_type const& __tag) {
		if(!CountTag<_Index>(__tag)) {
			throw std::out_of_range("bul::common::Container<...>::EraseByTag(...) : Tag does not exist.");
		}
		auto const& list = GetByTag<_Index>(__tag);
		auto iter_end = list.end();
		for(auto iter = list.begin(); iter != iter_end;) {
			auto tmp = iter;
			iter++;
			_M_EraseByValue(*tmp);
		}
	}

	void EraseByValue(value_type const& __value) {
		if(!CountValue(__value)) {
			throw std::out_of_range("bul::common::Container<...>::EraseByValue(...) : Value does not exist.");
		}
		_M_EraseByValue(__value);
	}

	/// Provides read-only access to elements by key or tag.
	template<std::size_t _Index>
	value_type GetByKey(typename std::tuple_element<_Index, key_type>::type::key_type const& __key) const {
		if(!CountKey<_Index>(__key)) {
			throw std::out_of_range("bul::common::Container<...>::GetByKey(...) : Key does not exist.");
		}
		return std::get<_Index>(_M_key_storage).at(__key);
	}

	template<std::size_t _Index>
	value_list_type const& GetByTag(typename std::tuple_element<_Index, tag_type>::type::key_type const& __tag) const {
		if(!CountTag<_Index>(__tag)) {
			throw std::out_of_range("bul::common::Container<...>::GetByTag(...) : Tag does not exist.");
		}
		return std::get<_Index>(_M_tag_storage).at(__tag);
	}

	/// Provides read-only traversal in key-order or tag-order
	template<std::size_t _Index>
	typename std::tuple_element<_Index, key_type>::type::const_iterator BeginByKey() const {
		return std::get<_Index>(_M_key_storage).begin();
	}

	template<std::size_t _Index>
	typename std::tuple_element<_Index, key_type>::type::const_iterator EndByKey() const {
		return std::get<_Index>(_M_key_storage).end();
	}

	template<std::size_t _Index>
	typename std::tuple_element<_Index, tag_type>::type::const_iterator BeginByTag() const {
		return std::get<_Index>(_M_tag_storage).begin();
	}

	template<std::size_t _Index>
	typename std::tuple_element<_Index, tag_type>::type::const_iterator EndByTag() const {
		return std::get<_Index>(_M_tag_storage).end();
	}

	/// Counts the number of elements by key or tag or value.
	template<std::size_t _Index>
	std::size_t CountKey(typename std::tuple_element<_Index, key_type>::type::key_type const& __key) const {
		return std::get<_Index>(_M_key_storage).count(__key);
	}

	template<std::size_t _Index>
	std::size_t CountTag(typename std::tuple_element<_Index, tag_type>::type::key_type const& __tag) const {
		if(std::get<_Index>(_M_tag_storage).count(__tag)) {
			return std::get<_Index>(_M_tag_storage).at(__tag).size();
		} else {
			return 0;
		}
	}

	std::size_t CountValue(value_type const& __value) const {
		return _M_locator_storage.count(__value);
	}

protected:
	/// Inersts the keys part of elements.
	template<std::size_t _Index>
	void _M_Insert_Keys(locator & __locator, value_type const& __value) { }

	template<std::size_t _Index, typename _Head, typename... _Tail>
	void _M_Insert_Keys(locator & __locator, value_type const& __value,
			_Head const& __head, _Tail const&... __tail) {
		if(std::get<_Index>(_M_key_storage).count(__head)) {
			throw std::out_of_range("bul::common::Container<...>::Insert(...) : Key already exists.");
		}
		auto ret = std::get<_Index>(_M_key_storage).insert(std::pair<_Head, value_type>(__head, __value));
		std::get<_Index>(__locator._key) = ret.first;

		_M_Insert_Keys<_Index + 1, _Tail...>(__locator, __value, __tail...);
	}

	/// Inserts the tags part of elements.
	template<std::size_t _Index>
	void _M_Insert_Tags(locator & __locator, value_type const& __value) { }

	template<std::size_t _Index, typename _Head, typename... _Tail>
	void _M_Insert_Tags(locator & __locator, value_type const& __value,
			_Head const& __head, _Tail const&... __tail) {
		std::get<_Index>(_M_tag_storage)[__head].push_back(__value);
		auto major_ret = std::get<_Index>(_M_tag_storage).find(__head);
		std::get<_Index>(__locator._tag_major) = major_ret;
		std::get<_Index>(__locator._tag_minor) = --std::get<_Index>(_M_tag_storage)[__head].end();

		_M_Insert_Tags<_Index + 1, _Tail...>(__locator, __value, __tail...);
	}

	/// Erases elements by value.
	void _M_EraseByValue(value_type const& __value) {
		_Erase_Key_Helper<sizeof...(_Keys), locator, key_type> erase_key_helper;
		erase_key_helper(_M_key_storage, _M_locator_storage[__value]);
		_Erase_Tag_Helper<sizeof...(_Tags), locator, tag_type> erase_tag_helper;
		erase_tag_helper(_M_tag_storage, _M_locator_storage[__value]);
		_M_locator_storage.erase(__value);
	}

private:
	key_type _M_key_storage;
	tag_type _M_tag_storage;

	locator_type _M_locator_storage;
};

} /* namespace common */
} /* namespace bul */

#endif /* _BUL_COMMON_CONTAINER_H */
