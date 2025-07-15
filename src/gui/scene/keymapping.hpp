#pragma once

#include "exceptions.hpp"

#include <GLFW/glfw3.h>

#include <glaze/glaze.hpp>

#include <array>
#include <string>
#include <unordered_map>

namespace gui {

/**
 * @brief Class to represent GLFW keys 
 */
enum class Key : int {
    MOUSE_LEFT = GLFW_MOUSE_BUTTON_LEFT,     // GLFW_MOUSE_BUTTON_1
    MOUSE_RIGHT = GLFW_MOUSE_BUTTON_RIGHT,   // GLFW_MOUSE_BUTTON_2
    MOUSE_MIDDLE = GLFW_MOUSE_BUTTON_MIDDLE, // GLFW_MOUSE_BUTTON_3
    MOUSE_4 = GLFW_MOUSE_BUTTON_4,           // 3
    MOUSE_5 = GLFW_MOUSE_BUTTON_5,           // 4
    MOUSE_6 = GLFW_MOUSE_BUTTON_6,           // 5
    MOUSE_7 = GLFW_MOUSE_BUTTON_7,           // 6
    MOUSE_8 = GLFW_MOUSE_BUTTON_8,           // 7
    //    MOUSE_LAST = GLFW_MOUSE_BUTTON_LAST, //      GLFW_MOUSE_BUTTON_8

    SPACE = GLFW_KEY_SPACE,
    APOSTROPHE = GLFW_KEY_APOSTROPHE,
    COMMA = GLFW_KEY_COMMA,
    MINUS = GLFW_KEY_MINUS,
    PERIOD = GLFW_KEY_PERIOD,
    SLASH = GLFW_KEY_SLASH,
    KEY_0 = GLFW_KEY_0,
    KEY_1 = GLFW_KEY_1,
    KEY_2 = GLFW_KEY_2,
    KEY_3 = GLFW_KEY_3,
    KEY_4 = GLFW_KEY_4,
    KEY_5 = GLFW_KEY_5,
    KEY_6 = GLFW_KEY_6,
    KEY_7 = GLFW_KEY_7,
    KEY_8 = GLFW_KEY_8,
    KEY_9 = GLFW_KEY_9,
    SEMICOLON = GLFW_KEY_SEMICOLON,
    EQUAL = GLFW_KEY_EQUAL,
    A = GLFW_KEY_A,
    B = GLFW_KEY_B,
    C = GLFW_KEY_C,
    D = GLFW_KEY_D,
    E = GLFW_KEY_E,
    F = GLFW_KEY_F,
    G = GLFW_KEY_G,
    H = GLFW_KEY_H,
    I = GLFW_KEY_I,
    J = GLFW_KEY_J,
    K = GLFW_KEY_K,
    L = GLFW_KEY_L,
    M = GLFW_KEY_M,
    N = GLFW_KEY_N,
    O = GLFW_KEY_O,
    P = GLFW_KEY_P,
    Q = GLFW_KEY_Q,
    R = GLFW_KEY_R,
    S = GLFW_KEY_S,
    T = GLFW_KEY_T,
    U = GLFW_KEY_U,
    V = GLFW_KEY_V,
    W = GLFW_KEY_W,
    X = GLFW_KEY_X,
    Y = GLFW_KEY_Y,
    Z = GLFW_KEY_Z,
    LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,
    BACKSLASH = GLFW_KEY_BACKSLASH,
    RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
    GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,
    WORLD_1 = GLFW_KEY_WORLD_1,
    WORLD_2 = GLFW_KEY_WORLD_2,

