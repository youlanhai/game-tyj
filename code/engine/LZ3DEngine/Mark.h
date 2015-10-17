#pragma once
#include "base.h"
#include <vector>

class CMark : public IBase
{
public:
    typedef char Type;
    typedef std::vector<Type>       MarkVector;
    typedef MarkVector::iterator    MarkIterator;

    CMark(void);
    CMark(int rows, int cols);
    ~CMark(void);

    void init(int rows, int cols);
    void reset(char destValue);
    char get(int r, int c);
    void set(int r, int c, char mark);

    int getRows(void){ return m_rows; }
    int getCols(void){ return m_cols; }
protected:
    int m_rows;
    int m_cols;
    MarkVector m_mark;
};
