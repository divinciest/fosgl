
void error_handler(int status, const char *message)
{
    KAGUYA_UNUSED(status);
    std::cout<<"Error : "<<message<<std::endl;
}
using namespace std;
Path * CreatePath(const char* s)
{
    return new Path(s);
}
#define BEGIN_FUNCTION(Tag) \
            functions.push_back\
            (\
             std::make_pair(\
                            std::string(Tag),\
                            [this] (TiXmlElement* node ) -> void\
                            {\

#define END_FUNCTION(...) for (auto e  = node->FirstChildElement();e != nullptr;e = e->NextSiblingElement())ExecuteNode(e);   __VA_ARGS__  }));
#define DECLARE_ATTRIB(X,node) const char * CONCAT(X,___) = node->Attribute(#X);if (!CONCAT(X,___)) return;std::string X = Eval(CONCAT(X,___));


#include <VariablesExpander.hpp>
class Executer
{
   protected:
       kaguya::State state;
       VariablesExpander variables;
   public :
    Executer(){}
    virtual void InitLuaState()
    {
         static auto  VariablesExpanderClass =

            kaguya::UserdataMetatable<VariablesExpander>()
            .setConstructors<VariablesExpander()>()
            .addFunction("SetVariable",VariablesExpander::SetVariable);
          static auto PathClass =  kaguya::UserdataMetatable<Path>()
            .setConstructors<Path(const char*)>()
            .addStaticFunction("CreatePath",CreatePath)
            .addStaticFunction("MakeDir",&Path::MakeDir)
            .addStaticFunction("Concat",&Path::Concat)
            .addStaticFunction("ConcatStrPath2Str",&Path::ConcatStrPath2Str)
            .addStaticFunction("StrPathCompare",&Path::StrPathCompare)
            .addStaticFunction("CurrentDir",&Path::CurrentDir)
            .addStaticFunction("ChDir",&Path::ChDir)
            .addStaticFunction("CreateSymLink",&Path::CreateSymLink)
            .addFunction("Exists",&Path::Exists)
            .addFunction("ToStr",&Path::ToStr)
            .addFunction("AppendComponent",&Path::AppendComponent)
            .addFunction("GetParent",&Path::GetParent);
        state.setErrorHandler( error_handler );
        state["Path"].setClass(PathClass);
        state["VariablesExpander"].setClass(VariablesExpanderClass);
        state["variables"] = &variables;
    }
    virtual bool LoadFile(const char*) = 0;
    virtual std::string GetErrorString() = 0;
    virtual  void SetVariable (std::string,std::string) =0;
};
class FosglExecuter : public Executer
{
private :
    std::vector<Path> CurrentPathStack;
    Path CurrentPath;
    std::string ErrorDesc;
    TiXmlDocument doc;
    std::vector<std::pair<std::string,std::function< void(TiXmlElement*)>>> functions;
    std::string last_node="none";
    std::vector<bool> if_stack;
    bool last_if_expression_evaluation = false;
    struct CustomTagInfo
    {
        std::string Tag;
        std::vector<std::string> attributes;
        std::string script;
    };
    std::vector <CustomTagInfo> custom_tags;
public :
    GETTER(ErrorDesc);
    FosglExecuter()
    {
        static bool once = false;
        if (once == true)
            throw std::runtime_error("Only one parser allowed .");
        once = true;
        InitLuaState();
        /// Tasg

        // Folder

        BEGIN_FUNCTION("Folder")
        DECLARE_ATTRIB(name,node);
        Path folder_path = CurrentPathStack.back() + (name);
        if(!Path::MakeDir(folder_path))
        {
            print_note("Failed to make directory : ",folder_path,".");
            return;
        }
        CurrentPath = folder_path;
        CurrentPathStack.push_back(CurrentPath);
        variables.SetVariable("this",CurrentPath.ToString());
        END_FUNCTION(CurrentPathStack.pop_back();)
        // File
        BEGIN_FUNCTION("File")
        DECLARE_ATTRIB(name,node);
        Path ThisPath = CurrentPathStack.back();
        Path file_path = ThisPath + Eval(name);
        std::ofstream fileout(file_path.ToString().c_str());
        if (!fileout)
        {
            print_note("Failed to create file : ",file_path,".");
            return;
        }
        const char * txt = node->GetText();
        if (txt)
        {
            fileout<<Eval(txt);
        }
        fileout.close();
        END_FUNCTION()

          // Script
        BEGIN_FUNCTION("Script")
        const char * script = node->GetText();
        if (script)
        {
            std::string script_string = Eval(script);
            script_string = Helpers::String::RemoveSpacesFromSides(script_string);
            state.dostring(script_string.c_str());

        }
        END_FUNCTION()
          // Custom
        BEGIN_FUNCTION("Declare")
        DECLARE_ATTRIB(attributes,node);
        DECLARE_ATTRIB(script,node);
        DECLARE_ATTRIB(tag,node);
            std::string attribute;
            CustomTagInfo Info;
            Info.script = script;
            Info.Tag = tag;

        for (auto c : attributes)
        {
            if (c == ';')
            {
                Info.attributes.push_back(attribute);
                attribute = "";
            }else
            {
                attribute += c;
            }
        }
        if (attribute.length())
        {
            Info.attributes.push_back(attribute);
        }
        custom_tags.push_back(Info);
        END_FUNCTION()

        BEGIN_FUNCTION("Variable")
        DECLARE_ATTRIB(name,node);
        DECLARE_ATTRIB(value,node);
        variables.SetVariable(name,value);
        END_FUNCTION()

        BEGIN_FUNCTION("Copy")
        DECLARE_ATTRIB(source,node);
        DECLARE_ATTRIB(target,node);
        Path::DeepCopy(Path(source),Path(target));
        END_FUNCTION()


        BEGIN_FUNCTION("Custom")

        DECLARE_ATTRIB(tag,node);
        CustomTagInfo * info = nullptr;
        for (auto & t : custom_tags)
        {
            if (t.Tag == tag)
            {
                info = &t;
                break;
            }
        }
        if (info == nullptr) return;
        for (auto & a : info->attributes)
        {
            const char * attr_val = node->Attribute(a.c_str());
            if (attr_val)
            {
                state[a.c_str()] = attr_val;
            }
        }
        state.dostring(info->script);
        END_FUNCTION()


          // WHILE
        BEGIN_FUNCTION("If")
        DECLARE_ATTRIB(expression,node);
        std::string exe_string;
        exe_string = as_string("if (",expression,") then  expression__=true; else  expression__=false; end;");
        state.dostring(exe_string);
        bool result = state["expression__"];
        last_if_expression_evaluation = (result);
        if (!result)
        {
            return;
        }
        END_FUNCTION()
          // WHILE
        BEGIN_FUNCTION("While")
        bool condition = false;
        do{
        DECLARE_ATTRIB(expression,node);
        std::string exe_string;
        exe_string = as_string("if (",expression,") then  expression__=true; else  expression__=false; end;");
        state.dostring(exe_string);
        condition = state["expression__"];
        last_if_expression_evaluation = (condition);
        if (!condition)
        {
            return;
        }
        END_FUNCTION(}while (condition);)
          // Else
        BEGIN_FUNCTION("Else")
        auto prev_node = node->PreviousSibling();
        if (prev_node && prev_node->ValueTStr() == "If"){
        if (last_if_expression_evaluation == true)
            return;
        }else
        {
            print_note("Invalide 'Else' statement .");
            return;
        }
        END_FUNCTION()
    }
    virtual void InitLuaState()
    {
        variables.SetVariable("this",CurrentPath.ToString());
        Executer::InitLuaState();
    }
     std::string Eval(std::string str)
     {

       str = variables.Eval(str);
       if (str.length() && str[0]=='~'){
        str.erase(str.begin());
        state.dostring("attribute=nil");
        try{
        state.dostring(str);
        } catch (kaguya::LuaTypeMismatch& e)
        {
            print("Exception thrown while executing string : ",e.what());
        }
           const char * str__ = state["attribute"];
           str = str__;
        }
       return str;
    }
     std::string GetErrorString()
     {
         return GetParseError()+"\nError Description : "+GetErrorDesc();
     }
    std::string GetParseError()
    {
        return as_string("Error while parsing file , error '",doc.ErrorDesc(),"' at , (",doc.ErrorRow(),",",doc.ErrorCol(),")\n");
    }
    void SetVariable (std::string var,std::string val)
    {
        variables.SetVariable(var,val);
    }
    bool LoadFile (const char * path)
    {
        bool loadOkay = doc.LoadFile(path);
        if ( !loadOkay )
        {
            ErrorDesc = "File can not be loaded";
            return false;
        }
        if (doc.Error())
        {
            ErrorDesc = as_string("Error while parsing file , error '",doc.ErrorDesc(),"' at , (",doc.ErrorRow(),",",doc.ErrorCol(),")\n");
            return false;
        }

        CurrentPath = Path::CurrentDir();
        CurrentPathStack.push_back(CurrentPath);
        variables.SetVariable("this_dir",Path(path).GetParent().ToString());
        ExecuteNode(doc.FirstChildElement());
        return true;
    }
    void ExecuteNode (TiXmlElement * node = nullptr)
    {
        for (auto & p : functions)
        {
            if (p.first == node->Value())
            {
                if (p.second)
                {
                    p.second(node);
                }
            }
        }
    }

    VariablesExpander & GetVariablesExpender()
    {
        return variables;
    }
};
