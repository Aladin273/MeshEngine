#include "Application/Application.h"

int main()
{    
    Application* app = Application::instance();
    std::unique_ptr<Model> model = app->loadModel(Settings::model3);
    View* view = app->createView(Settings::title, Settings::width, Settings::height, Settings::icon);

    view->setModel(model.get());
    app->run();

    return 0;
}
