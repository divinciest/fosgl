#include "path.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <unistd.h>
#include <dirent.h>
#include <assert.h>
#include <fstream>
#define  IS_SEPERATOR(X) (X=='\\' or X=='/')
#define DEFAULT_SEPERATOR '/'
#include <stdexcept>
#include <Helpers/Helpers.hpp>
#include "path.h"
Path::Path(){}
Path::Path(string s)
{
    if(!DecomposeStringPath(Helpers::String::RemoveSpacesFromSides(s),components))
      throw std::invalid_argument("Given string is not a valide path");
}
Path::Path(const char * s):Path(std::string(s))
{
}
Path Path::GetParent()
{
    if (components.size() == 0) return *this;
    Path parent = *this;
    parent.components.pop_back();
    return parent;
}
Path::Path(const Path& p):components(p.components) {}
string Path::GetExtention()
{
    if (components.size() == 0) return "";
    string last = components.back();
    int pos = 0;
    int len = last.length();
    pos = last.find_last_of(".");
    if (pos==-1)
        return "";
    return last.substr(pos,len-pos);
}
void Path::DeepCopy(Path f1,Path  f2)
{
    if (f1.IsFile())
    {
        assert(Path::CopyFile(f1,f2));
    }
    if (f1.IsFolder())
    {
        assert(MakeDir(f2));
        DIR * f1d = opendir(f1);
        assert(f1d);
        dirent* entry = nullptr;
        readdir(f1d);readdir(f1d);
        while (entry = readdir(f1d))
        {
          DeepCopy(f1 + entry->d_name,f2 + entry->d_name);
        }
        closedir(f1d);
    }
}
Path* Path::Concat(Path* p1,Path* p2)
{
    return new Path( (*p1) + (*p2));
}
bool  Path::ChDir(const char * p1)
{
    return (chdir(p1) == 0);
}
Path Path::CurrentDir()
{
    char  buf[256];
    const char * rv = getcwd(&buf[0],256);
    if (rv)
    return Path(rv);
    return Path();
}
/**
 @Experimental
**/
bool Path::MakeRelative(Path& path)
{
    if (path.components.size() == 0) return false;
    if (components.size() == 0) return false;
    int pos_in_this_path = 0;
    bool start_removing = false;
    for (auto & c : components) //// for each component of this path
    {
        if (c == *path.components.begin()) //// if one of them match the beginning f the other path
        {
            if (components.size()-pos_in_this_path  > path.components.size() ) /// this path is more deep than the other , it can't be relative to it
            {
                /// we should go back some steps
                unsigned int depth_difference = (components.size() - pos_in_this_path)-path.components.size();
                path.components.clear();
                for (unsigned  i = 0;i<depth_difference;i++)
                 {
                     path.components.push_back(".."); /// go back honey !
                 }
                 return true;
            }
            start_removing = true; /// we start removing from that point all common components
            path.components.erase(path.components.begin());
        }else{ /// components does not match
          if (start_removing) /// did we al ready find a common component ?
            break;/// no more common components
        }
        pos_in_this_path++;
    }
    return start_removing;
}
Path Path::Create(const char* s)
{
    return Path(s);
}
bool Path::MakeDir(Path dir)
{
     unsigned int count = dir.GetComponentCount();
    if (count == 0) return false;
    std::string current_path;
    DIR* current_dir = nullptr;
    std::string top_created_folder = "";
    for(unsigned int c = 0; c<count; c++)
    {
        current_path += dir.GetComponent(c);
        current_path.push_back('/');
        if ( (current_dir = opendir(current_path.c_str())) == nullptr )
        {
            if (!mkdir(current_path.c_str()))
            {
                if (top_created_folder!="")
                {
                    remove(current_path.c_str());
                    return false;
                }
            }
            else
            {
                if (top_created_folder == "")  top_created_folder = current_path;
            }
        }
        closedir(current_dir);
    }
    return true;
}
bool Path::CopyFile(Path f1,Path f2)
{
    std::ifstream  src(f1, std::ios::binary);
    if (!src.good()) return false;
    std::ofstream  dst(f2,   std::ios::binary);
    if (!dst.good()) return false;
    dst << src.rdbuf();
    return true;
}
bool Path::IsFolder() const
{
#ifdef WIN32
    if (components.size() == 1 && components[0].length()>1 && components[0][1] == ':') return true;
#endif // WIN32
    struct stat path_stat;
    std::string path_cpp_str = ToString();
    stat(path_cpp_str.c_str(), &path_stat);
    return S_ISDIR(path_stat.st_mode);
}
bool Path::IsFile()
{
    struct stat path_stat;
    std::string path_cpp_str = ToString();
    stat(path_cpp_str.c_str(), &path_stat);
    return S_ISREG(path_stat.st_mode);
}
Path Path::operator+ (string s)
{
    Path p(s);
    return (*this)+p;
}
Path Path::operator+ (const char* s)
{
    Path p(s);
    return (*this)+p;
}
Path Path::operator=(Path p)
{
    components.clear();
    for(auto c:p.components) components.push_back(c);
    return p;
}

