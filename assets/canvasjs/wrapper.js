// Browser environment emulation layer (JavaScript side)
Qt.include("env.js")

// Multiple layers game engine
Qt.include("engine.js") // Base object that manages the dynamic part
Qt.include("class.js") // Allow to use classes in JS
Qt.include("widgets.js") // Basic widgets on the canvas
Qt.include("main.js")


// Global tick called by QML
function paint()
{
    if (initialized)
    {
        engine.update();
    }
}
