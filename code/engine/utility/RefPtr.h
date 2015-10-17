//SmartPtr.h 智能指针
// author: 游蓝海  
// blog: http://blog.csdn.net/you_lan_hai

#ifndef LAZY3D_SMARTPTR_H
#define LAZY3D_SMARTPTR_H

#include <cassert>

namespace Lazy
{

/** 引用计数类*/
class  IBase
{
public:
    IBase(void) : m_nRefCounter(0) { };
    virtual ~IBase(void) { }

    /** 增加引用计数*/
    void  addRef(void) { ++m_nRefCounter; }

    /** 减少引用计数*/
    void  delRef(void)
    {
        --m_nRefCounter;
        if (m_nRefCounter <= 0) 
        {
            destroyThis();
        }
    }

    /** 获得引用计数*/
    int getRef(void){ return m_nRefCounter; }

	bool isRefUnique(){ return m_nRefCounter == 1; }

    virtual void destroyThis(void){ delete this; }

private:
    int     m_nRefCounter;//引用计数
};

/** 智能指针*/
template<typename T>
class RefPtr
{
public:
	RefPtr()
		: m_ptr(0)
	{

	}

    RefPtr(T* ptr)
		: m_ptr(ptr)
    {
		safeAddRef(m_ptr);
    }

    RefPtr(const RefPtr<T>& ptr) 
    {
        m_ptr = const_cast<T*>(ptr.m_ptr);
		safeAddRef(m_ptr);
    }

    ~RefPtr(void) 
    {
        safeDelRef(m_ptr);
    }

    RefPtr<T>& operator=(T* ptr) 
    {
        if (ptr != m_ptr) //防止自复制
		{
			safeAddRef(ptr);
            safeDelRef(m_ptr);
            m_ptr = ptr;
        }
        return *this;
    }

    RefPtr<T>& operator=(const RefPtr<T>& ptr) 
    {
        return *this = const_cast<T*>(ptr.m_ptr);
    }


    //强制类型转换
    template<typename U>
    operator RefPtr<U>() 
    {
        return RefPtr<U>((U*)m_ptr);
    }

	T* operator->() const 
	{
		assert(m_ptr && "pointer is null!");

		return m_ptr;
	}

	T& operator*() const 
	{
		assert(m_ptr && "pointer is null!");

		return *m_ptr;
	}

	operator bool() const 
	{
		return (m_ptr != NULL);
	}

	T* get() const 
	{
		return m_ptr;
	}

private:
	T*      m_ptr;

	inline void safeAddRef(T * ptr)
	{
		if (ptr != NULL) 
		{
			ptr->addRef();
		}
	}

	inline void safeDelRef(T * ptr)
	{
		if (ptr != NULL) 
		{
			ptr->delRef();
		}
	}
};


template<typename T, typename U>
bool operator==(const RefPtr<T>& a, const RefPtr<U>& b){ return (a.get() == b.get()); }

template<typename T>
bool operator==(const RefPtr<T>& a, const T* b){ return (a.get() == b); }

template<typename T>
bool operator==(const T* a, const RefPtr<T>& b) { return (a == b.get()); }


template<typename T, typename U>
bool operator!=(const RefPtr<T>& a, const RefPtr<U>& b) { return (a.get() != b.get()); }

template<typename T>
bool operator!=(const RefPtr<T>& a, const T* b) { return (a.get() != b); }

template<typename T>
bool operator!=(const T* a, const RefPtr<T>& b) { return (a != b.get()); }

template<typename T, typename U>
bool operator < (const RefPtr<T>& a, const RefPtr<U>& b){ return (a.get() < b.get()); }

template<typename T, typename U>
bool operator > (const RefPtr<T>& a, const RefPtr<U>& b){ return (a.get() > b.get()); }

}//namespace Lazy

#endif //LAZY3D_SMARTPTR_H
