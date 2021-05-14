//
// Created by Kirill Golubev on 17.04.2021.
//

#include "../common.h"
#include "components.h"

#ifndef ROGUE_LIKE_ENTITY_HPP
#define ROGUE_LIKE_ENTITY_HPP
namespace roguelike {

    struct entity {
        entity() = delete;
        explicit entity(int id) : id{id} {}

        entity_id id{-1};
        int level = -1;
    };

    struct interaction_info {
        bool consolidate = false;
        bool remove_interacted = false;
        bool remove_interacting = false;

        static const interaction_info nothing_happened;
        static const interaction_info item_picked;
        static const interaction_info interacted_got_killed;
        static const interaction_info interacting_got_killed;
        static const interaction_info interacted_got_destroyed;
    };

    template <typename interacted, typename interacting>
    struct interacter {  // C++ does not permit partial function template specialization
      public:
        static inline interaction_info interact(interacted &inted, interacting &inting) {
            return interaction_info::nothing_happened;
        }
    };

    namespace default_interactors {

        template <typename interacted, typename interacting>
        requires has_atk_component<interacting> and has_health_component<interacted>
        struct agressive {
          public:
            static inline interaction_info interact(interacted &inted, interacting &inting) {
                auto dmg = atk_component::calculate_damage(&inting);
                auto rec_dmg = health_component::receive_damage(&inted, dmg);
                std::string inted_name = name_component::get_name(&inted);
                std::stringstream entry;
                entry << "you damaged " << inted_name << " by " << rec_dmg << "\n";
                logging_component::log_entry(&inting, entry.str());
                std::stringstream another_entry;
                another_entry << "you received " << rec_dmg << " damage\n";
                logging_component::log_entry(&inted, another_entry.str());
                bool dead = not health_component::is_alive(&inted);
                if (dead) {
                    std::stringstream death_entry;
                    death_entry << "you have killed " << inted_name << "\n";
                    logging_component::log_entry(&inting, death_entry.str());
                    if constexpr (has_level_component_v<interacted>) {
                        int new_exp = inted.lvl_cpt.experience_on_kill();
                        bool lvlup = expirience_components::gain_experience(&inting, new_exp);
                        if (lvlup) {
                            expirience_components::perform_lvlups(&inting);
                        }
                    }
                    return interaction_info::interacted_got_killed;
                }
                return interaction_info::nothing_happened;
            }
        };

        template <typename interacted, typename interacting>
        struct logging {
          public:
            static inline interaction_info interact(interacted &inted, interacting &inting) {
                std::stringstream entry;
                auto interacted_name = name_component::get_name(&inted);
                entry << "you have interacted with " << interacted_name << ". id: " << std::to_string(inted.id.value)
                      << "\n";
                logging_component::log_entry(&inting, entry.str());
                return interaction_info::nothing_happened;
            }
        };

        template <typename interacted, typename interacting>
        requires has_simple_inventory_component<interacting> and has_pickable_component<interacted>
        struct item_picking {
          public:
            static inline interaction_info interact(interacted &inted, interacting &inting) {
                lwlog_info("somebody with inventory interacted with potion");
                bool successful_pick = inting.s_inv_cpt.pick_item(&inted);
                std::stringstream entry;
                successful_pick ? entry << "you picked " << inted.nm_cpt.name << "\n"
                                : entry << "you can't pick this item\n";
                logging_component::log_entry(&inting, entry.str());
                return interaction_info::item_picked;
            }
        };
    }  // namespace default_interactors
}  // namespace roguelike

#endif  // ROGUE_LIKE_ENTITY_HPP
