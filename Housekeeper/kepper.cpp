#include"kepper.hpp"

int main()
{
  AiVoice ai;
  ai.Init();
  ai.Run();
  /*Tuling *tl = new Tuling();
  while(1){
    string content;
    cout<<"输入内容"<<endl;
    cin>>content;
    string con = tl->Chat(content);
    cout<<con<<endl;
  }*/
  return 0;
}
