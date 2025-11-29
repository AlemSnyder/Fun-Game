#include "user_interface_setup.hpp"

#include "../the_buttons/bordered_window.hpp"
#include "../the_buttons/user_interface.hpp"

namespace gui {

void
setup(the_buttons::UserInterface& user_interface) {
    std::shared_ptr<the_buttons::BorderedWindow> a_window =
        std::make_shared<the_buttons::BorderedWindow>(
            std::make_shared<render::WindowTexture>()
        );

    user_interface.add(a_window);

    // window_pipeline->data.push_back(scene.a_window.get());

    return;
}

} // namespace gui
