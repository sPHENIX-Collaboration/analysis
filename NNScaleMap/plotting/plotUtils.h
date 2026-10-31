#ifndef NNSCALEMAP_PLOTUTILS_H
#define NNSCALEMAP_PLOTUTILS_H

// Path helpers shared by the NNScaleMap plotting macros

#include <cctype>
#include <string>

inline std::string directoryName(const std::string &path)
{
    const std::size_t slash = path.find_last_of('/');
    return slash == std::string::npos ? "." : path.substr(0, slash);
}

inline std::string baseName(const std::string &path)
{
    const std::size_t slash = path.find_last_of('/');
    return slash == std::string::npos ? path : path.substr(slash + 1);
}

inline std::string stripExtension(const std::string &name)
{
    const std::size_t dot = name.find_last_of('.');
    return dot == std::string::npos ? name : name.substr(0, dot);
}

inline std::string safeFileStem(std::string name)
{
    for (char &c : name)
    {
        if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-' || c == '.'))
            c = '_';
    }
    return name;
}

#endif