    ESCAPE = GLFW_KEY_ESCAPE,
    ENTER = GLFW_KEY_ENTER,
    TAB = GLFW_KEY_TAB,
    BACKSPACE = GLFW_KEY_BACKSPACE,
    INSERT = GLFW_KEY_INSERT,
    DELETE = GLFW_KEY_DELETE,
    RIGHT = GLFW_KEY_RIGHT,
    LEFT = GLFW_KEY_LEFT,
    DOWN = GLFW_KEY_DOWN,
    UP = GLFW_KEY_UP,
    PAGE_UP = GLFW_KEY_PAGE_UP,
    PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
    HOME = GLFW_KEY_HOME,
    END = GLFW_KEY_END,
    CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
    SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,
    NUM_LOCK = GLFW_KEY_NUM_LOCK,
    PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
    PAUSE = GLFW_KEY_PAUSE,
    F1 = GLFW_KEY_F1,
    F2 = GLFW_KEY_F2,
    F3 = GLFW_KEY_F3,
    F4 = GLFW_KEY_F4,
    F5 = GLFW_KEY_F5,
    F6 = GLFW_KEY_F6,
    F7 = GLFW_KEY_F7,
    F8 = GLFW_KEY_F8,
    F9 = GLFW_KEY_F9,
    F10 = GLFW_KEY_F10,
    F11 = GLFW_KEY_F11,
    F12 = GLFW_KEY_F12,
    F13 = GLFW_KEY_F13,
    F14 = GLFW_KEY_F14,
    F15 = GLFW_KEY_F15,
    F16 = GLFW_KEY_F16,
    F17 = GLFW_KEY_F17,
    F18 = GLFW_KEY_F18,
    F19 = GLFW_KEY_F19,
    F20 = GLFW_KEY_F20,
    F21 = GLFW_KEY_F21,
    F22 = GLFW_KEY_F22,
    F23 = GLFW_KEY_F23,
    F24 = GLFW_KEY_F24,
    F25 = GLFW_KEY_F25,
    KP_0 = GLFW_KEY_KP_0,
    KP_1 = GLFW_KEY_KP_1,
    KP_2 = GLFW_KEY_KP_2,
    KP_3 = GLFW_KEY_KP_3,
    KP_4 = GLFW_KEY_KP_4,
    KP_5 = GLFW_KEY_KP_5,
    KP_6 = GLFW_KEY_KP_6,
    KP_7 = GLFW_KEY_KP_7,
    KP_8 = GLFW_KEY_KP_8,
    KP_9 = GLFW_KEY_KP_9,
    KP_DECIMAL = GLFW_KEY_KP_DECIMAL,
    KP_DIVIDE = GLFW_KEY_KP_DIVIDE,
    KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY,
    KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT,
    KP_ADD = GLFW_KEY_KP_ADD,
    KP_ENTER = GLFW_KEY_KP_ENTER,
    KP_EQUAL = GLFW_KEY_KP_EQUAL,
    LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
    LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL,
    LEFT_ALT = GLFW_KEY_LEFT_ALT,
    LEFT_SUPER = GLFW_KEY_LEFT_SUPER,
    RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
    RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
    RIGHT_ALT = GLFW_KEY_RIGHT_ALT,
    RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,
    MENU = GLFW_KEY_MENU,

