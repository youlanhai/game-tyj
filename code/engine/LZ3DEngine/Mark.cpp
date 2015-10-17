#include "Mark.h"

CMark::CMark(void)
    : m_rows(0)
    , m_cols(0)    
{
}

CMark::CMark(int rows, int cols)
{
    init(rows,cols);
}

CMark::~CMark(void)
{
}

void CMark::init(int rows, int cols)
{
    m_rows = rows;
    m_cols = cols;
    size_t size = rows*cols;
    if (size <= 0)
    {
        return;
    }
    m_mark.resize(size);
}

void CMark::reset(char destValue)
{
    if (m_mark.size() > 0)
    {
        memset(&m_mark[0], destValue, m_mark.size());
    }
}

char CMark::get(int r, int c)
{
    return m_mark.at(r*m_cols + c);
}

void CMark::set(int r, int c, char mark)
{
    m_mark[r*m_cols+c] = mark;
}

