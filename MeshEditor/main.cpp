#include "Application/Application.h"

int main()
{    
    Application* app = Application::instance();
    std::unique_ptr<Model> model = std::make_unique<Model>();
    View* view = app->createView(Settings::title, Settings::width, Settings::height);

    view->setModel(model.get());
    app->run();

    return 0;
}
