#include <iostream>
#include <vector>
#include <map>

#include "MapsHandler.h"
#include "Command.h"
#include "OpenDataServerFactory.h"
#include "ConnectFactory.h"
#include "IfFactory.h"
#include "WhileFactory.h"
#include "CommandFactory.h"
#include "PrintFactory.h"
#include "SetFactory.h"
#include "VarFactory.h"
#include "EnterCFactory.h"


#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <fstream>

using namespace std;



vector<string> lexer(const string &line) {
    vector<string> vec;
    std::size_t prev = 0, pos;
    while ((pos = line.find_first_of(" +-*/()<>=!", prev)) != std::string::npos)
    {
        if (pos > prev) {
            vec.push_back(line.substr(prev, pos - prev));
            if (line.at(pos) != ' ') {
                if (pos + 1 < line.length() && line.at(pos+1) == '=') {
                    vec.push_back(line.substr(pos, 2));
                    prev = pos + 2;
                } else {
                    vec.push_back(line.substr(pos, 1));
                    prev = pos + 1;
                }
            } else {
                prev = pos+1;
            }
        } else if (pos == prev) {
            if (line.at(pos) != ' ') {
                if (pos + 1 < line.length() && line.at(pos+1) == '=') {
                    vec.push_back(line.substr(pos, 2));
                    prev = pos + 2;
                } else {
                    vec.push_back(line.substr(pos, 1));
                    prev = pos + 1;
                }
            } else {
                prev = pos+1;
            }
        }
    }
    if (prev < line.length()) {
        vec.push_back(line.substr(prev, std::string::npos));
    }
    return vec;
}
Expression* parser(vector<string> &vec, const map<string, CommandFactory*> &commandMap) {
    if (commandMap.find(vec.at(0)) == commandMap.end()) {
        if (vec.at(1) == "=") {
            return commandMap.at("=")->getCommand(vec, vector<Expression*>());
        } else {
            cout << "wrong input" << endl;
            return nullptr;
        }
    } else {
        vector<string> params(vec.begin() + 1, vec.end());
        return commandMap.at(vec.at(0))->getCommand(params, vector<Expression*>());
    }
}
Expression* loopParser(vector<string> &vec, const map<string, CommandFactory*> &commandMap) {
    if (vec.at(1) == "(") {
        if (*(vec.end() - 1) != ")") {
            cout << "wrong input" << endl;
            return nullptr;
        }
        vec.erase(vec.begin() + 1);
        vec.erase(vec.end() - 1);
    }
    string line;
    vector<Expression*> commands;
    Expression* command;
    vector<string> currLine;
    getline (cin, line);
    currLine = lexer(line);
    do {
        if (currLine.back() == "{") {
            command = loopParser(currLine, commandMap);
        } else {
            command = parser(currLine, commandMap);
        }
        commands.push_back(command);
        getline (cin, line);
        currLine = lexer(line);
    } while (currLine.back() != "}");
    currLine.pop_back();
    if (currLine.begin() != currLine.end()) {
        command = parser(currLine, commandMap);
        commands.push_back(command);
    }
    vector<string> params(vec.begin() + 1, vec.end());
    return commandMap.at(vec.at(0))->getCommand(params, commands);
}
void createCommandMap(map<string, CommandFactory*> &commandMap) {
    commandMap.insert(pair<string, CommandFactory*>("openDataServer", new OpenDataServerFactory()));
    commandMap.insert(pair<string, CommandFactory*>("connect", new ConnectFactory()));
    commandMap.insert(pair<string, CommandFactory*>("var", new VarFactory()));
    commandMap.insert(pair<string, CommandFactory*>("if", new IfFactory()));
    commandMap.insert(pair<string, CommandFactory*>("while", new WhileFactory()));
    commandMap.insert(pair<string, CommandFactory*>("print", new PrintFactory()));
    commandMap.insert(pair<string, CommandFactory*>("=", new SetFactory()));
    commandMap.insert(pair<string, CommandFactory*>("enterc", new EnterCFactory()));
}
int main(int argc, char *argv[]) {
    map<string, CommandFactory*> commandMap;
    createCommandMap(commandMap);
    map<string, double> symbolTable;
    map<string, string> addressTable;
    string line;

    bool fromFile = false;
    std::ifstream f;
    if (argc == 2) {
        f.open(argv[1]);
        fromFile = true;
    }
    std::istream &in = (argc == 2) ? f : std::cin;


    while (true) {
        getline (in, line);
        if (!fromFile &&line == "exit"  || fromFile && in.eof()) {
            break;
        }
        vector<string> vec = lexer(line);
        Expression* command;
        if (vec.back() == "{") {
            vec.pop_back();
            command = loopParser(vec, commandMap);
        }
        else {
            command = parser(vec, commandMap);
        }
        command->calculate();
        delete(command);
    }

    //Delete the factory
    map<string,CommandFactory*>::iterator it= commandMap.begin();

    /*delete(it->second);
    it++;
    delete(it->second);
    it++;
    delete(it->second);
    it++;
    delete(it->second);
    it++;*/

    //closeTwoSockets and the thread
    ComunicateWithSimulator::closeAll();

    return 0;
}