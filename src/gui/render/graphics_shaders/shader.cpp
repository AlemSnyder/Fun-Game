#include "shader.hpp"

#include "../../../logging.hpp"
#include "gui_render_types.hpp"
#include "opengl_program.hpp"

#include <GL/glew.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace gui {

namespace shader {

std::optional<std::string>
File::get_file_content() {
    std::ifstream shader_stream(file_, std::ios::in);
    if (shader_stream.is_open()) {
        std::stringstream sstr;
        sstr << shader_stream.rdbuf();
        std::string shader_code = sstr.str();
        shader_stream.close();
        status_ = FileStatus::OK;
        return shader_code;
    } else {
        LOG_ERROR(
            logging::opengl_logger, "Cannot open {}. Is this the right directory?",
            file_.string()
        );
        status_ = FileStatus::FILE_NOT_FOUND;
        return {};
    }
}

Program::~Program() {
    for (OpenGLProgramExecuter* executer : program_executors_) {
        executer->no_program();
    }
}

std::string
get_shader_string(GLuint gl_shader_type) {
    switch (gl_shader_type) {
        case GL_VERTEX_SHADER:
            return "vertex";
        case GL_FRAGMENT_SHADER:
            return "fragment";
        case GL_GEOMETRY_SHADER:
            return "geometry";

        default:
            return "NOT A VALID SHADER TYPE";
    }
}

void
Shader::reload() {
    if (shader_ID_ != 0) {
        glDeleteShader(shader_ID_);
    }

    std::vector<std::string> source_string;
    std::string shader_type_string = get_shader_string(shader_type_);

    // Create the shader
    shader_ID_ = glCreateShader(shader_type_);

    for (File file : files_) {
        std::optional<std::string> file_read = file.get_file_content();

        if (file.get_status() != FileStatus::OK) {
            status_ = ShaderStatus::INVALID_FILE;
            return;
        }

        if (!file_read) {
            status_ = ShaderStatus::INVALID_FILE;
            return;
        }

        source_string.push_back(std::move(file_read.value()));
    }

    std::vector<const char*> source_char;

    for (size_t i = 0; i < source_string.size(); i++) {
        source_char.push_back(source_string[i].c_str());
    }

    GLint Result = GL_FALSE;
    int info_log_length;

    // Compile Vertex Shader
    LOG_BACKTRACE(logging::opengl_logger, "Compiling {} shader.", shader_type_string);
    glShaderSource(shader_ID_, 1, source_char.data(), nullptr);
    glCompileShader(shader_ID_);

    // Check Vertex Shader
    glGetShaderiv(shader_ID_, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shader_ID_, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::string shader_error_message(info_log_length + 1, '\0');
        glGetShaderInfoLog(
            shader_ID_, info_log_length, nullptr, shader_error_message.data()
        );

        LOG_ERROR(
            logging::opengl_logger, "{} shader error: {}", shader_type_string,
            shader_error_message
        );
        status_ = ShaderStatus::COMPILATION_ERROR;
        return;
    }

    status_ = ShaderStatus::OK;
}

void
Program::reload() {
    if (vertex_shader_.get_status() != ShaderStatus::OK) {
        // Not ok reload
        vertex_shader_.reload();
        // second fail
        if (vertex_shader_.get_status() != ShaderStatus::OK) {
            LOG_ERROR(logging::opengl_logger, "Some error occurred.");
            status_ = ProgramStatus::INVALID_SHADER;
            return;
        }
    }
    if (fragment_shader_.get_status() != ShaderStatus::OK) {
        // Not ok reload
        fragment_shader_.reload();
        // second fail
        if (fragment_shader_.get_status() != ShaderStatus::OK) {
            LOG_ERROR(logging::opengl_logger, "Some error occurred.");
            status_ = ProgramStatus::INVALID_SHADER;
            return;
        }
    }

    GLint vertex_shader_id = vertex_shader_.get_shader_ID();
    GLint fragment_shader_id = fragment_shader_.get_shader_ID();

    GLint Result = GL_FALSE;
    int info_log_length;

    // Link the program
    // LOG_BACKTRACE(logging::opengl_logger, "Linking shader program");

    program_ID_ = glCreateProgram();
    glAttachShader(program_ID_, vertex_shader_id);
    glAttachShader(program_ID_, fragment_shader_id);
    glLinkProgram(program_ID_);

    // Check the program
    glGetProgramiv(program_ID_, GL_LINK_STATUS, &Result);
    glGetProgramiv(program_ID_, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::string program_error_message(info_log_length + 1, '\0');
        glGetProgramInfoLog(
            program_ID_, info_log_length, nullptr, program_error_message.data()
        );

        LOG_ERROR(
            logging::opengl_logger, "Shader program linking error: {}",
            program_error_message
        );
        status_ = ProgramStatus::LINKING_FAILED;
        goto detach_shader;
    }

    LOG_INFO(
        logging::opengl_logger, "Shader compiled successfully with program ID {}",
        program_ID_
    );

    status_ = ProgramStatus::OK;

detach_shader:

    glDetachShader(program_ID_, vertex_shader_id);
    glDetachShader(program_ID_, fragment_shader_id);
}

const std::pair<std::string, std::string>&
Program::get_status_string() const {
    static std::pair<std::string, std::string> ok_string = {
        "OK",
        "This program and its corresponding shaders have compiled successfully. If "
        "there is still some error check that Uniforms and Locations are set correctly."
    };

    static std::pair<std::string, std::string> linking_failed_string = {
        "Linking Failed",
        "There is an error when connecting different shader types together. Check that "
        "the inputs and output between shaders align."
    };

    static std::pair<std::string, std::string> invalid_shader_string = {
        "Shader Failed", "Error compiling constituent shader(s). Check the log file "
                         "for more information."
    };

    static std::pair<std::string, std::string> empty_program_string = {
        "No Program; Reload", "Program has not been loaded. Click the reload button."
    };

    static std::pair<std::string, std::string> other_string = {
        "This should not happen",
        "This is a bug that should be reported to the developers."
    };

    switch (status_) {
        case ProgramStatus::OK:
            return ok_string;
        case ProgramStatus::LINKING_FAILED:
            return linking_failed_string;
        case ProgramStatus::INVALID_SHADER:
            return invalid_shader_string;
        case ProgramStatus::EMPTY:
            return empty_program_string;
        default:
            return other_string;
    }
}

void
ShaderHandler::clear() {
    for (auto it = shaders_.begin(); it != shaders_.end(); it++) {
        GLuint shader_id = it->second.get_shader_ID();
        glDeleteShader(shader_id);
    }
    shaders_.clear();
}

// public
Shader&
ShaderHandler::get_shader(const std::vector<File> source_files, GLuint gl_shader_type) {
        ShaderData shader_data(source_files, gl_shader_type);
    auto it = shaders_.find(shader_data);
    if (it == shaders_.end()) {
        Shader shader = Shader(shader_data);
        auto inserted_iterator = shaders_.emplace(shader_data, std::move(shader));
        // auto shader = ;
        return inserted_iterator.first->second;
    } else {
        return it->second;
    }
}

// TODO(alem): what do we do on error
Program&
ShaderHandler::load_program(
    const std::vector<std::filesystem::path> vertex_file_paths,
    const std::vector<std::filesystem::path> fragment_file_paths
) {
    logging::opengl_logger->init_backtrace(4, quill::LogLevel::Error);

    std::vector<File> vertex_source_files_(
        vertex_file_paths.begin(), vertex_file_paths.end()
    );
    std::vector<File> fragment_source_files_(
        fragment_file_paths.begin(), fragment_file_paths.end()
    );

    Shader& vertex_shader = get_shader(vertex_source_files_, GL_VERTEX_SHADER);
    Shader& fragment_shader = get_shader(fragment_source_files_, GL_FRAGMENT_SHADER);

    // test if we already have the program
    Program test_program(vertex_shader, fragment_shader);
    auto it = programs_.find(test_program);

    // already have the program
    if (it != programs_.end()) {
        Program& program = it->second;
        // Program is not ok
        if (program.get_status() != ProgramStatus::OK) {
            program.reload();
            return program;
        }
        // program is ok
        return program;
    }

    // don't have the program
    auto inserted_iterator = programs_.emplace(test_program, test_program);
    return inserted_iterator.first->second;
}

} // namespace shader

} // namespace gui
