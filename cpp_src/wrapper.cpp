//
// Created by Kirill Golubev on 01.04.2021.
//

#include <pybind11/pybind11.h>

#include "controller/controller.h"

namespace py = pybind11;

PYBIND11_MODULE(rogue_like, m) {
    py::class_<controller>(m, "Controller")
            .def(py::init<>())
            .def("submit_action", &controller::receive_command);


    py::enum_<controller::cmd>(m, "Command")
            .value("up", controller::cmd::UP)
            .value("down", controller::cmd::DOWN)
            .value("left", controller::cmd::LEFT)
            .value("right", controller::cmd::RIGHT)
            .value("esc", controller::cmd::ESC)
            .value("pass", controller::cmd::PASS)
            .value("enter", controller::cmd::ENTER)
            .export_values();
}


