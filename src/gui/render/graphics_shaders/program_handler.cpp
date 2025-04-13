#include "program_handler.hpp"

#include "gui/render/gl_enums.hpp"
#include "logging.hpp"

#include <GL/glew.h>

#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace gui {

namespace shader {

std::optional<std::string>
File::get_file_content() {
    std::ifstream shader_stream(file_, std::ios::in);
    if (!shader_stream.is_open()) [[unlikely]] {
        LOG_ERROR(
            logging::opengl_logger, "Cannot open {}. Is this the right directory?",
            file_.string()
        );
        status_ = FileStatus::FILE_NOT_FOUND;
        return {};
    }
    std::stringstream sstr;
    sstr << shader_stream.rdbuf();
    std::string shader_code = sstr.str();
    status_ = FileStatus::OK;
    return shader_code;
}

void
Shader::reload() {
    if (shader_ID_ != 0) {
        glDeleteShader(shader_ID_);
    }

    std::vector<std::string> source_string;
    std::string shader_type_string = get_shader_string(shader_type_);

    // Create the shader
    shader_ID_ = glCreateShader(static_cast<GLenum>(shader_type_));

    for (File& file : files_) {
        std::optional<std::string> file_content = file.get_file_content();

        if (!file_content) [[unlikely]] {
            status_ = ShaderStatus::INVALID_FILE;
            LOG_ERROR(
                logging::file_io_logger,
                "Could not read {}. Check that the files exists.", file.get_file_path()
            );
            return;
        }

        std::string file_text = file_content.value();

        source_string.push_back(file_text);
    }

    std::vector<const char*> source_char;

    for (size_t i = 0; i < source_string.size(); i++) {
        source_char.push_back(source_string[i].c_str());
    }

    GLint result = GL_FALSE;
    int info_log_length;

    // Compile Vertex Shader
    LOG_BACKTRACE(logging::opengl_logger, "Compiling {} shader.", shader_type_string);
    glShaderSource(shader_ID_, 1, source_char.data(), nullptr);
    glCompileShader(shader_ID_);

    // Check Vertex Shader
    glGetShaderiv(shader_ID_, GL_COMPILE_STATUS, &result);
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
    uniforms_.clear();

    if (vertex_shader_.get_status() != ShaderStatus::OK) [[unlikely]] {
        // Not ok reload
        vertex_shader_.reload();
        // second fail
        if (vertex_shader_.get_status() != ShaderStatus::OK) {
            LOG_ERROR(logging::opengl_logger, "Some error occurred.");
            status_ = ProgramStatus::INVALID_SHADER;
            return;
        }
    }
    if (fragment_shader_.get_status() != ShaderStatus::OK) [[unlikely]] {
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

    program_ID_ = glCreateProgram();
    glAttachShader(program_ID_, vertex_shader_id);
    glAttachShader(program_ID_, fragment_shader_id);
    glLinkProgram(program_ID_);

    // Check the program
    glGetProgramiv(program_ID_, GL_LINK_STATUS, &Result);
    glGetProgramiv(program_ID_, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) [[unlikely]] {
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
        logging::opengl_logger, "Shader {} compiled successfully with program ID {}",
        name_, program_ID_
    );

    status_ = ProgramStatus::OK;
    attach_uniforms();

detach_shader:

    glDetachShader(program_ID_, vertex_shader_id);
    glDetachShader(program_ID_, fragment_shader_id);
}

const std::pair<std::string, std::string>&
Program::get_status_string() const {
    static std::pair<std::string, std::string> ok_string = {
        "OK",
        "This program and its corresponding shaders have compiled successfully. If "
        "there is still some error check that Uniforms and Locations are set "
        "correctly."};

    static std::pair<std::string, std::string> linking_failed_string = {
        "Linking Failed",
        "There is an error when connecting different shader types together. Check that "
        "the inputs and output between shaders align."};

    static std::pair<std::string, std::string> invalid_shader_string = {
        "Shader Failed", "Error compiling constituent shader(s). Check the log file "
                         "for more information."};

    static std::pair<std::string, std::string> empty_program_string = {
        "No Program; Reload", "Program has not been loaded. Click the reload button."};

    static std::pair<std::string, std::string> other_string = {
        "This should not happen",
        "This is a bug that should be reported to the developers."};

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
Program::attach_uniforms() {
    GLint count;
    const GLsizei buf_size = 64;
    GLchar name[buf_size];
    GLsizei length;
    GLint size;
    GLenum type;

    glGetProgramiv(program_ID_, GL_ACTIVE_UNIFORMS, &count);

    for (GLuint uid = 0; uid < static_cast<GLuint>(count); uid++) {
        glGetActiveUniform(program_ID_, uid, buf_size, &length, &size, &type, name);

        std::string str_name(name);

        if (type >= GPUDataType::NUM_TYPES) {
            LOG_WARNING(
                logging::opengl_logger,
                "Uniform type id: {} ivalid.", type
            );
        }

        gpu_data::GPUDataType enum_type =
            static_cast<gpu_data::GPUDataType>(type);
        // this probably won't fail

        LOG_INFO(
            logging::opengl_logger, "Uniform found with id: {}, name: {}, and type {}",
            uid, name, gpu_data::to_string(enum_type)
        );

        if (length > buf_size - 4) {
            LOG_WARNING(
                logging::opengl_logger,
                "Uniform name might be too long. Check that the name above is the same "
                "as the name in the source file."
            );
        }

        uniforms_.emplace(
            std::piecewise_construct, std::forward_as_tuple(name),
            std::forward_as_tuple(name, enum_type, uid)
        );
    }
}

void
Program::set_uniform(std::shared_ptr<UniformExecuter> uex, std::string uniform_name) {
    auto found_uniform = uniforms_.find(uniform_name);

    if (found_uniform != uniforms_.end()) {
        found_uniform->second.set_call_function(uex);
        auto found_type = found_uniform->second.get_type();
        auto given_type = uex->get_type();
        if (given_type != found_type) {
            LOG_WARNING(
                logging::opengl_logger,
                "Uniform types do not match. Given {} to uniform of type {}.",
                gpu_data::to_string(given_type), gpu_data::to_string(found_type)
            );
        }
    } else {
        LOG_WARNING(
            logging::opengl_logger,
            "Uniform Error. Uniform \"{}\" not found in program.", uniform_name.c_str()
        );
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
ShaderHandler::get_shader(
    const std::vector<File> source_files, gpu_data::ShaderType gl_shader_type
) {
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

Program&
ShaderHandler::load_program(
    std::string name, const std::vector<std::filesystem::path> vertex_file_paths,
    const std::vector<std::filesystem::path> fragment_file_paths
) {
    logging::opengl_logger->init_backtrace(4, quill::LogLevel::Error);

    std::vector<File> vertex_source_files_(
        vertex_file_paths.begin(), vertex_file_paths.end()
    );
    std::vector<File> fragment_source_files_(
        fragment_file_paths.begin(), fragment_file_paths.end()
    );

    Shader& vertex_shader =
        get_shader(vertex_source_files_, gpu_data::ShaderType::VERTEX_SHADER);
    Shader& fragment_shader =
        get_shader(fragment_source_files_, gpu_data::ShaderType::FRAGMENT_SHADER);

    // test if we already have the program
    ProgramData test_program(vertex_shader, fragment_shader);
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
    auto inserted_iterator =
        programs_.emplace(test_program, Program(name, test_program));
    return inserted_iterator.first->second;
}

} // namespace shader

} // namespace gui