const char* Path::ToStr()
{
    UpdateBuffer();
    return & buffer[0];
}
Path Path::operator + (const Path& p1) const
{
    assert(!p1.IsFullPath());
    Path rv(*this);
    for (int c=0; c<p1.components.size(); c++)
    {
        rv.components.push_back(p1.components[c]);
    }
    return rv;
}
string Path::ToString() const
{
    string rv;
    if (!components.size()>1)
        rv.push_back(DEFAULT_SEPERATOR);
    for(unsigned int c=0; c<components.size(); c++)
    {
        rv+=components[c];
        if (c == components.size()-1) break;
        rv.push_back(DEFAULT_SEPERATOR);
    }
    if (components.size() == 1&& IsFullPath())
        rv.push_back(DEFAULT_SEPERATOR);
    return rv;
}
const char *  Path::StringCopy()
{
    UpdateBuffer();
    char * rv = new char[strlen(buffer)+1];
    strcpy(rv,buffer);
    return rv;
}
const char *  Path::ConcatStrPath2Str(const char * str_p1,const char* str_p2)
{
   return (Path(str_p1)+str_p2).StringCopy();
}
bool Path::operator == (const Path& other)
{
    if (other.components.size() != components.size()) return false;
        for (unsigned int i = 0;i<components.size();i++)
    {
        if (components[i] != other.components[i]) return false;
    }
    return true;
}
bool Path::StrPathCompare(const char * p1 , const char * p2)
{
   return (Path(p1)==(Path(p2)));
}
bool Path::IsFullPath() const
{
#if defined(_WIN32)
   if (components.size() == 0)
   return false;
   return components[0].find(":")<components[0].length();
#else
#error "No implimentation for this platform"
#endif
}
bool Path::DecomposeStringPath(string p,vector<string>& components)
{
    bool error = false;
    string component="";
    components.clear();
    while (p.length() && IS_SEPERATOR( p[0]))
    {
        p.erase(p.begin());
    }
    char current = '\0';
    for (unsigned int c =0; c<p.length(); c++)
    {
        current = p[c];
        if (IS_SEPERATOR(current))
        {
            if (component == "")
            {
                error = true;
                break;
            }
            components.push_back(component);
            component="";
        }
        else
        {
            if (current=='<' || current=='>' || current=='?' || current=='|' || current=='"' || current=='*')
            {
                error = true;
                break;
            }
            else
            {
                if (current == ':')
                {
                    if (components.size() != 0) // not first component => not drive letter
                    {
                        error = true;
                        break;
                    }
                }
                component.push_back(current);
            }
        }
    }
    if (component.length()) components.push_back(component);
    return !error;
}
unsigned int Path::GetComponentCount()
{
  return components.size();
}
std::string Path::GetComponent(unsigned int index)
{
  assert(index>=0 && index<components.size());
  return components[index];
}
void Path::ReplaceComponent(unsigned int index,std::string c)
{
    components[index] = c;
}
Path::operator const char* ()
{
    UpdateBuffer();
    return buffer;
}
void Path::UpdateBuffer()
{
    strcpy(buffer,ToString().c_str());
}
void Path::AppendComponent(const char* c)
{
    if (c == nullptr) return;
    components.push_back(c);
}
void Path::SetExtention(std::string ex)
{
    if (components.size() == 0)
    {
        components.push_back(ex);
        return;
    }
    auto pos = components.back().find_last_of(".");
    if (pos != std::string::npos)
        components.back().replace(pos, components.back().length()-pos,ex.c_str());
        else
    components.back() += ex;
}
std::string Path::GetFileName()
{
    if (components.size() == 0) return "";
    std::string last = components.back();
    return last.substr(0,last.length()-this->GetExtention().length());
}
#include <filesystem>
bool Path::CreateSymLink(Path* the_new,Path* the_old)
{
  if (!(the_new&&the_old)) return false;
    std::filesystem:: create_symlink(the_old->ToStr(),the_new->ToStr());
    return std::filesystem::is_symlink(the_new->ToStr());
}
bool Path::Exists()
{
    return  std::filesystem::exists(ToStr());
}
