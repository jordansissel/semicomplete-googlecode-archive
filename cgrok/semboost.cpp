#include <string>
#include <iostream>
#include <boost/xpressive/xpressive.hpp>
#include <boost/xpressive/regex_actions.hpp>
using namespace boost::xpressive;

int main()
{
    std::map<std::string, int> result;
    std::string str("aaa=>1 bbb=>23 ccc=>456");
    
    // Match a word and an integer, separated by =>,
    // and then stuff the result into a std::map<>
    sregex pair = ( (s1= +_w) >> "=>" >> (s2= +_d) )
        [ ref(result)[s1] = as<int>(s2) ];
    
    // Match one or more word/integer pairs, separated
    // by whitespace.
    sregex rx = pair >> *(+_s >> pair);

    if(regex_match(str, rx))
    {
        std::cout << result["aaa"] << '\n';
        std::cout << result["bbb"] << '\n';
        std::cout << result["ccc"] << '\n';
    }
    
    return 0;
}

