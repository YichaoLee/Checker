#include "general.h"
using namespace std;

void pause_key()
{
  cout << "waiting for key:" << endl;
  char c;
  cin >> c;
}          

void throw_error(const char*  s){
	cout << "fatal error: " << s << endl << flush;
	exit(1);
}

void throw_error(string msg){
	throw_error(msg.c_str());
}
void throw_warning(string s)
{
  cout << "WARNING: " << s << flush << endl;
  cout << "Press a key and then press Enter to continue:"<< flush << endl;
  pause_key();
}


void progress_dot()
{
 
}

void progress_dot_end()
{
}

void message(unsigned int level,string content){
	if(VERBOSE_LEVEL>=(int)level)
	  std::cout<<content<<std::endl;
}

// convert integer to char, but only up to 256 digits!
string int2string(const int i)
{
  char chr[256];
  string str;
  sprintf(chr,"%i",i);
  str=chr;
  return str;
}
 // convert integer to char, but only up to 256 digits!
string double2string(const double d)
{
  char chr[256];
  string str;
  sprintf(chr,"%g",d);
  str=chr;
  return str;
}



