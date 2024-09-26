#pragma once
#ifndef __PATH_HPP_INCLUDED__
#define __PATH_HPP_INCLUDED__

#include <vector>
#include <string>
#define PATH_MAX_LENGTH 256
using namespace std;
class Path
{
private :
    vector<string> components;
public :
    char buffer[PATH_MAX_LENGTH];
    Path();
    Path(string s);
    Path(const Path& p);
    Path(const char*);
    static bool DecomposeStringPath(string p,vector<string>& components);
    string ToString()const;
    Path GetParent();
    bool IsFullPath() const;
    Path operator + (const Path& p1) const;
    Path operator + (string s);
    Path operator + (const char*);
    Path operator = (Path);
    bool operator == (const Path&);
    operator const char* ();
    string GetExtention();
    void SetExtention(std::string);
    static void DeepCopy(Path,Path);
    static bool CopyFile(Path,Path);
    static bool MakeDir(Path);
    static Path Create(const char*);
    void UpdateBuffer();
    bool IsFile();
    bool IsFolder() const;
    string GetFileName();
    const char* ToStr();
    static Path* Concat(Path*,Path*);
    unsigned int GetComponentCount();
    std::string GetComponent(unsigned int);
    void ReplaceComponent(unsigned int,std::string);
    bool MakeRelative(Path&);
    const char * StringCopy();
    static const char * ConcatStrPath2Str(const char * ,const char*);
    static bool StrPathCompare(const char * p1 , const char * p2);
    static bool ChDir(const char * p1);
    static Path CurrentDir();
    void AppendComponent(const char*);
    static bool CreateSymLink(Path* src,Path* trgt);
    bool Exists();
};

#endif // __PATH_HPP_INCLUDED__
