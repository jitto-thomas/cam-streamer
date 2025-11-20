// v4l2_stream.cpp
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <cstring>
#include <iostream>
#include <cstdlib>

int main() {
    const char* device = "/dev/video0"; // some error due to hardware access inside ubuntu from virtual box
    int fd = open(device, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        perror("VIDIOC_QUERYCAP");
        return 1;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        std::cerr << "Device does not support capture" << std::endl;
        return 1;
    }

    v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        return 1;
    }

    std::cout << "Streaming camera. Press Ctrl+C to stop." << std::endl;

    // Allocate buffer
    const int buf_size = fmt.fmt.pix.sizeimage;
    unsigned char* buffer = new unsigned char[buf_size];

    while (true) {
        int r = read(fd, buffer, buf_size);
        if (r < 0) {
            perror("Read failed");
            break;
        }

        // use ffplay to play
        fwrite(buffer, 1, r, stdout);
    }

    delete[] buffer;
    close(fd);
    return 0;
}
