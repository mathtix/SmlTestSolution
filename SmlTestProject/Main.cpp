#include <iostream>
#include <format>

#include <boost/sml.hpp>

namespace sml = boost::sml;

struct machine_logger {
    template<class T>
    auto name() { return sml::aux::get_type_name<T>(); }

    template <class SM, class TEvent>
    void log_process_event(const TEvent&) {
        std::cout << std::format("[{}][process_event] {}\n", name<SM>(), name<TEvent>());
    }
    template <class SM, class TGuard, class TEvent>
    void log_guard(const TGuard&, const TEvent&, bool result) {
        std::cout << std::format("[{}][guard] {} {} {}\n", name<SM>(), name<TGuard>(), name<TEvent>(), (result ? "[OK]" : "[Reject]"));
    }
    template <class SM, class TAction, class TEvent>
    void log_action(const TAction&, const TEvent&) {
        std::cout << std::format("[{}][action] {} {}\n", name<SM>(), name<TAction>(), name<TEvent>());
    }
    template <class SM, class TSrcState, class TDstState>
    void log_state_change(const TSrcState& src, const TDstState& dst) {
        std::cout << std::format("[{}][transition] {} -> {}\n", name<SM>(), src.c_str(), dst.c_str());
    }
};

// Events
struct e1 {};
struct e2 {};
struct e3 {};

struct S1 {};
struct S2 {};
struct S3 {};

struct Controller
{
    auto operator()() const
    {

        return sml::make_transition_table(
            sml::state<S2>     <= *sml::state<S1> + sml::event<e1>,
            sml::state<S3>     <= sml::state<S2> + sml::event<e2>,
            sml::X <= sml::state<S3> + sml::event<e3>
        );
    }
};

int main()
{
    machine_logger logger;
    sml::sm <Controller, sml::logger<machine_logger>> sm{logger};

    std::cout << std::format("Is in state S1 = {}\n", (sm.is(sml::state<S1>) ? "[True]" : "[False]"));
    sm.process_event(e1{});
    std::cout << std::format("Is in state S1 = {}\n", (sm.is(sml::state<S1>) ? "[True]" : "[False]"));
    sm.process_event(e2{});
    sm.process_event(e3{});

    return 0;
}
