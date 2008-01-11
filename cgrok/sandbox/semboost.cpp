#include <string>
#include <iostream>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
using namespace boost::xpressive;
using namespace std;

int main()
{
    string str("hello there");
    smatch m;
    
    sregex rx = *_ >> bos >> (s1=*_) >> eos >> *_;
    //sregex rx = sregex::compile(".*^.*$.*");
    if(regex_search(str, m, rx))
    {
        cout << m[0] << endl;
    }
    
    return 0;
}

