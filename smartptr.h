#ifndef _SMARTPTR_H
#define _SMARTPTR_H
// reference countable smart point
// circular member setting will not free memory
//
// eg) 
// CA has SmartPtr<CB> member and CB has SmartPtr<CA> member.
// In other word, each object has the other object's smart pointer.
// CA will be freed after CB is freed, and vise versa.
// Memory will not be freed forever.
#include <cassert>

template<class _Ty>
class SmartPtr {
	struct RefPtr {
		_Ty *_Ptr;
		int _Refcnt;
	};
public:
	typedef _Ty element_type;
	explicit SmartPtr(_Ty *_P = 0) throw ()
	{
		if(_P != 0)
		{
			_Ptr = new RefPtr;
			_Ptr->_Ptr = _P;
			_Ptr->_Refcnt = 0;

			IncRefcnt();
		}
		else
			_Ptr = 0;
	}

	SmartPtr(const SmartPtr<_Ty>& _Y) throw ()
		: _Ptr(_Y._Ptr) 
	{
		IncRefcnt();
	}

	SmartPtr<_Ty>& operator=(const SmartPtr<_Ty>& _Y) throw ()
	{
		if (this != &_Y)
		{
			if (_Ptr != _Y._Ptr)
			{
				DecRefcnt();

				_Ptr = _Y._Ptr;

				IncRefcnt();
			}
        }   
		return (*this); 
	}

	~SmartPtr()
	{
		DecRefcnt();
	}
	_Ty& operator*() const throw ()
		{return (*get()); }
	_Ty *operator->() const throw ()
		{return (get()); }
	_Ty *get() const throw ()
	{
		if(!_Ptr) return NULL;
		if(_Ptr->_Refcnt == 0) return NULL;
		return (_Ptr->_Ptr); 
	}
	_Ty *release() throw ()
	{
		_Ty *_tPtr = NULL;
		if(_Ptr)
		{
			_tPtr = _Ptr->_Ptr;
			_Ptr->_Refcnt = 0;
			_Ptr = NULL;
		}
		
		return (_tPtr); 
	}
	bool operator !()
	{
		if(_Ptr == 0 || _Ptr->_Refcnt == 0) return TRUE;
		if(_Ptr->_Ptr == NULL) return TRUE;
		return FALSE;
	}
	bool operator != (const void* p)
	{
		if(_Ptr != 0 && _Ptr->_Refcnt > 0)
		{
			if(_Ptr->_Ptr != p) return TRUE;
		}

		return FALSE;
	}
	operator void*() const
	{
		if(_Ptr == NULL) return NULL;
		if(_Ptr->_Refcnt == 0) return NULL;
		return _Ptr->_Ptr;
	}

//	template<class _OTy>
//	operator SmartPtr<_OTy>()
//	{
//		return SmartPtr<_OTy> (_Ptr);
//	}

private:
	void IncRefcnt()
	{
		if(_Ptr)
			++(_Ptr->_Refcnt);
	}

	void DecRefcnt()
	{
		if(_Ptr)
		{
			--(_Ptr->_Refcnt);
			if(_Ptr->_Refcnt <= 0)
			{
				delete _Ptr->_Ptr;
				_Ptr->_Ptr = NULL;
				delete _Ptr;
			}
			_Ptr = NULL;
		}
	}

private:
	RefPtr *_Ptr;
};

/*
template<class _Ty>
class SmartPtr {
public:
	typedef _Ty element_type;
	explicit SmartPtr(_Ty *_P = 0) _THROW0()
		: _Owns(_P != 0), _Ptr(_P) {}
	SmartPtr(const SmartPtr<_Ty>& _Y) _THROW0()
		: _Owns(_Y._Owns), _Ptr(_Y.release()) {}
	SmartPtr<_Ty>& operator=(const SmartPtr<_Ty>& _Y) _THROW0()
		{if (this != &_Y)
			{if (_Ptr != _Y.get())
				{if (_Owns)
					delete _Ptr;
				_Owns = _Y._Owns; }
			else if (_Y._Owns)
				_Owns = true;
			_Ptr = _Y.release(); }
		return (*this); }
	~SmartPtr()
		{if (_Owns)
			delete _Ptr; }
	_Ty& operator*() const _THROW0()
		{return (*get()); }
	_Ty *operator->() const _THROW0()
		{return (get()); }
	_Ty *get() const _THROW0()
		{return (_Ptr); }
	_Ty *release() const _THROW0()
		{((SmartPtr<_Ty> *)this)->_Owns = false;
		_Ty *_tPtr = _Ptr;
		_Ptr = 0;
		return (_tPtr); }
	bool operator !()
	{
		if(_Ptr == 0) return TRUE;
		return FALSE;
	}

	template<class newType>
	operator SmartPtr<newType> ()
	{
		return SmartPtr<newType> (_Ptr);
	}
private:
	bool _Owns;
	_Ty *_Ptr;
};
*/
#endif