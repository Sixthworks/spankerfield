#include "path.h"

namespace big
{
    void validate_path(std::filesystem::path path)
    {
        if (!std::filesystem::exists(path))
        {
            std::filesystem::create_directory(path);
        }
        else if (!std::filesystem::is_directory(path))
        {
            std::filesystem::remove(path);
            std::filesystem::create_directory(path);
        }
    }

    std::filesystem::path get_appdata_folder()
    {
        auto file_path = std::filesystem::path(std::getenv(xorstr_("appdata")));
        file_path /= xorstr_("Spankerfield");

        validate_path(file_path);

        return file_path;
    }
}