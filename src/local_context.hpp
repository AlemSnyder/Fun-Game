// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software
 *
 * Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file local_context.hpp
 *
 * @brief Defines Local Context class
 *
 * @details Creates a local environment. Helpfull for multithreading.
 *
 * @ingroup --
 */

#pragma once

#include <angelscript.h>
#include <logging.hpp>

#include <expected>

class LocalContext {
 private:
    LocalContext();

    AngelScript::asIScriptContext* context_;

    // Return values
    // asSUCCESS                Success
    // asCONTEXT_NOT_PREPARED   The context is not in prepared state.
    // asINVALID_ARG            The arg is larger than the number of arguments in the
    // prepared function. asINVALID_TYPE           The argument is not an object or
    // handle.
    inline AngelScript::asERetCodes
    set_arg(size_t i, bool arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgByte(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, int8_t arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgByte(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, uint8_t arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgByte(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, int16_t arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgWord(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, uint16_t arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgWord(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, int32_t arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgDWord(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, uint32_t arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgDWord(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, int64_t arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgQWord(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, uint64_t arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgQWord(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, float arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgFloat(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, double arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgDouble(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, void* arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgObject(i, arg));
    }

    inline AngelScript::asERetCodes
    set_arg(size_t i, std::string& arg) {
        return static_cast<AngelScript::asERetCodes>(context_->SetArgObject(i, &arg));
    }

    template <typename A>
    inline AngelScript::asERetCodes
    set_all_args(const size_t i, const A&& a) {
        AngelScript::asERetCodes result = set_arg(i, a);
        if (result != AngelScript::asERetCodes::asSUCCESS) {
            LOG_ERROR(
                logging::main_logger, "An error occurred {} at parameter {}",
                std::to_underlying(result), i
            );
        }
        return result;
    }

    template <typename A, typename... Args>
    inline AngelScript::asERetCodes
    set_all_args(const size_t i, const A&& a, const Args&&... args) {
        AngelScript::asERetCodes result = set_arg(i, a);
        if (result != AngelScript::asERetCodes::asSUCCESS) {
            LOG_ERROR(
                logging::main_logger, "An error occurred {} at parameter {}",
                std::to_underlying(result), i
            );
            return result;
        }
        return set_all_args(i + 1, std::forward<const Args>(args)...);
    }

 public:
    [[nodiscard]] static LocalContext& instance();
    ~LocalContext();

    template <class T = void*, class... Args>
    inline std::expected<T, AngelScript::asEContextState>
    run_function(AngelScript::asIScriptFunction* function, const Args&&... args) {
        AngelScript::asEContextState result =
            static_cast<AngelScript::asEContextState>(context_->Prepare(function));
        if (result != AngelScript::asEContextState::asEXECUTION_FINISHED) {
            return std::unexpected(result);
        }
        if constexpr (sizeof...(args) != 0) {
            AngelScript::asERetCodes args_result =
                set_all_args(0, std::forward<const Args>(args)...);
            if (args_result != AngelScript::asERetCodes::asSUCCESS) {
                LOG_WARNING(
                    logging::as_logger, "Setting script arguments did not secseed."
                );
                return std::unexpected(AngelScript::asEContextState::asEXECUTION_ERROR);
            }
        }

        result = static_cast<AngelScript::asEContextState>(context_->Execute());

        if (result != AngelScript::asEContextState::asEXECUTION_FINISHED) {
            return std::unexpected(result);
        }

        if constexpr (std::is_same<T, void*>::value) {
            return nullptr;
        }

        AngelScript::asEContextState error;
        if constexpr (std::is_pointer<T>::value) {
            void* pointer = context_->GetReturnObject();
            if (pointer) {
                T object = *static_cast<T*>(pointer);
                return object;
            }
            error = AngelScript::asEContextState::asEXECUTION_ERROR;
        } else {
            T out;
            error = get_return_value(out);
            if (error == AngelScript::asEContextState::asEXECUTION_FINISHED) {
                return out;
            }
        }
        return std::unexpected(error);
    }

    // this might crash. should probably do some tests
    AngelScript::asIScriptObject*
    get_return_object() const {
        auto out = context_->GetAddressOfReturnValue();
        if (out == nullptr) {
            return nullptr;
        }
        return *(AngelScript::asIScriptObject**)out;
    }

    template <class T = void*, class... Args>
    inline std::expected<T, AngelScript::asEContextState>
    run_method(
        AngelScript::asIScriptObject* object, AngelScript::asIScriptFunction* function,
        const Args&&... args
    ) {
        AngelScript::asEContextState result =
            static_cast<AngelScript::asEContextState>(context_->Prepare(function));
        if (result != AngelScript::asEContextState::asEXECUTION_FINISHED) {
            return std::unexpected(result);
        }
        result = static_cast<AngelScript::asEContextState>(context_->SetObject(object));
        if (result != AngelScript::asEContextState::asEXECUTION_FINISHED) {
            return std::unexpected(result);
        }
        if constexpr (sizeof...(args) != 0) {
            AngelScript::asERetCodes args_result =
                set_all_args(0, std::forward<const Args>(args)...);
            if (args_result != AngelScript::asERetCodes::asSUCCESS) {
                LOG_WARNING(
                    logging::as_logger, "Setting script arguments did not secseed."
                );
                return std::unexpected(AngelScript::asEContextState::asEXECUTION_ERROR);
            }
        }

        result = static_cast<AngelScript::asEContextState>(context_->Execute());

        if (result != AngelScript::asEContextState::asEXECUTION_FINISHED) {
            return std::unexpected(result);
        }

        if constexpr (std::is_same<T, void*>::value) {
            return nullptr;
        }

        AngelScript::asEContextState error;
        if constexpr (std::is_pointer<T>::value) {
            void* pointer = context_->GetReturnObject();
            if (pointer) {
                T object_out = *static_cast<T*>(pointer);
                return object_out;
            }
            error = AngelScript::asEContextState::asEXECUTION_ERROR;
        } else {
            T out;
            error = get_return_value(out);
            if (error == AngelScript::asEContextState::asEXECUTION_FINISHED) {
                return out;
            }
        }
        return std::unexpected(error);
    }

    AngelScript::asEContextState
    get_return_value(int& value) {
        AngelScript::asEContextState state = context_->GetState();
        if (state != AngelScript::asEContextState::asEXECUTION_FINISHED) {
            return state;
        }
        value = context_->GetReturnDWord();
        return AngelScript::asEContextState::asEXECUTION_FINISHED;
    }

    AngelScript::asEContextState
    get_return_value(bool& value) {
        AngelScript::asEContextState state = context_->GetState();
        if (state != AngelScript::asEContextState::asEXECUTION_FINISHED) {
            return state;
        }
        value = context_->GetReturnByte();
        return AngelScript::asEContextState::asEXECUTION_FINISHED;
    }

    AngelScript::asEContextState
    get_return_value(float& value) {
        AngelScript::asEContextState state = context_->GetState();
        if (state != AngelScript::asEContextState::asEXECUTION_FINISHED) {
            return state;
        }
        value = context_->GetReturnFloat();
        return AngelScript::asEContextState::asEXECUTION_FINISHED;
    }

    inline int
    get_line_number(int stack_level, int* column, const char** section_name) const {
        return context_->GetLineNumber(stack_level, column, section_name);
    }
};