    NOT_BOUND
};

/**
 * @brief Convert a key to a string representation.
 * 
 * @param key keyboard key or mouse button
 * @return std::string GLFW name of key
 */
constexpr inline std::string
to_string(const Key& key) {
    switch (key) {
        case Key::MOUSE_LEFT:
            return "MOUSE_LEFT";
        case Key::MOUSE_RIGHT:
            return "MOUSE_RIGHT";
        case Key::MOUSE_MIDDLE:
            return "MOUSE_MIDDLE";
        case Key::MOUSE_4:
            return "MOUSE_4";
        case Key::MOUSE_5:
            return "MOUSE_5";
        case Key::MOUSE_6:
            return "MOUSE_6";
        case Key::MOUSE_7:
            return "MOUSE_7";
        case Key::MOUSE_8:
            return "MOUSE_8";
        case Key::SPACE:
            return "SPACE";
        case Key::APOSTROPHE:
            return "APOSTROPHE";
        case Key::COMMA:
            return "COMMA";
        case Key::MINUS:
            return "MINUS";
        case Key::PERIOD:
            return "PERIOD";
        case Key::SLASH:
            return "SLASH";
        case Key::KEY_0:
            return "0";
        case Key::KEY_1:
            return "1";
        case Key::KEY_2:
            return "2";
        case Key::KEY_3:
            return "3";
        case Key::KEY_4:
            return "4";
        case Key::KEY_5:
            return "5";
        case Key::KEY_6:
            return "6";
        case Key::KEY_7:
            return "7";
        case Key::KEY_8:
            return "8";
        case Key::KEY_9:
            return "9";
        case Key::SEMICOLON:
            return "SEMICOLON";
        case Key::EQUAL:
            return "EQUAL";
        case Key::A:
            return "A";
        case Key::B:
            return "B";
        case Key::C:
            return "C";
        case Key::D:
            return "D";
        case Key::E:
            return "E";
        case Key::F:
            return "F";
        case Key::G:
            return "G";
        case Key::H:
            return "H";
        case Key::I:
            return "I";
        case Key::J:
            return "J";
        case Key::K:
            return "K";
        case Key::L:
            return "L";
        case Key::M:
            return "M";
        case Key::N:
            return "N";
        case Key::O:
            return "O";
        case Key::P:
            return "P";
        case Key::Q:
            return "Q";
        case Key::R:
            return "R";
        case Key::S:
            return "S";
        case Key::T:
            return "T";
        case Key::U:
            return "U";
        case Key::V:
            return "V";
        case Key::W:
            return "W";
        case Key::X:
            return "X";
        case Key::Y:
            return "Y";
        case Key::Z:
            return "Z";
        case Key::LEFT_BRACKET:
            return "LEFT_BRACKET";
        case Key::BACKSLASH:
            return "BACKSLASH";
        case Key::RIGHT_BRACKET:
            return "RIGHT_BRACKET";
        case Key::GRAVE_ACCENT:
            return "GRAVE_ACCENT";
        case Key::WORLD_1:
            return "WORLD_1";
        case Key::WORLD_2:
            return "WORLD_2";

        case Key::ESCAPE:
            return "ESCAPE";
        case Key::ENTER:
            return "ENTER";
        case Key::TAB:
            return "TAB";
        case Key::BACKSPACE:
            return "BACKSPACE";
        case Key::INSERT:
            return "INSERT";
        case Key::DELETE:
            return "DELETE";
        case Key::RIGHT:
            return "RIGHT";
        case Key::LEFT:
            return "LEFT";
        case Key::DOWN:
            return "DOWN";
        case Key::UP:
            return "UP";
        case Key::PAGE_UP:
            return "PAGE_UP";
        case Key::PAGE_DOWN:
            return "PAGE_DOWN";
        case Key::HOME:
            return "HOME";
        case Key::END:
            return "END";
        case Key::CAPS_LOCK:
            return "CAPS_LOCK";
        case Key::SCROLL_LOCK:
            return "SCROLL_LOCK";
        case Key::NUM_LOCK:
            return "NUM_LOCK";
        case Key::PRINT_SCREEN:
            return "PRINT_SCREEN";
        case Key::PAUSE:
            return "PAUSE";
        case Key::F1:
            return "F1";
        case Key::F2:
            return "F2";
        case Key::F3:
            return "F3";
        case Key::F4:
            return "F4";
        case Key::F5:
            return "F5";
        case Key::F6:
            return "F6";
        case Key::F7:
            return "F7";
        case Key::F8:
            return "F8";
        case Key::F9:
            return "F9";
        case Key::F10:
            return "F10";
        case Key::F11:
            return "F11";
        case Key::F12:
            return "F12";
        case Key::F13:
            return "F13";
        case Key::F14:
            return "F14";
        case Key::F15:
            return "F15";
        case Key::F16:
            return "F16";
        case Key::F17:
            return "F17";
        case Key::F18:
            return "F18";
        case Key::F19:
            return "F19";
        case Key::F20:
            return "F20";
        case Key::F21:
            return "F21";
        case Key::F22:
            return "F22";
        case Key::F23:
            return "F23";
        case Key::F24:
            return "F24";
        case Key::F25:
            return "F25";
        case Key::KP_0:
            return "KP_0";
        case Key::KP_1:
            return "KP_1";
        case Key::KP_2:
            return "KP_2";
        case Key::KP_3:
            return "KP_3";
        case Key::KP_4:
            return "KP_4";
        case Key::KP_5:
            return "KP_5";
        case Key::KP_6:
            return "KP_6";
        case Key::KP_7:
            return "KP_7";
        case Key::KP_8:
            return "KP_8";
        case Key::KP_9:
            return "KP_9";
        case Key::KP_DECIMAL:
            return "KP_DECIMAL";
        case Key::KP_DIVIDE:
            return "KP_DIVIDE";
        case Key::KP_MULTIPLY:
            return "KP_MULTIPLY";
        case Key::KP_SUBTRACT:
            return "KP_SUBTRACT";
        case Key::KP_ADD:
            return "KP_ADD";
        case Key::KP_ENTER:
            return "KP_ENTER";
        case Key::KP_EQUAL:
            return "KP_EQUAL";
        case Key::LEFT_SHIFT:
            return "LEFT_SHIFT";
        case Key::LEFT_CONTROL:
            return "LEFT_CONTROL";
        case Key::LEFT_ALT:
            return "LEFT_ALT";
        case Key::LEFT_SUPER:
            return "LEFT_SUPER";
        case Key::RIGHT_SHIFT:
            return "RIGHT_SHIFT";
        case Key::RIGHT_CONTROL:
            return "RIGHT_CONTROL";
        case Key::RIGHT_ALT:
            return "RIGHT_ALT";
        case Key::RIGHT_SUPER:
            return "RIGHT_SUPER";
        case Key::MENU:
            return "MENU";
        case Key::NOT_BOUND:
            return "NOT_BOUND";
        default:
            throw exc::not_implemented_error("Given Key is not implemented.");
    }
}

namespace scene {

enum Action : int {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    PRIMARY_INTERACT,
    SECONDARY_INTERACT,
    UP,
    DOWN,
    FAST,
    //...
    __NO_ACTION__, // not a real action, cant be red from file
};

/**
 * @brief Convert game action to string representation.
 * 
 * @param action action to be taken
 * @return std::string name of action
 */
constexpr inline std::string
to_string(const Action& action) {
    switch (action) {
        case Action::FORWARD:
            return "FORWARD";
        case Action::BACKWARD:
            return "BACKWARD";
        case Action::LEFT:
            return "LEFT";
        case Action::RIGHT:
            return "RIGHT";
        case Action::PRIMARY_INTERACT:
            return "PRIMARY_INTERACT";
        case Action::SECONDARY_INTERACT:
            return "SECONDARY_INTERACT";
        case Action::UP:
            return "UP";
        case Action::DOWN:
            return "DOWN";
        case Action::FAST:
            return "FAST";
        default:
            throw exc::not_implemented_error("Given Action is not implemented.");
    }
}

/**
 * @brief Control key mapping between inputs and actions
 * 
 * @details Literally just an array that maps keys to actions.
 */
class KeyMapping {
 private:
    std::array<Key, Action::__NO_ACTION__> array_map_;

