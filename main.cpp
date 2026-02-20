#include <QApplication>
#include <QLabel>
#include <QImage>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    constexpr int width = 600;
    constexpr int height = 600;

    const int radius = 50;
    const int centerX = 80;
    const int centerY = 180;

    QImage image(width, height, QImage::Format_ARGB32);

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if ((x-centerX)*(x-centerX) + (y-centerY)*(y-centerY) < radius*radius) {
                image.setPixelColor(x, y, Qt::red);
            } else {
                image.setPixelColor(x, y, Qt::black);
            }
        }
    }

    QLabel label;
    label.setPixmap(QPixmap::fromImage(image));
    label.show();

    return QApplication::exec();
}