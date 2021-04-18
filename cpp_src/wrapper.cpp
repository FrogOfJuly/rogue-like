//
// Created by Kirill Golubev on 01.04.2021.
//

#include <pybind11/pybind11.h>

#include "game_state/gamestate.h"

namespace py = pybind11;

PYBIND11_MODULE(rogue_like, m) {
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


    py::enum_<roguelike::cmd>(m, "Command")
            .value("up", roguelike::cmd::UP)
            .value("down", roguelike::cmd::DOWN)
            .value("left", roguelike::cmd::LEFT)
            .value("right", roguelike::cmd::RIGHT)
            .value("esc", roguelike::cmd::ESC)
            .value("pass", roguelike::cmd::PASS)
            .value("enter", roguelike::cmd::ENTER)
            .export_values();
}


