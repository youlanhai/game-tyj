#pragma once

namespace Lazy
{
    class StringConvTool
    {
    public:
        StringConvTool(void)
        {}

        StringConvTool(const tstring & v) 
            : m_value(v) 
        {}

        StringConvTool(const StringConvTool& st) 
            : m_value(st.m_value) 
        {}

        virtual ~StringConvTool(void)
        {}

        const StringConvTool& operator=(const StringConvTool & st)
        {
            m_value = st.m_value;
            return *this;
        }

        const StringConvTool& operator=(const tstring & v)
        {
            m_value = v;
            return *this;
        }

    public:

        int asInt() const;
        size_t asUint() const;
        size_t asHex() const;
        bool asBool() const;
        float asFloat() const;
        double asDouble() const;
        LPCTSTR asText() const{return m_value.c_str();}
        const tstring & asString() const{ return m_value; }
        std::string asUtf8() const;

        void setInt(int data);
        void setUint(size_t data);
        void setHex(size_t data);
        void setBool(bool data);
        void setFloat(float data);
        void setDouble(double data);
        inline void setText(LPCTSTR data){ m_value = data; }
        inline void setString(const tstring& data){m_value = data;}
        void setUtf8(const std::string & data);

        bool formatFrom(LPCTSTR pFormat, ...);
        bool formatTo(LPCTSTR pFormat, ...);

    protected:
        tstring m_value;
    };

}//namespace Lazy