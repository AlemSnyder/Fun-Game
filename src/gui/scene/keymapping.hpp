#pragma once

#include "exceptions.hpp"

#include <glaze/glaze.hpp>

#include <GLFW/glfw3.h>

#include <string>
#include <unordered_map>

namespace gui {

enum class Key : int{
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
    WORLD_2 = GLFW_KEY_WORLD_2
};

constexpr inline std::string to_string(const Key& key) {
    switch (key) {
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
        default:
                    throw exc::not_implemented_error("Given Key is not implemented.");
    }

}


namespace scene {

    enum class Action {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        PRIMARY_INTERACT,
        SECONDARY_INTERACT,
        //...
    };

constexpr inline std::string to_string(const Action& action) {
    switch(action) {
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
        default: 
            throw exc::not_implemented_error("Given Action is not implemented.");
    }
}

    
    class KeyMapping
    {
        private:
        std::unordered_map<Key, Action> key_mapping_map_;
        public:
        KeyMapping();
        KeyMapping(std::unordered_map<Action, Key> key_mapping_map);

        [[nodiscard]] Action operator[](Key key) const;
    };
}

} // namespace gui

template <>
struct glz::meta<gui::Key> {
    using enum gui::Key;
    static constexpr auto value = enumerate(SPACE, APOSTROPHE, COMMA, MINUS, PERIOD, SLASH, KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, SEMICOLON, EQUAL, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, LEFT_BRACKET, BACKSLASH, RIGHT_BRACKET, GRAVE_ACCENT, WORLD_1, WORLD_2);
};

