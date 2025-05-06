#ifndef MIME_TYPES_H
#define MIME_TYPES_H

#include <map>
#include <string>

// MIME 타입 목록 정의
extern std::map<std::string, std::string> mimeTypes;

// 확장자로 MIME 타입을 가져오는 함수
std::string getContentType(const std::string& fileExtension);

#endif
