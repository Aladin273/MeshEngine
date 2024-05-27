#include "Application/Application.h"

int main()
{
    const std::string dataPath = DATA_PATH;
    const std::string shadersPath = SHADERS_PATH;

    Application* app = Application::instance();
    std::unique_ptr<Model> model = app->loadModel(dataPath + Settings::model9);
    View* view = app->createView(Settings::title, Settings::width, Settings::height);

    view->setModel(model.get());
    app->run();

    return 0;
}
