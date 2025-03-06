#include "MimeTypes.hpp"

#include <iostream>
#include <map>

// MIME 타입 목록 정의
std::map<std::string, std::string> createMimeTypes() {
    std::map<std::string, std::string> mimeTypes;
    
    // 텍스트 파일
    mimeTypes[".html"] = "text/html";
    mimeTypes[".htm"] = "text/html";
    mimeTypes[".css"] = "text/css";
    mimeTypes[".js"] = "application/javascript";
    mimeTypes[".json"] = "application/json";
    mimeTypes[".xml"] = "application/xml";
    mimeTypes[".csv"] = "text/csv";
    mimeTypes[".txt"] = "text/plain";

    // 이미지 파일
    mimeTypes[".png"] = "image/png";
    mimeTypes[".jpg"] = "image/jpeg";
    mimeTypes[".jpeg"] = "image/jpeg";
    mimeTypes[".gif"] = "image/gif";
    mimeTypes[".bmp"] = "image/bmp";
    mimeTypes[".ico"] = "image/vnd.microsoft.icon";
    mimeTypes[".svg"] = "image/svg+xml";
    mimeTypes[".webp"] = "image/webp";
    mimeTypes[".tif"] = "image/tiff";
    mimeTypes[".tiff"] = "image/tiff";

    // 오디오 파일
    mimeTypes[".mp3"] = "audio/mpeg";
    mimeTypes[".wav"] = "audio/wav";
    mimeTypes[".ogg"] = "audio/ogg";
    mimeTypes[".m4a"] = "audio/mp4";
    mimeTypes[".aac"] = "audio/aac";

    // 비디오 파일
    mimeTypes[".mp4"] = "video/mp4";
    mimeTypes[".mpeg"] = "video/mpeg";
    mimeTypes[".mpg"] = "video/mpeg";
    mimeTypes[".avi"] = "video/x-msvideo";
    mimeTypes[".mov"] = "video/quicktime";
    mimeTypes[".wmv"] = "video/x-ms-wmv";
    mimeTypes[".flv"] = "video/x-flv";
    mimeTypes[".webm"] = "video/webm";

    // 문서 파일
    mimeTypes[".pdf"] = "application/pdf";
    mimeTypes[".doc"] = "application/msword";
    mimeTypes[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    mimeTypes[".xls"] = "application/vnd.ms-excel";
    mimeTypes[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    mimeTypes[".ppt"] = "application/vnd.ms-powerpoint";
    mimeTypes[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    mimeTypes[".odt"] = "application/vnd.oasis.opendocument.text";
    mimeTypes[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    mimeTypes[".odp"] = "application/vnd.oasis.opendocument.presentation";

    // 압축 파일
    mimeTypes[".zip"] = "application/zip";
    mimeTypes[".rar"] = "application/x-rar-compressed";
    mimeTypes[".tar"] = "application/x-tar";
    mimeTypes[".gz"] = "application/gzip";
    mimeTypes[".7z"] = "application/x-7z-compressed";

    // 기타 실행 파일 및 바이너리
    mimeTypes[".exe"] = "application/octet-stream";
    mimeTypes[".bin"] = "application/octet-stream";
    mimeTypes[".dll"] = "application/octet-stream";
    mimeTypes[".iso"] = "application/x-iso9660-image";
    mimeTypes[".img"] = "application/octet-stream";

    // 글꼴 파일
    mimeTypes[".woff"] = "font/woff";
    mimeTypes[".woff2"] = "font/woff2";
    mimeTypes[".ttf"] = "font/ttf";
    mimeTypes[".otf"] = "font/otf";

    // 기타
    mimeTypes[".wasm"] = "application/wasm";
    mimeTypes[".rtf"] = "application/rtf";

    return mimeTypes;
}

std::map<std::string, std::string> mimeTypes = createMimeTypes();

std::string getContentType(const std::string& fileExtension) {
    if (mimeTypes.find(fileExtension) != mimeTypes.end()) {
        return mimeTypes[fileExtension];
    }
    return "application/octet-stream";  // 기본값 (알 수 없는 파일)
}
