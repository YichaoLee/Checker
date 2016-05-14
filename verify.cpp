#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sstream>
using namespace std;

string split(string filename);
void compile(string filename);
void opt(string name, int b, char* f, char* e);

int main(int argc, char* argv[]){
    if(argc < 6){
        cout << "Usage: verify filename -f <func> -b <bound>" << endl;
        exit(1);
    }
  
    //Process the flags
    string filename(argv[1]);
    cout << "<" << filename << "> \n";
    char* expression = NULL;
    char* func = NULL;
    int bound;
    for(int i = 2; i < argc; i ++){
        if(argv[i][0] == '-' && strlen(argv[i]) == 2){
            if(argv[i][1] == 'f'){
                func = argv[i+1]; 
                i ++;
            }
            else if(argv[i][1] == 'b'){
                bound = atoi(argv[i+1]);
                i ++;
            }
            else if(argv[i][1] == 'e'){
                expression = argv[i+1];
                i ++;
            }
            else{
                cout << "Usage: verify filename -f <func> -b <bound>" << endl;
                exit(1); 
            }
        }
        else{
            cout << "Usage: verify filename -f <func> -b <bound>" << endl;
            exit(1);
        }
    }
    string name = split(filename);
    compile(name);
    opt(name, bound, func, expression);
    return 0;
}

void compile(string name){
    string cflags = "-emit-llvm -g ";
    string command = "clang -c " + cflags + name + ".c " + "-o "+ name + ".bc";
    system(command.c_str());
    return;
}

void opt(string name, int b, char* f, char* e){
    stringstream s1;
    s1 << b;
    string bound = "-bound=" + s1.str();
    string func = "-func=" + string(f);
    string expr = "";
    if(e != NULL)
        expr += "-expression=" + string(e);
    string command = "opt -load buildCFG.so -buildCFG " + bound+ " " + func + " " + expr + "<" + name + ".bc> " + name + "buildCFG.bc";
//    cout << "<" << name << ".c> \t";
    system(command.c_str());
}
string split(string filename){
    int i;
    for(i = 0; i < filename.length(); i ++){
        if(filename.at(i) == '.')
            break;
    }
    return filename.substr(0,i);
}
