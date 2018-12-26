//
// Created by ofir8 on 12/20/18.
//

#include "PrintCommand.h"

void PrintCommand::doCommand() {
    if (params.at(0).at(0) == '"' && params.at(0).back() == '"') {
        cout << params.at(0) << endl;
    } else {
        vector<string>::iterator it = params.begin();
        Expression* ex = ExpressionBuilder::getExpression(it, params.end());
        if (it != params.end()) {
            cout << "wrong input" << endl;
            delete(ex);
            return;
        }
        cout << ex->calculate() << endl;
        delete(ex);
    }
}