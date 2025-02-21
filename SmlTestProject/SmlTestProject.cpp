// SmlTestProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <format>

#include <boost/sml.hpp>

namespace sml = boost::sml;

// Events
struct Startup {};
struct Shutdown {};
struct Initialize {};
struct Deinitialize {};
struct Start {};
struct Pause {};
struct Continue {};
struct Stop {};
struct Exit {};

// Logger helpers
void log_transition(const std::string& ostate, const std::string& istate) { std::cout << std::format("[{}] -> [{}]", ostate, istate) << std::endl; }

// State Machine Definition
struct ControllerMachine {
    auto operator()() const noexcept {
        using namespace sml;

        // Nested Initialized State
        struct Stopped { };

        struct Running { };

        struct Paused { };

        struct Exited { };

        struct Initialized {

            auto operator()() const noexcept {
                return make_transition_table(
                    *state<Stopped> +event<Start> / [] {log_transition("Stopped","Running"); } = state<Running>,
                    state<Running> +event<Pause> / [] { log_transition("Running","Paused"); } = state<Paused>,
                    state<Paused> +event<Continue> / [] { log_transition("Paused","Running"); } = state<Running>,
                    state<Running> +event<Stop> / [] { log_transition("Running","Stopped"); } = state<Stopped>,
                    state<Paused> +event<Stop> / [] { log_transition("Paused","Stopped"); } = state<Stopped>,
                    state<Stopped> +event<Exit> / [] { log_transition("Stopped","Exited"); } = state<Exited>,
                    state<Running> +event<Exit> / [] { log_transition("Running","Exited"); } = state<Exited>,
                    state<Paused> +event<Exit> / [] { log_transition("Paused","Exited"); } = state<Exited>
                );
            }
        };

        struct Uninitialized { };

        // State Machine Transitions
        return make_transition_table(
            *state<Uninitialized> +event<Initialize> / [] { log_transition("Uninitialized","Initialized"); } = state<Initialized>,
            state<Initialized> +event<Exit> / [] { log_transition("Initialized","Uninitialized"); } = state<Uninitialized>
        );
    }
};

int main() {
    sml::sm<ControllerMachine> sm;

    std::cout << "Is Uninitialized" <<  sm.is(sml::state<class Uninitialized>) << std::endl;

    sm.process_event(Start{});
    sm.process_event(Initialize{});
    sm.process_event(Start{});
    sm.process_event(Pause{});
    sm.process_event(Continue{});
    sm.process_event(Stop{});
    sm.process_event(Exit{});

    return 0;
}

