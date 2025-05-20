# FlapKat

A modern, cross-platform remake of Flappy Bird using [raylib](https://www.raylib.com/).

Play on itch.io: https://adrianmirea.itch.io/

---

## Features

- **Modern Graphics**: Smooth parallax background, animated player, and graphical pipes.
- **Responsive Controls**: Keyboard and mobile touch support.
- **Mobile & Desktop**: Runs on Windows, Linux, macOS, and the web (Emscripten).
- **Pause & Resume**: Tap the title bar on mobile to pause, tap anywhere to resume.
- **Customizable**: Easily tweak player, pipe, and background parameters.
- **High Score Tracking**: Keeps your best score between sessions.
- **Debug Tools**: Optional collision box display for development.

---

## Controls

### Desktop
- **Flap**: `Space`, `W`, or `Up Arrow`
- **Pause**: `P`
- **Exit**: `Esc`
- **Fullscreen**: `Alt+Enter`
- **Start/Restart**: `Enter`

### Mobile/Web
- **Flap**: Tap anywhere on the game area
- **Pause**: Tap the top (title bar) area
- **Resume**: Tap anywhere

---

## Building the Project

### Desktop Build (CMake)

1. Create a build directory:
    ```bash
    mkdir build
    cd build
    ```

2. Configure and build:
    ```bash
    cmake ..
    cmake --build . --config Release
    ```

The executable will be created in the `build` directory.

### Web Build (Emscripten)

To build for web platforms, simply run:
```bash
./build_web.sh
```

This will:
- Build the project using Emscripten
- Generate a web-compatible build
- Create a `web-build.zip` file ready for itch.io deployment

---

## Project Structure

- `src/`: Source code directory
- `lib/`: Library dependencies
- `Font/`: Font assets
- `Data/`: Game assets (images, sounds)
- `build/`: Desktop build output
- `web-build/`: Web build output
- `CMakeLists.txt`: CMake build configuration
- `build_web.sh`: Web build script
- `custom_shell.html`: Custom HTML shell for web builds

---

## Technical Details

- **Render to Texture**: Ensures consistent visuals and scaling across platforms.
- **Dynamic Resizing**: Handles window and orientation changes on all platforms.
- **Asset Pipeline**: Uses TTF fonts and PNG images for crisp, scalable graphics.

---

## Credits

- **Code & Design**: [Your Name or Team]
- **Art & Sound**: [Attribution for any third-party assets]
- **Powered by**: [raylib](https://www.raylib.com/)

---

## License

This project is licensed under the terms specified in the `LICENSE.txt` file.
