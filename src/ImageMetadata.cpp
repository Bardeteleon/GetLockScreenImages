#include "ImageMetadata.h"
#include "EXIFStreamFile.h"
#include "jpegsize.h"

ImageMetadata::ImageMetadata(const std::string& path)
{
    ImageMetadata jpeg_image{TryReadAsJpeg(path)};
    ImageMetadata exif_image{TryReadAsExif(path)};

    if (exif_image.IsValid())
    {
        *this = exif_image;
    }
    else
    {
        *this = jpeg_image;
    }
}

ImageMetadata ImageMetadata::TryReadAsJpeg(const std::string& path)
{
    int image_width = 0;
    int image_height = 0;

    bool jpegsize_abort = false;
    FILE* p_file;
    fopen_s(&p_file, path.c_str(), "r");
    if (scanhead(p_file, &image_width, &image_height))
    {
        //cout << "Size: " << image_width << " - " << image_height << "\n";
    }
    else
    {
        //cout << "jpegsize: File not detected as jpeg\n";
        jpegsize_abort = true;
    }
    fclose(p_file);

    ImageMetadata::Type type = jpegsize_abort ? ImageMetadata::Type::INVALID : ImageMetadata::Type::JPEG;
    return ImageMetadata(path, type, image_height, image_width);
}

ImageMetadata ImageMetadata::TryReadAsExif(const std::string& path)
{
    int image_width = 0;
    int image_height = 0;

    bool exif_abort = false;
    EXIFStreamFile img_stream(path.c_str());
    if (!img_stream.IsValid())
    {
        //cout << "EXIF: Can not open " << source_path_string << "\n";
        exif_abort = true;
    }
    else 
    {
        TinyEXIF::EXIFInfo img_exif(img_stream);
        if (!img_exif.Fields)
        {
            //cout << "EXIF: No EXIF or XMP metadata\n";
            exif_abort = true;
        }
        else
        {
            image_width = img_exif.ImageWidth;
            image_height = img_exif.ImageHeight;
        }
    }

    ImageMetadata::Type type = exif_abort ? ImageMetadata::Type::INVALID : ImageMetadata::Type::EXIF;
    return ImageMetadata(path, type, image_height, image_width);
}
