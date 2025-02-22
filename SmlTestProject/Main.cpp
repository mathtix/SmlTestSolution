#include <iostream>
#include <format>

#include <boost/sml.hpp>

namespace sml = boost::sml;

struct state_machine_logger {
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
struct e4 {};
struct e5 {};

struct es1 {};
struct es2 {};
struct es3 {};

struct S1 {};
struct S2 {};
struct S3 {};

struct SS1 {};
struct SS2 {};
struct SS3 {};
struct SS4 {};

struct SS {
    auto operator()() const
    {
        return sml::make_transition_table(
            sml::state<SS2>     <= *sml::state<SS1> + sml::event<es1>,
            sml::state<SS3>     <= sml::state<SS2>  + sml::event<es2>,
            sml::X              <= sml::state<SS3>  + sml::event<es3>
        );
    }
};

struct Controller
{
    auto operator()() const
    {
        return sml::make_transition_table(
            sml::state<S2>     <= *sml::state<S1> + sml::event<e1>,
            sml::state<S3>     <= sml::state<S2>  + sml::event<e2>,
            sml::state<SS>     <= sml::state<S3>  + sml::event<e3>,
            sml::state<S3>     <= sml::state<SS>  + sml::event<e4>,
            sml::X             <= sml::state<S3>  + sml::event<e5>
        );
    }
};

int main()
{
    state_machine_logger logger;
    sml::sm <Controller, sml::logger<state_machine_logger>> sm{logger};

    std::cout << std::format("Is in state S1 = {}\n", (sm.is(sml::state<S1>) ? "[True]" : "[False]"));
    sm.process_event(e1{});
    std::cout << std::format("Is in state S1 = {}\n", (sm.is(sml::state<S1>) ? "[True]" : "[False]"));
    sm.process_event(e2{});
    sm.process_event(e3{});
    sm.process_event(es1{});
    sm.process_event(es2{});
    sm.process_event(es3{});
    sm.process_event(e4{});
    sm.process_event(e5{});

    return 0;
}
