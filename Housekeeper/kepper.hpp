#ifndef _KEPPER_HPP_
#define _KEPPER_HPP_
#include<iostream>
#include<string>
#include<cstdio>
#include<sstream>
#include<memory>
#include<json/json.h>
#include"speech/speech.h"
#include"speech/base/http.h"
#include<map>
#include<unordered_map>

#define CMD_ETC "command.etc"
#define VOICE_PATH "voice" 
#define ASR "asr.wav"
#define TTL "ttl.mp3"

using namespace std;

class Tuling{
private:
  string url = "http://openapi.tuling123.com/openapi/api/v2";
  string apikey = "1df9ce5a10bc4006806169ec1b0c08f2";
  string userid = "1";
  aip::HttpClient client;
public:
  Tuling(){

  }

  string TResponse(string& response){
    JSONCPP_STRING errs;
    Json::Value root;
    Json::CharReaderBuilder rdb;
    std::unique_ptr<Json::CharReader> const jsonReader(rdb.newCharReader());
    bool res = jsonReader->parse(response.data(),response.data()+response.size(),&root,&errs);
    if(!res||!errs.empty()){
      cout<<"反序列化出错"<<endl;
      return errs;
    }
    Json::Value _res = root["results"];
    Json::Value _val = _res[0]["values"];
    return _val["text"].asString();
  }

  string Chat(string content){

  Json::Value root;
  root["reqType"] = 0;

  Json::Value message;
  Json::Value text;
  text["text"] = content;
  message["inputText"] = text;

  root["perception"] = message;

  Json::Value user;
  user["apiKey"] = apikey;
  user["userId"] = userid;
  root["userInfo"] = user;
  Json::StreamWriterBuilder wb;
  std::ostringstream os;

  std::unique_ptr<Json::StreamWriter> jsonWriter(wb.newStreamWriter());
  jsonWriter->write(root,&os);
  std::string body =  os.str(); //构造好的json串,接下来就是推送给图灵机器人
  
  //推送给图灵机器人，调用百度SDK的http接口
  string response;//post请求的返回值
  int code = client.post(url,nullptr,body,nullptr,&response);
  if(code!=CURLcode::CURLE_OK){
    cout<<"http 请求出错"<<endl;
    return " ";
  }
  string reply = TResponse(response);
  return reply;
  }
  ~Tuling(){

  }
};

class AiVoice{
private:
  Tuling AI;
  aip::Speech *client;

  string appid = "21634398";
  string apikey = "wg6Xr0BwQ68Rf8I7qujCCNeD";
  string secretkey = "Xe9ZyuGHfm9In4Wqx1VyHZPa3o1x1j7Y";

  unordered_map<string,string> cmd_set;//配置文件

  string vcmd;
  string play;
public:
  AiVoice()
    :client(nullptr)
  {

  }

  void LoadCommand(){
    string fname = CMD_ETC;
    ifstream in(fname);
    if(!in.is_open()){
      cout<<"打开配置文件出错"<<endl;
      exit(1);
    }
    char line[1024];
    string sep = ":";
    while(in.getline(line,sizeof(line))){
      string str = line;
      size_t pos = str.find(sep);
      if(pos == string::npos){
        cout<<"配置文件错误"<<endl;
        break;
      }
      string key = str.substr(0,pos);
      string value = str.substr(pos+sep.size());
      key += "。";
      cmd_set.insert({key,value});
    }
    in.close();
  }
  void Init(){
    client = new aip::Speech(appid,apikey,secretkey);
    cout<<"加载配置文件..."<<endl;
    LoadCommand();
    cout<<"配置文件加载完毕..."<<endl;

    vcmd = "arecord -t wav -c 1 -r 16000 -d 5 -f S16_LE ";
    vcmd += VOICE_PATH;
    vcmd += "/";
    vcmd += ASR;
    vcmd += ">/dev/null 2>&1";//将错误信息重定向到dev/null

    play = "cvlc --play-and-exit ";
    play += VOICE_PATH;
    play += "/";
    play += TTL;
    play += ">/dev/null 2>&1";
  }
  //本地录音
  bool Exec(string comd,bool is_out){
    FILE* fp = popen(comd.c_str(),"r");
    if(nullptr == fp){
      cout<<"录音命令失败"<<endl;
      return false;
    }
     
    if(is_out){
      char c;
      size_t s = 0;
      while((s = fread(&c,1,1,fp))>0){
        cout<<c;
      }
    }
    pclose(fp);
    return true;
  }

  string BResponse(Json::Value& root){
    int err_no = root["err_no"].asInt();
    if(err_no!=0){
      cout<<root["err_msg"]<<"错误码"<<err_no<<endl;
      return "unknow";
    }
    return root["result"][0].asString();
    
  }

  string BASR(aip::Speech* client){
    string asr_file = VOICE_PATH;
    asr_file += "/";
    asr_file += ASR;

    map<string,string> options;
    string file_content;

    aip::get_file_content(asr_file.c_str(),&file_content);//将录好的语音读取到file_content

    Json::Value root = client->recognize(file_content,"wav",16000,options);
    /*return root.toStyledString();*/

   string bres = BResponse(root);
   return bres;
  }


  bool BTTL(aip::Speech*client,string& str){
    ofstream ofile;
    string ttl = VOICE_PATH;
    ttl += "/";
    ttl += TTL;
    ofile.open(ttl.c_str(),ios::out | ios::binary);

    string file_ret;
    map<string,string> options;

    options["spd"] = "5";
    options["per"] = "4";

    Json::Value result = client->text2audio(str,options,file_ret);
    if(!file_ret.empty()){
      ofile << file_ret;
    }
    else{
      cout<<result.toStyledString()<<endl;
    }
    ofile.close();
  }

  bool IsCommand(string& mes){
    return cmd_set.find(mes) != cmd_set.end() ? true:false;
  }

  void Run(){
    cout<<"录音中..."<<endl;
    fflush(stdout);
    if(Exec(vcmd,false)){
      
      cout<<"识别中...";
      fflush(stdout);
      
      string mes = BASR(client);
      cout<<endl;
      if(IsCommand(mes)){
        //是命令
        Exec(cmd_set[mes],true);
       /* continue;*/
      }
      else{
        //不是命令
        
        cout<<"我：";
        cout<<mes<<endl;
        string Tres = AI.Chat(mes);
        cout<<"语音管家："<<Tres<<endl;
        //文本转语音
        BTTL(client,Tres);

        
        Exec(play,false);
      }
    }

  }
};
#endif
