//
// Created by Kirill Golubev on 01.04.2021.
//

#include <string>

#ifndef CPP_PYTHON_TEST_CONTROLLER_H
#define CPP_PYTHON_TEST_CONTROLLER_H


class controller {

public:
    enum cmd {UP = 0, DOWN, LEFT, RIGHT, ENTER, ESC, PASS};

    std::string receive_command(cmd c);
};


#endif //CPP_PYTHON_TEST_CONTROLLER_H
