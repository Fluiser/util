#include <SFML/Graphics.hpp>
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

std::string_view powerset = " .,-+*!?qG&$#";

int W_X = 0;
int W_Y = 0;

int main(int argc, char **argv) {
  if (argc < 2) {
    return 0;
  }
  sf::Image img;
  if (!img.loadFromFile(argv[1])) {
    std::cout << "err load\n";
    return 1;
  }
  winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
    std::cout << "err get size window\n";
    return 1;
  }

  W_X = w.ws_col / 1.5;
  W_Y = w.ws_row / 1.2;

  int step_x = img.getSize().x / W_X;
  int step_y = img.getSize().y / W_Y;

  if (argc >= 4) {
    step_x = std::atoi(argv[2]);
    step_y = std::atoi(argv[3]);
  }
  bool XOR = argc > 4;

  std::cout << step_x << ";" << img.getSize().x << ";" << W_X << "\n";

  for (unsigned y = 0; y < img.getSize().y; y += step_y) {
    for (unsigned x = 0; x < img.getSize().x; x += step_x) {
      auto c = img.getPixel(sf::Vector2u{x, y});

      float power = (c.r + c.g + c.b) / 3.0f / 255.0f * (float)powerset.size();
      int idx = static_cast<int>(power);

      if (XOR)
        idx = powerset.size() - idx - 1;

      std::cout << "\x1b[48;2;" <<(int)c.r/4 << ";" << (int)c.g/4 << ";" << (int)c.b/4 <<"m" //background
        << "\x1b[38;2;" << (int)c.r << ";" << (int)c.g << ";" << (int)c.b << "m" // foreground
        << powerset[idx]
        << "\x1b[0m";
    }
    std::cout << '\n';
  }

  return 0;
}
