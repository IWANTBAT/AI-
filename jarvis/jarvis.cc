#include"jarvis.hpp"

int main()
{
    Tuling *tl = new Tuling();
    string message;

    while(1){
        cout<<"请输入 ";
        cin >> message;
        string result = tl->Chat(message);
        cout<<result<<endl;
    }
    return 0;
}
