#ifndef __SKIP_LIST_HPP__
#define __SKIP_LIST_HPP__

#include <time.h>
#include <stdlib.h>
#include <functional>

namespace std
{
	struct _skiplist_random
	{
	public:
		_skiplist_random()
		{
			srand(time(NULL));
		}
		long operator()()
		{
			return rand();//Next();
		}
	};

	struct _skiplist_true{};
	struct _skiplist_false{};
	template<int istrue>
	struct _skiplist_istrue;
	template<>
	struct _skiplist_istrue<1>
	{
		typedef _skiplist_true type;
	};
	template<>
	struct _skiplist_istrue<0>
	{
		typedef _skiplist_false type;
	};
	
	template<typename _Tkey, typename _Tval, typename _Tcpr, typename _Trand, int _Lvl_radio, int _Max_level, int _Mult>
	struct _skiplist_traits
	{
		typedef _Tkey key_type;
		typedef _Tval val_type;
		typedef std::pair<key_type,val_type> value_type;
		typedef _Tcpr cpr_type;
		typedef _Trand rand_type;
		enum
		{
			lvl_radio = _Lvl_radio,
			max_level = _Max_level,
			is_multi = _Mult,
		};
	};
	
	template<typename _Ttraits> struct _skiplist_node;
	
	template<typename _Ttraits>
	struct _skiplist_level
	{
		_skiplist_node<_Ttraits>* forward;
		_skiplist_node<_Ttraits>* backward;
	};

	template<typename _Ttraits>
	struct _skiplist_node
	{
		typename _Ttraits::value_type kv;
		int lsize;
		_skiplist_level<_Ttraits> levels[1];
	};

	template<typename _Ttraits>
	class _skiplist_val;

	template<typename _Ttraits>
	class _skiplist_const_iterator
	{
	public:
		friend class _skiplist_val<_Ttraits>;
		typedef _skiplist_const_iterator<_Ttraits> _MyType;
		typedef typename _Ttraits::value_type value_type;
		typedef _skiplist_node<_Ttraits> node_type;
	public:
		_skiplist_const_iterator()
			:pnode_(NULL)
		{
		}

		_skiplist_const_iterator(node_type *pnode)
			:pnode_(pnode)
		{
		}

		_skiplist_const_iterator(const _MyType& other)
			:pnode_(other.pnode_)
		{
		}

		_MyType& operator=(const _MyType& other)
		{
			pnode_ = other.pnode_;
			return *this;
		}

		const value_type& operator* () const
		{
			return pnode_->kv;
		}
		const value_type* operator-> () const
		{
			return &pnode_->kv;
		}
		_MyType& operator++()
		{
			pnode_ = pnode_->levels[0].forward;
			return *this;
		}
		_MyType operator++(int)
		{
			_MyType _Tmp = *this;
			++*this;
			return (_Tmp);
		}
		_MyType& operator--()
		{
			pnode_ = pnode_->levels[0].backward;
			return *this;
		}
		_MyType operator--(int)
		{
			_MyType _Tmp = *this;
			--*this;
			return (_Tmp);
		}

		bool operator==(const _MyType& other)
		{
			return pnode_ == other.pnode_;
		}
		bool operator!=(const _MyType& other)
		{
			return pnode_ != other.pnode_;
		}
	protected:
		node_type *pnode_;
	};

	template<typename _Ttraits>
	class _skiplist_iterator
		: public _skiplist_const_iterator<_Ttraits>
	{
	public:
		friend class _skiplist_val<_Ttraits>;
		typedef _skiplist_iterator<_Ttraits> _MyType;
		typedef typename _Ttraits::value_type value_type;
		typedef _skiplist_node<_Ttraits> node_type;
	public:
		_skiplist_iterator()
			:_skiplist_const_iterator<_Ttraits>(NULL)
		{
		}

		_skiplist_iterator(node_type *pnode)
			:_skiplist_const_iterator<_Ttraits>(pnode)
		{
		}

		_skiplist_iterator(const _MyType& other)
			:_skiplist_const_iterator<_Ttraits>(other.pnode_)
		{
		}

		_MyType& operator=(const _MyType& other)
		{
			pnode_ = other.pnode_;
			return *this;
		}

		value_type& operator* () const
		{
			return pnode_->kv;
		}
		value_type* operator-> () const
		{
			return &pnode_->kv;
		}
		_MyType& operator++()
		{
			pnode_ = pnode_->levels[0].forward;
			return *this;
		}
		_MyType operator++(int)
		{
			_MyType _Tmp = *this;
			++*this;
			return (_Tmp);
		}
		_MyType& operator--()
		{
			pnode_ = pnode_->levels[0].backward;
			return *this;
		}
		_MyType operator--(int)
		{
			_MyType _Tmp = *this;
			--*this;
			return (_Tmp);
		}
		bool operator==(const _MyType& other)
		{
			return pnode_ == other.pnode_;
		}
		bool operator!=(const _MyType& other)
		{
			return pnode_ != other.pnode_;
		}
	};

