# KONSOFREN
## KonAki's Software Renderer

![License](https://img.shields.io/badge/license-MIT-blue)
![Language](https://img.shields.io/badge/language-C99-orange)

## About

This project was meant to be a single header file that I can use in my own projects if ever I needed a basic software renderer.

Although the name has the word "Renderer", it is actually closer to a rasterizer as of now.

This project was made with [c99](https://en.wikipedia.org/wiki/C99) in mind specifically. The C from the year 1999.

You can read about c99 through this [wiki page](https://en.wikipedia.org/wiki/C99), or through this [full standard text](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf).

## Usage

You don't have to build the project. Just copy and paste the [header file](konsofren.h) into your project and implement it.

Just include the header and implement it in **ONLY** one file by defining KONSOFREN_IMPLEMENTATION.
```c
#define KONSOFREN_IMPLEMENTATION
#include "konsofren.h"
```

### Examples

If you need some examples of how to use it or implement it, check out the [examples folder](examples).

One example I made during development was the file [main.c](examples/main.c) in the examples directory/folder.

You can compile the example by using my [Makefile](Makefile) using stuff like GNU make on linux and make on some BSDs yourself:
```sh
make
```
Run this in the root of the repo.

You can also test it immediately by doing:
```sh
make test
```

**This project's Makefile isn't compatible with Windows and MacOS as of now, sorry**

I used [RGFW](https://github.com/ColleagueRiley/RGFW) and [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) in the main example. RGFW for window management, and stb_image.h explains itself.

Here is a small example of a minimum setup if you don't want to read it in the code directly:
```c
#define KONSOFREN_IMPLEMENTATION
#include "konsofren.h"

/* r g b a color */
#define CLEAR_COLOR  0x000000FF // color black
#define SQUARE_COLOR 0xFFA500FF // color orange

int main(void) {
	int width = 640;
	int height = 480;

	kon_framebuffer_t *fb = kon_createFramebuffer(width, height);
	kon_clearFramebuffer(fb, CLEAR_COLOR);
	kon_fillRectangle(fb, 50, 50, 100, 100, SQUARE_COLOR);

	/* make sure to do this for good practice, please */
	kon_freeFramebuffer(fb);

	return 0;
}
```

If you notice that it displays nothing on the screen after copy and pasting this(past me did this). It's because it's just storing and modifying the data.

To actually create a window that show the contents of the framebuffer, you can use a windowing library like [RGFW](https://github.com/ColleagueRiley/RGFW), [SDL](https://www.libsdl.org), and [GLFW](https://www.glfw.org).

Or you could directly communicate with [win32api](https://learn.microsoft.com/en-us/windows/win32/api/), [X11](https://www.x.org/releases/current/doc/libX11/libX11/libX11.html) and or [Wayland](https://wayland.freedesktop.org/docs/html/) if you feel like it. Once I learn them properly, I might show an example here soon.

And if you tried loading an image, you will need something like [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) to actually get the data of the image. You could learn to get the data yourself as well if you wish.

## Credits:
Thank you [Yusei](https://x.com/ju_nava) - for the [picture](control_2x.png) used by the example.
![picture in question](control_2x.png)

## To be added:
- Probably some support for rendering text/fonts... I still need to study truetype and harfbuzz.

## License - [MIT License](LICENSE)