 public:
    KeyMapping();
    KeyMapping(std::unordered_map<Action, Key> key_mapping_map);

    [[nodiscard]] Key operator[](Action action) const;
};
} // namespace scene

} // namespace gui

template <>
struct glz::meta<gui::Key> {
    using enum gui::Key;
    static constexpr auto value = enumerate(
        MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE, MOUSE_4, MOUSE_5, MOUSE_6, MOUSE_7,
        MOUSE_8, SPACE, APOSTROPHE, COMMA, MINUS, PERIOD, SLASH, KEY_0, KEY_1, KEY_2,
        KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, SEMICOLON, EQUAL, A, B, C, D,
        E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, LEFT_BRACKET,
        BACKSLASH, RIGHT_BRACKET, GRAVE_ACCENT, WORLD_1, WORLD_2, ESCAPE, ENTER, TAB,
        BACKSPACE, INSERT, DELETE, RIGHT, LEFT, DOWN, UP, PAGE_UP, PAGE_DOWN, HOME, END,
        CAPS_LOCK, SCROLL_LOCK, NUM_LOCK, PRINT_SCREEN, PAUSE, F1, F2, F3, F4, F5, F6,
        F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22,
        F23, F24, F25, KP_0, KP_1, KP_2, KP_3, KP_4, KP_5, KP_6, KP_7, KP_8, KP_9,
        KP_DECIMAL, KP_DIVIDE, KP_MULTIPLY, KP_SUBTRACT, KP_ADD, KP_ENTER, KP_EQUAL,
        LEFT_SHIFT, LEFT_CONTROL, LEFT_ALT, LEFT_SUPER, RIGHT_SHIFT, RIGHT_CONTROL,
        RIGHT_ALT, RIGHT_SUPER, MENU
    );
};

template <>
struct glz::meta<gui::scene::Action> {
    using enum gui::scene::Action;
    static constexpr auto value = enumerate(
        FORWARD, BACKWARD, LEFT, RIGHT, PRIMARY_INTERACT, SECONDARY_INTERACT, UP, DOWN, FAST
    );
};
