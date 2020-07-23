#include<iostream>
#include<sstream>
#include<memory>
#include<string>
#include<json/json.h>

using  namespace std;

int main()
{
    Json::Value root;
    Json::StreamWriterBuilder wb;
    std::ostringstream os;

    root["name"] = "张三";
    root["age"] = 29;
    root["lang"] = "c++";

    std::unique_ptr<Json::StreamWriter> jw(wb.newStreamWriter()); 
    jw->write(root,&os);
    std::string result = os.str();

    cout<<result<<endl;


    return 0;
}
