//
// Created by Kirill Golubev on 01.04.2021.
//

#include "controller.h"
#include "../../magic_enum/include/magic_enum.hpp"

std::string controller::receive_command(controller::cmd c) {
    return "received this cmd: " + std::string(magic_enum::enum_name<cmd>(c));
}
