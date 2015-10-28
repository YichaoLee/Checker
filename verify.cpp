#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sstream>
using namespace std;

string split(string filename);
void compile(string filename);
void opt(string name, int b, int l, char* e);

int main(int argc, char* argv[]){
    if(argc < 6){
        cout << "Usage: verify filename -l <line> -b <bound>" << endl;
        exit(1);
    }
  
    //Process the flags
    string filename(argv[1]);
    cout << "<" << filename << "> \n";
    char* expression = NULL;
    int line,bound;
    for(int i = 2; i < argc; i ++){
        if(argv[i][0] == '-' && strlen(argv[i]) == 2){
            if(argv[i][1] == 'l'){
                line = atoi(argv[i+1]); 
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
                cout << "Usage: verify filename -l <line> -b <bound>" << endl;
                exit(1); 
            }
        }
        else{
            cout << "Usage: verify filename -l <line> -b <bound>" << endl;
            exit(1);
        }
    }
    string name = split(filename);
    compile(name);
    opt(name, bound, line, expression);
    return 0;
}

void compile(string name){
    string cflags = "-Werror -emit-llvm -g ";
    string command = "clang -c " + cflags + name + ".c " + "-o "+ name + ".bc";
    system(command.c_str());
    return;
}

void opt(string name, int b, int l, char* e){
    stringstream s1,s2;
    s1 << b;
    s2 << l;
    string bound = "-bound=" + s1.str();
    string line = "-line=" + s2.str();
    string expr = "";
    if(e != NULL)
        expr += "-expression=" + string(e);
    string command = "opt -load buildCFG.so -buildCFG " + bound+ " " + line + " " + expr + " -load libz3.so " + "-load libminisat.so.2.1.0 " + "<" + name + ".bc> " + name + "buildCFG.bc";
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
