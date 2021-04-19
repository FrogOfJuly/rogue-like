//
// Created by Kirill Golubev on 01.04.2021.
//

#include <pybind11/pybind11.h>

#include "game_state/gamestate.h"

namespace py = pybind11;

PYBIND11_MODULE(roguelike, m) {
    py::class_<roguelike::gamestate>(m, "GameState")
            .def(py::init<>())
            .def("initialize", &roguelike::gamestate::initialize,
                 py::arg("player_number")
            )
            .def("receive_player_command", &roguelike::gamestate::receive_player_command,
                 py::arg("player_idx"),
                 py::arg("command")
            )
            .def("perform_interactions", &roguelike::gamestate::interact)
            .def("move_players", &roguelike::gamestate::move_players)
            .def("move_nonplayers", &roguelike::gamestate::move_nonplayers)
            .def("move_nonplayers", &roguelike::gamestate::move_nonplayers)
            .def("clean_dead", &roguelike::gamestate::clean_dead)
            .def("clean_decisions", &roguelike::gamestate::clean_decisions)
            .def("get_serialization", &roguelike::gamestate::get_serialization)
            .def("receive_player_command", &roguelike::gamestate::receive_player_command);


    py::enum_<roguelike::cmd>(m, "cmd")
            .value("UP", roguelike::cmd::UP)
            .value("DOWN", roguelike::cmd::DOWN)
            .value("LEFT", roguelike::cmd::LEFT)
            .value("RIGHT", roguelike::cmd::RIGHT)
            .value("ESC", roguelike::cmd::ESC)
            .value("PASS", roguelike::cmd::PASS)
            .value("ENTER", roguelike::cmd::ENTER)
            .export_values();
}


