//
// Created by robin on 8/8/15.
//

#ifndef PDNS_SLAVE_MAINCLASS_H
#define PDNS_SLAVE_MAINCLASS_H

#include <memory>
#include <Business/FreeFareTagBusiness.h>

class MainClass {
public:
    MainClass(int argc, char* argv[]);

    int main();

    int mapKeys(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys);

    int dump(std::shared_ptr<FreeFareTagBusiness> tag, std::vector<std::string> keys);

    void printBlockAccessBits(const AccessBitsDbo& accessBits, int block);

    void printTrailerAccessBits(const AccessBitsDbo& accessBits);

private:
    int _argc;

    char** _argv;

};


#endif //PDNS_SLAVE_MAINCLASS_H
