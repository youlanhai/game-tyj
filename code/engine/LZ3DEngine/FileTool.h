#pragma


typedef std::vector<std::string> StringPool;

void formatSlash(std::string & name);

void formatDirName(std::string & dirName);

std::string getPathFile(const std::string & filename);

std::string getFileName(const std::string & filename);

std::string getFileExt(const std::string & filename);

bool findFiles(StringPool & vect, 
               const std::string & path, 
               const std::string & ext,
               bool unionPath=true);

//////////////////////////////////////////////////////////////////////////
//文件搜索
//////////////////////////////////////////////////////////////////////////
class cFindFile
{
public:
    cFindFile() : m_handle(NULL) {   }
    ~cFindFile(){ reset(); }

    bool findFirst(const std::string & name);

    //必须调用一次findNext才可用。
    bool findNext();

    bool valid(){ return NULL != m_handle && INVALID_HANDLE_VALUE!=m_handle; }

    std::string fileName(){ return m_data.cFileName; }

    bool isDot()
    { 
        std::string fname = fileName();
        return fname=="." || fname==".."; 
    }

    bool isDir()
    {
        return (m_data.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) != 0;
    }

private:

    void reset();

private:
    WIN32_FIND_DATAA m_data;
    HANDLE      m_handle;
};
