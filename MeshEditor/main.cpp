#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

#include "Application/Application.h"

int main()
{
    auto logger = std::make_shared<spdlog::sinks::daily_file_sink_st>("Logs/MeshEditor.txt", 23, 55);
    spdlog::default_logger()->sinks().push_back(logger);

    Application* app = Application::instance();
    std::unique_ptr<Model> model = app->loadModel(Settings::dataPath + Settings::model9);
    View* view = app->createView(Settings::title, Settings::width, Settings::height);

    view->setModel(model.get());
    app->run();

    return 0;
}