	template<typename _Ttraits>
	class _skiplist_val
	{
	public:
		typedef _skiplist_iterator<_Ttraits> iterator;
		typedef _skiplist_const_iterator<_Ttraits> const_iterator;
	
		typedef _skiplist_val _MyType;
		typedef size_t size_type;
		typedef typename _Ttraits::value_type value_type;
		typedef _skiplist_node<_Ttraits> node_type;
		typedef _skiplist_level<_Ttraits> level_type;
		typedef typename _Ttraits::key_type key_type;
		typedef typename _Ttraits::val_type val_type;
		typedef typename _Ttraits::cpr_type key_compare;
		typedef typename _Ttraits::rand_type rand_type;
	
		typedef std::pair<iterator, bool> _Pairib;
		typedef std::pair<iterator, iterator> _Pairii;
		typedef std::pair<const_iterator, const_iterator> _Paircc;
	public:
		_skiplist_val()
		{
			srand(time(NULL));
			head_ = _alloc_node(_Ttraits::max_level);
			for(int i = 0; i < _Ttraits::max_level; ++i)
			{
				_level_forward(head_,i) = head_;
				_level_backward(head_,i) = head_;
			}
			levelsize_ = 0;
			length_ = 0;
		}
		_skiplist_val(const _MyType& other)
		{
			srand(time(NULL));
			head_ = _alloc_node(_Ttraits::max_level);
			for(int i = 0; i < _Ttraits::max_level; ++i)
			{
				_level_forward(head_,i) = head_;
				_level_backward(head_,i) = head_;
			}
			levelsize_ = 0;
			length_ = 0;
		}
		~_skiplist_val()
		{
			_dealloc_node(head_);
		}
	public:
		iterator begin()
		{
			return iterator(_level_forward(head_,0));
		}
		const_iterator begin() const
		{
			return const_iterator(_level_forward(head_,0));
		}
		iterator end()
		{
			return iterator(head_);
		}
		const_iterator end() const
		{
			return const_iterator(head_);
		}
		iterator rbegin()
		{
			return iterator(_level_backward(head_,0));
		}
		const_iterator rbegin() const
		{
			return const_iterator(_level_backward(head_,0));
		}
		iterator rend()
		{
			return iterator(head_);
		}
		const_iterator rend() const
		{
			return const_iterator(head_);
		}
		_Pairib insert(const value_type& kv)
		{
			return _insert_ismulti(kv,_skiplist_istrue<_Ttraits::is_multi>::type());
		}
		template<class _Iter>
		void insert(_Iter _First, _Iter _Last)
		{
			for (; _First != _Last; ++_First)
			{
				insert(*_First);
			}
		}
		iterator erase(const_iterator _Where)
		{
			return _erase(_Where.pnode_);
		}
		iterator erase(const_iterator b, const_iterator e)
		{
			while(b != e)
				erase(b++);
			return iterator(b.pnode_);
		}
		size_type erase(const key_type& k)
		{
			size_type ret = 0;
			_Paircc be = equal_range(k);
			while(be.first != be.second)
			{
				erase(be.first++);
				ret++;
			}
			return ret;
		}
		_Pairii equal_range(const key_type& k)
		{
			_Pairii ret;
			ret.first = _find(k);
			for(ret.second = ret.first; ret.second != end(); ++ret.second)
			{
				if(kcpr_(ret.first->first,ret.second->first))
					break;
			}
			return ret;
		}
		_Paircc equal_range(const key_type& k) const
		{
			_Paircc ret;
			ret.first = _find(k);
			for(ret.second = ret.first; ret.second != end(); ++ret.second)
			{
				if(kcpr_(ret.first->first,ret.second->first))
					break;
			}
			return ret;
		}
		size_type count(const key_type& k) const
		{
			_Paircc pr;
			size_type ret = 0;
			pr.first = _find(k);
			for(pr.second = pr.first; pr.second != end(); ++pr.second)
			{
				if(kcpr_(pr.first->first,pr.second->first))
					break;
				ret++;
			}
			return ret;
		}
		iterator find(const key_type& k)
		{
			return _find(k);
		}
		const_iterator find(const key_type& k) const
		{
			return _find(k);
		}
	private:
		static key_type& _key(node_type* pnode)
		{
			return pnode->kv.first;
		}
		static val_type& _val(node_type* pnode)
		{
			return pnode->kv.second;
		}
		static node_type*& _level_forward(node_type* pnode, size_type level)
		{
			return pnode->levels[level].forward;
		}
		static node_type*& _level_backward(node_type* pnode, size_type level)
		{
			return pnode->levels[level].backward;
		}
	private:
		size_type _random_level()
		{
			long i;
			size_type h = 1;
			for(;;)
			{
				i = rand_();
				if(!(i%_Ttraits::lvl_radio))
					h++;
				else
					return h > _Ttraits::max_level ? _Ttraits::max_level : h;
			}
		}
		node_type* _alloc_node(int lvl)
		{
			node_type* ret = (node_type*)malloc(sizeof(node_type)+sizeof(level_type)*(lvl-1));
			ret->lsize = lvl;
			return ret;
		}
		void _dealloc_node(node_type* pnode)
		{
			free(pnode);
		}
		void _create_node(node_type* pnode, const value_type& kv)
		{
			new (&pnode->kv) value_type(kv);
		}
		void _destroy_node(node_type* pnode)
		{
			pnode->kv.~value_type();
		}
		iterator _find(const key_type& k)
		{
			node_type* x = head_;
			for(long i = levelsize_ - 1; i >= 0; --i)
				for(;;)
				{
					if(_level_forward(x,i) == head_)
						break;
					else if(!kcpr_(_key(_level_forward(x,i)) , k))
					{
						break;
					}
					x = _level_forward(x,i);
				}

			if(!kcpr_(k, _key(_level_forward(x,0))))
			{
				return iterator(_level_forward(x,0));
			}
			else
			{
				return iterator(head_);
			}
		}
		const_iterator _find(const key_type& k) const
		{
			node_type* x = head_;
			for(long i = levelsize_ - 1; i >= 0; --i)
				for(;;)
				{
					if(_level_forward(x,i) == head_)
						break;
					else if(!kcpr_(_key(_level_forward(x,i)) , k))
					{
						break;
					}
					x = _level_forward(x,i);
				}
			if(!kcpr_(k, _key(_level_forward(x,0))))
			{
				return const_iterator(_level_forward(x,0));
			}
			else
			{
				return const_iterator(head_);
			}
		}
		iterator _insert(const value_type& kv, node_type** pos, size_type lvl)
		{
			size_type i;
			node_type* x = _alloc_node(lvl);
			_create_node(x,kv);

			if (lvl > levelsize_) {
				for (i = levelsize_; i < lvl; i++) {
					pos[i] = head_;
				}
				levelsize_ = lvl;
			}

			for (i = 0; i < lvl; i++) {
				_level_forward(x,i) = _level_forward(pos[i],i);
				_level_forward(pos[i],i) = x;
				_level_backward(x,i) = pos[i];
				_level_backward(_level_forward(x,i),i) = x;
			}

			length_++;
			return iterator(x);
		}
		_Pairib _insert_ismulti(const value_type& kv, _skiplist_true)
		{
			node_type* update[_Ttraits::max_level], *x = head_;
			bool isfind = false;
			int i;
			for(i = levelsize_ - 1; i >= 0; --i)
			{
				if(!isfind) for(;;)
				{
					if(_level_forward(x,i) == head_)
						break;
					else if(!kcpr_(_key(_level_forward(x,i)) , kv.first))
					{
						if(!kcpr_(kv.first, _key(_level_forward(x,i))))
						{
							x = _level_forward(x,i);
							isfind = true;
						}
						break;
					}
					x = _level_forward(x,i);
				}
				update[i] = x;
			}

			return _Pairib(_insert(kv,update,isfind ? 1 : _random_level()),true);
		}
		_Pairib _insert_ismulti(const value_type& kv, _skiplist_false)
		{
			node_type* update[_Ttraits::max_level], *x = head_;
			int i;
			for(i = levelsize_ - 1; i >= 0; --i)
			{
				for(;;)
				{
					if(_level_forward(x,i) == head_)
						break;
					else if(!kcpr_(_key(_level_forward(x,i)) , kv.first))
					{
// 						if(!kcpr_(kv.first, _key(_level_forward(x,i))))
// 						{
// 							return _Pairib(iterator(_level_forward(x,i)),false);
// 						}
						break;
					}
					x = _level_forward(x,i);
				}
				update[i] = x;
			}

			if(levelsize_&&_level_forward(update[0],0) != head_&&!kcpr_(kv.first, _key(_level_forward(update[0],0))))
			{
				return _Pairib(iterator(_level_forward(update[0],0)),false);
			}
			else
			{
				return _Pairib(_insert(kv,update,_random_level()),true);
			}
		}
		node_type* _erase(node_type* pnode)
		{
			node_type* ret = _level_forward(pnode,0);
			for(size_type i = 0; i < pnode->lsize; ++i)
			{
				_level_forward(_level_backward(pnode,i),i) = _level_forward(pnode,i);
				_level_backward(_level_forward(pnode,i),i) = _level_backward(pnode,i);
			}
			_destroy_node(pnode);
			_dealloc_node(pnode);
			length_--;
			return ret;
		}
	private:
		node_type* head_;
		size_type levelsize_;
		size_type length_;
		key_compare kcpr_;
		rand_type rand_;
	};

	template<typename _Tkey
		, typename _Tval
		, typename _Tcpr = std::less<_Tkey>
		, typename _Trand = _skiplist_random
		, int _Max_level = 32>
	class skiplist
		: public _skiplist_val<_skiplist_traits<_Tkey, _Tval, _Tcpr
		, _Trand, 3, _Max_level, 0 > >
	{
	public:

	};

}

#endif//__SKIP_LIST_HPP__