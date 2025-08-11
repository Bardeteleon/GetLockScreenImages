
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

    std::string GetPath() const { return path_; }
    Type GetType() const { return type_; }
    std::uint32_t GetHeight() const { return height_; }
    std::uint32_t GetWidth() const { return width_; }

    bool IsLandscape() const { return width_ > height_; }
    bool IsValid() const { return type_ != Type::INVALID; }

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
