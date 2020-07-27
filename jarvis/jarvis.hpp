#ifndef _JARVIS_HPP_
#define _JARVIS_HPP_

#include<iostream>
#include<cstdio>
#include<string>
#include<sstream>
#include<memory>
#include<json/json.h>
#include"speech/base/http.h"
#include"speech/speech.h"

#define VOICE_PATH "voice"
#define SPEECH_ASR "asr.wav"
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
        //反序列化
        string ResponsePickup(std::string &str){
            JSONCPP_STRING errs; 
            Json::Value root; 
            Json::CharReaderBuilder rb; 
            std::unique_ptr<Json::CharReader> const jsonReader(rb.newCharReader());
            bool res = jsonReader->parse(str.data(), str.data()+str.size(), &root, &errs); 
            if(!res || !errs.empty()){
                std::cerr << "json parse error: " << errs << std::endl;
                return errs;
            }
            //根据json格式，提取机器人返回的对话内容
            Json::Value _result = root["results"];
            Json::Value values = _result[0]["values"];
            return values["text"].asString();
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


              return ResponsePickup(response);




        }
        ~Tuling(){

        }
};

class Jarvis{
    private:
        Tuling tl;
        aip::Speech *client;

        string aipKey = "wg6Xr0BwQ68Rf8I7qujCCNeD";
        string appid = "21634398";
        string secretkey = "Xe9ZyuGHfm9In4Wqx1VyHZPa3o1x1j7Y";
    public:
        Jarvis():client(nullptr){

        }
        void Init(){
            client = new aip::Speech(appid,aipKey,secretkey);

        }
        bool Exec(string cmd,bool is_print){
            FILE* fp = popen(cmd.c_str(),"r");
            if(fp == nullptr){
                cout<<"open error!"<<endl;
                return false;
            }
            string result;
            if(is_print){
                char c;
                size_t s = 0;
                while((s = fread(&c,1,1,fp)) > 0){
                    result.push_back(c);
                }
                cout<<result<<endl;
            }
            pclose(fp);
            return true;
        }
        string RecognizePickup(Json::Value &root){
            
        }   
        string ASR(aip::Speech *client){
            string asr_file = VOICE_PATH;
            asr_file += "/";
            asr_file += SPEECH_ASR;

            
            std::map<string,string> options;
            string file_content;
            aip::get_file_content(asr_file.c_str(),&file_content);
            Json::Value root = client->recognize(file_content,"wav",16000,options);

            return RecognizePickup(root);
        }
        void Run(){
            //录音命令
            string cmd = "arecord -t wav -c 1 -r 16000 -d 5 -f S16_LE ";
            cmd += VOICE_PATH;
            cmd += "/";
            cmd += SPEECH_ASR;
            cmd += "> /dev/null 2>&1";
            cout<<"正在录音..."<<endl;
            fflush(stdout);
            if(Exec(cmd,false)){
               string message = ASR(client);
               cout<<message<<endl;
            }

        }
};
#endif

