
#ifndef SRC_IMAGEMETADATA_H
#define SRC_IMAGEMETADATA_H

#include <string>

class ImageMetadata
{
  public:
    enum class Type
    {
        JPEG,
        EXIF,
        INVALID
    };

    explicit ImageMetadata(const std::string& path);

    std::string GetPath(void) { return path_; }
    Type GetType(void) { return type_; }
    std::uint32_t GetHeight(void) { return height_; }
    std::uint32_t GetWidth(void) { return width_; }

    bool IsLandscape(void) { return width_ > height_; }
    bool IsValid(void) { return type_ != Type::INVALID; }

  private:

    static ImageMetadata TryReadAsJpeg(const std::string& path);
    static ImageMetadata TryReadAsExif(const std::string& path);

    ImageMetadata(const std::string& path, Type type, std::uint32_t height, std::uint32_t width) :
        path_{path},
        type_{type},
        height_{height},
        width_{width}
    {}

    std::string path_;
    Type type_;
    std::uint32_t height_;
    std::uint32_t width_;
};

#endif