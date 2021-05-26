//
// Created by Kirill Golubev on 01.04.2021.
//

#include <pybind11/pybind11.h>

#include "game_state/gamestate.h"

namespace py = pybind11;

PYBIND11_MODULE(roguelike, m) {
    py::enum_<roguelike::cmd>(m, "cmd")
        .value("UP", roguelike::cmd::UP)
        .value("DOWN", roguelike::cmd::DOWN)
        .value("LEFT", roguelike::cmd::LEFT)
        .value("RIGHT", roguelike::cmd::RIGHT)
        .value("ESC", roguelike::cmd::ESC)
        .value("PASS", roguelike::cmd::PASS)
        .value("ENTER", roguelike::cmd::ENTER)
        .export_values();

    py::class_<roguelike::gamestate>(m, "GameState")
        .def(py::init<>())
        .def("initialize", &roguelike::gamestate::initialize, py::arg("player_number"))
        .def("initialize_player", &roguelike::gamestate::initialize_player)
        .def(
            "receive_player_command",
            &roguelike::gamestate::receive_player_command,
            py::arg("player_idx") = -1,
            py::arg("command") = roguelike::cmd::PASS)
        .def("resolve_all_interactions", &roguelike::gamestate::resolve_all_interactions)
        .def("move_target_player", &roguelike::gamestate::move_target_player)
        .def("set_decision_target_player", &roguelike::gamestate::set_decision_target_player)
        .def("move_players", &roguelike::gamestate::move_players)
        .def("move_nonplayers", &roguelike::gamestate::move_nonplayers)
        .def("clean_dead", &roguelike::gamestate::clean_dead)
        .def("clean_decisions", &roguelike::gamestate::clean_decisions)
        .def("get_serialization", &roguelike::gamestate::get_serialization)
        .def("clean_logs", &roguelike::gamestate::clean_logs)
        .def("clean_decisions", &roguelike::gamestate::clean_decisions)
        .def("end_turn", &roguelike::gamestate::end_turn)
        .def("decide_next_move", &roguelike::gamestate::decide_next_move);
}
