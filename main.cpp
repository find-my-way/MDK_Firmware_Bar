#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <string>

#include "tinyxml2.h"
#include "CLI11.hpp"

using namespace std;
using namespace tinyxml2;

void xml_parse(string project_path, string *m_path);
bool xml_find(XMLElement *element, string value, string *retval);
void map_parse(string map_path);
void show_bar(float precent, int size1, int size2);

int main(int argc, char** argv)
{
    // 创建一个命令行类
    CLI::App argsparse{"firmware size bar"};

    // 添加参数
    // -p 获取项目文件地址
    string project_path;
    argsparse.add_option("-p", project_path, "Project Path")->required();
    // -d 启用debug模式
    bool debug_mode = false;
    argsparse.add_flag("-d", debug_mode, "Enable Debug Mode");
    // 解析命令行输入
    try
    {
        argsparse.parse(argc, argv);
    }
    catch (const CLI::ParseError e)
    {
        cout << "args parse error! Please, check input" << endl;
        return argsparse.exit(e);
    }
    // 解析xml文件, 获取map文件路径
    string map_path;
    xml_parse(project_path, &map_path);
    if (debug_mode)
    {
        cout << map_path << endl;
    }
    // 解析map文件
    map_parse(map_path);

    return 0;
}

void xml_parse(string project_path, string *m_path)
{
    XMLDocument doc;
    const char *project_ptr = project_path.c_str();
    string output_dir;
    string output_name;
    // 加载XML文件
    if (doc.LoadFile(project_ptr) != XML_SUCCESS)
    {
        cout << "Can't open project file" << endl;
    }
    // 获取根节点
    XMLElement *root = doc.RootElement();
    if (root == nullptr)
    {
        cout << "Failed to find root element!" << endl;
    }
    // 查找OutputDirectory, OutputName
    xml_find(root, "OutputDirectory", &output_dir);
    xml_find(root, "OutputName", &output_name);
    // 拼接路径
    filesystem::path p_path(project_path);
    filesystem::path map_path = p_path.parent_path();
    int pos = output_dir.find(".\\");
    if (pos > -1)
    {
        output_dir = output_dir.erase(pos, 2);
    }
    map_path /= output_dir;
    map_path /= output_name + ".map";
    *m_path = map_path.string();
}

bool xml_find(XMLElement *element, string value, string *retval)
{
    const char *temp;

    if (element == nullptr)
        return false;
    if (string(element->Value()) == value)
    {
        temp = element->GetText();
        *retval = string(temp);
        return true;
    }

    XMLElement *child = element->FirstChildElement();
    while(child)
    {
        if (xml_find(child, value, retval))
        {
            return true;
        }
        child = child->NextSiblingElement();
    }
    
    return false;
}

void map_parse(string map_path)
{
    ifstream file(map_path);
    if (!file.is_open())
    {
        cout << "Can't open map file" << endl;
    }

    string line;
    string size;
    regex regex(R"(Execution Region (\S+) \(Exec base: (\S+), Load base: (\S+), Size: (\S+), Max: (\S+), (\S+)\))");
    smatch match;
    while (getline(file, line))
    {
        if (line.starts_with("    Execution Region "))
        {
            if (regex_search(line, match, regex))
            {
                string region_name = match[1];
                int region_size = stoi(string(match[5]).substr(2), nullptr, 16);
                int region_occupy_size = stoi(string(match[4]).substr(2), nullptr, 16);
                float occupy_precent = (float)region_occupy_size / region_size;
                cout << region_name.substr(3) << " ";
                show_bar(occupy_precent, region_occupy_size, region_size);
            }
        }
    }
}

void show_bar(float precent, int size1, int size2)
{
    int bar_width = 50;
    int bar_count = (int)(bar_width * precent);
    int count = 0;
    cout << "[";
    for (int i = 0; i < bar_width; i++)
    {
        if (count < bar_count)
        {
            cout << "=";
        }
        else if (count == bar_count)
        {
            cout << ">";
        }
        else if (count > bar_count)
        {
            cout << " ";
        }
        count++;
    }
    cout << fixed << setprecision(2);
    cout << "] ";
    string temp = to_string((float)size1 / 1024);
    string str = temp.substr(0, temp.find('.') + 3) + "KB/" + to_string(size2 / 1024) + "KB ";
    cout << setw(20) << setfill(' ') << left << str;
    cout << precent * 100 << "%" << endl;
}