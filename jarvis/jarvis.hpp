#ifndef _JARVIS_HPP_
#define _JARVIS_HPP_

#include<iostream>
#include<string>
#include<sstream>
#include<memory>
#include<json/json.h>
#include"speech/base/http.h"

using namespace std;

class Tuling{
    private:
        string url = "http://openapi.tuling123.com/openapi/api/v2";
        string apiKey = "bb17a8c40237429286cd82bd1a894531";
        string userId = "1";
        aip::HttpClient client;
    public:
        Tuling(){

        }
        //聊天内容
        string Chat(string message){
            //构建json串
            Json::Value root;
            root["reqType"] = 0;
            //感知信息
            Json::Value inputtext;//bug?
            Json::Value text;
            text["text"] = message;
            inputtext["inputText"] = text;
            root["perception"] = inputtext;
            //user信息
            Json::Value user;
            user["apiKey"] = apiKey;
            user["userId"] = userId;

            root["userInfo"] = user;

             Json::StreamWriterBuilder wb;
             std::ostringstream os;

             //序列化
              std::unique_ptr<Json::StreamWriter> jsonWriter(wb.newStreamWriter());
              jsonWriter->write(root, &os);

              //json串
              std::string body = os.str();
              
              //接下来向图灵机器人发送http请求
              std::string response;
              //url：请求地址
              //nullptr:params参数：
              //body:数据（json串）
              //nullptr:报头
              //返回
              //发起post请求
              int code = client.post(url,nullptr,body,nullptr,&response);
              if(code != CURLcode::CURLE_OK){
                  cout<<"HTTP requst error!"<<endl;
                  return "";
              }

              return response;




        }
        ~Tuling(){

        }
};
#endif

