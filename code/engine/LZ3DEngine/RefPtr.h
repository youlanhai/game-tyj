
#ifndef LAZY3D_REFPTR_H
#define LAZY3D_REFPTR_H

#include <cassert>

/** 引用计数类*/
class  IBase
{
public:
    IBase(void) : m_bManaged_(true),m_nRefCounter_(0) { };
    virtual ~IBase(void) { }

    /** 增加引用计数*/
    void  addRef(void) { ++m_nRefCounter_; }

    /** 减少引用计数*/
    void  delRef(void)
    {
        --m_nRefCounter_;
        if (m_nRefCounter_<=0 && m_bManaged_) 
        {
            destroyThis();
        }
    }

    /** 获得引用计数*/
    int getRef(void){ return m_nRefCounter_; }

	bool isRefUnique(){ return m_nRefCounter_ == 1; }

public://属性

    /*设置资源管理。*/
    void setManaged(bool m){ m_bManaged_ = m; }
    bool getManaged(void){ return m_bManaged_ ; }
    virtual void destroyThis(void){ delete this; }

private:
    bool    m_bManaged_ ;   //自动管理资源
    int     m_nRefCounter_ ;//引用计数
};

/** 智能指针基类*/
template<typename T>
class BaseSmartPtr 
{
public:
    typedef T type;

	BaseSmartPtr(type* ptr) 
		: m_ptr(ptr)
	{}

	~BaseSmartPtr(void){}

	type* operator->() const 
	{
		assert(m_ptr && "smartptr is null!");

		return m_ptr;
	}

	type& operator*() const 
	{
		assert(m_ptr && "smartptr is null!");

		return *m_ptr;
	}

	operator bool() const 
	{
		return (m_ptr != NULL);
	}

	type* get() const 
	{
		return m_ptr;
	}

protected:
	type* m_ptr;
};

/** 侵入式智能指针*/
template<typename T>
class RefPtr : public BaseSmartPtr<T>
{
public:
    typedef RefPtr<type> ThisType;

	RefPtr()
		: BaseSmartPtr(0)
	{

	}

    RefPtr(type* ptr)
		: BaseSmartPtr(ptr)
    {
		safeAddRef(m_ptr);
    }

    RefPtr(const ThisType & ptr) 
		: BaseSmartPtr(ptr.m_ptr)
    {
		safeAddRef(m_ptr);
    }

    ~RefPtr(void) 
    {
        safeDelRef(m_ptr);
    }

    const ThisType & operator=(type* ptr) 
    {
        if (ptr != m_ptr) //防止自复制
		{
			safeAddRef(ptr);
            safeDelRef(m_ptr);
            m_ptr = ptr;
        }
        return *this;
    }

    const ThisType & operator=(const ThisType & ptr) 
    {
        return *this = ptr.m_ptr;
    }

    ///强制类型转换
    template<typename U>
    const ThisType & operator=(U* ptr)
    {
        return *this = dynamic_cast<type*>(ptr);
    }

    ///强制类型转换
    template<typename U>
    const ThisType & operator=(const RefPtr<U> & ptr) 
    {
        return *this = dynamic_cast<type*>(ptr.get());
    }

    ///强制类型转换
    template<typename U>
    operator RefPtr<U>() 
    {
        return RefPtr<U>((U*)m_ptr);
    }

private:

	inline void safeAddRef(type * ptr)
	{
		if (ptr != NULL) 
		{
			ptr->addRef();
		}
	}

	inline void safeDelRef(type * ptr)
	{
		if (ptr != NULL) 
		{
			ptr->delRef();
		}
	}

};


template<typename T, typename U>
bool operator==(const RefPtr<T>& a, const RefPtr<U>& b) 
{
    return (a.get() == b.get());
}

template<typename T>
bool operator==(const RefPtr<T>& a, const T* b)
{
    return (a.get() == b);
}

template<typename T>
bool operator==(const T* a, const RefPtr<T>& b) 
{
    return (a == b.get());
}


template<typename T, typename U>
bool operator!=(const RefPtr<T>& a, const RefPtr<U>& b) 
{
    return (a.get() != b.get());
}

template<typename T>
bool operator!=(const RefPtr<T>& a, const T* b) 
{
    return (a.get() != b);
}

template<typename T>
bool operator!=(const T* a, const RefPtr<T>& b)
{
    return (a != b.get());
}

/** 非侵入式智能指针*/
template<typename T>
class SmartPtr : public BaseSmartPtr<T>
{
public:
	typedef RefPtr<IBase>	TypeRefCountPtr;
	typedef SmartPtr<T>		TypeThis;

	SmartPtr() 
		: BaseSmartPtr(0)
	{
	}

	//裸指针只能从构造函数传入
	SmartPtr(T* ptr) 
		: BaseSmartPtr(ptr)
	{
		if (m_ptr)
		{
			m_ref = new IBase(); //只有这一个地方生成引用计数对象
		}
	}

	//拷贝构造函数
	SmartPtr(const TypeThis& other) 
		: BaseSmartPtr(other.m_ptr)
		, m_ref(other.m_ref)
	{
	}

	//强制类型转换过来的拷贝构造
	template<typename U>
	SmartPtr(const SmartPtr<U> & other)
		: BaseSmartPtr(0)
		, m_ref(other.getRef())
	{
		m_ptr = dynamic_cast<T*>(other.get());
	}

	//析构函数
	~SmartPtr(void) 
	{
		safeDelSelfPtr();
	}

	TypeThis & operator=(const TypeThis & other) 
	{
		if (this != &other) //防止自复制
		{
			safeDelSelfPtr();
			m_ptr = other.m_ptr;
			m_ref = other.m_ref;
		}
		return *this;
	}

	template<typename U>
	TypeThis & operator=(const SmartPtr<U> & other) 
	{
		if ((void *)this != (void *)&other) //防止自复制
		{
			safeDelSelfPtr();
			m_ptr = dynamic_cast<T*>(other.get());
			m_ref = other.getRef();
		}
		return *this;
	}


	TypeRefCountPtr getRef() const
	{
		return m_ref;
	}

	int getRefCount() const
	{
		if (m_ref)
		{
			return m_ref->getRef();
		}
		return 0;
	}
private:

	inline void safeDelSelfPtr()
	{
		if (m_ref && m_ref->isRefUnique())//当引用计数唯一时，才释放被托管指针。
		{
			delete m_ptr;
		}
	}

	TypeRefCountPtr m_ref;

};

template<typename T, typename U>
bool operator==(const SmartPtr<T>& a, const SmartPtr<U>& b) 
{
	return (a.get() == b.get());
}

template<typename T, typename U>
bool operator!=(const SmartPtr<T>& a, const SmartPtr<U>& b) 
{
	return (a.get() != b.get());
}

#endif //LAZY3D_REFPTR_H