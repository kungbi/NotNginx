#include "Router.hpp"
#include <sys/stat.h>

// Constructor: Initializes and sorts locations from the server configuration
Router::Router(const ServerConfig& serverConfig) {
    const std::vector<LocationConfig>& locations = serverConfig.getLocations();
    for (size_t i = 0; i < locations.size(); i++) {
        sortedLocations_.push_back(std::make_pair(locations[i].getPattern(), locations[i]));
    }
    sortLocations();
}

Router::~Router() {}

// Sorts locations by pattern length in descending order
void Router::sortLocations() {
    std::sort(sortedLocations_.begin(), sortedLocations_.end(), compareLocationLength);
}

// Compares the lengths of two location patterns (used for sorting)
bool Router::compareLocationLength(const LocationPair& a, const LocationPair& b) {
    return a.first.size() > b.first.size();
}

// Helper function: 패턴과 타겟이 매칭되는지 확인
bool Router::isPatternMatch(const std::string& target, const std::string& pattern) const {
    return target.find(pattern) == 0;
}

// Helper function: 상대 경로를 계산
std::string Router::calculateRelativePath(const std::string& target, const std::string& pattern) const {
    std::string relativePath = target.substr(pattern.size());
    if (!relativePath.empty() && relativePath[0] == '/') {
        relativePath = relativePath.substr(1);
    }
    return relativePath;
}

// Helper function: 파일 경로를 생성
std::string Router::buildFilePath(const LocationConfig& location, const std::string& relativePath, const std::string& fileName) const {
    std::string filePath = location.getRoot();
    if (!relativePath.empty()) {
        filePath += "/" + relativePath; // Append relative path only if it's not empty
    }
    if (!fileName.empty()) {
        filePath += "/" + fileName; // Append file name only if it's not empty
    }
    return filePath;
}

// Helper function: 매칭된 LocationConfig에서 RouteResult 생성
RouteResult Router::createRouteResult(const LocationConfig& location, const std::string& filePath, const std::string& fileExtension) const {
    if (!location.getCgiInterpreter().empty()) {
        return RouteResult(CGI_RESOURCE, filePath, location.getCgiInterpreter(), fileExtension);
    }
    return RouteResult(STATIC_RESOURCE, filePath, "", fileExtension);
}

// Helper function: LocationConfig의 index vector에서 index 파일을 가져옴
std::string Router::getIndexFile(const LocationConfig& location) const {
    if (!location.getIndex().empty()) {
        return location.getIndex()[0]; // 첫 번째 index 파일을 반환
    }
    throw std::runtime_error("Error: No index file found in location with autoindex enabled.");
}

// Helper function: 파일이 존재하는지 확인
bool Router::fileExists(const std::string& path) const {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

// Helper function: 파일 확장자를 추출
std::string Router::extractFileExtension(const std::string& fileName) const {
    size_t dotPos = fileName.rfind('.');
    if (dotPos != std::string::npos) {
        return fileName.substr(dotPos + 1);
    }
    return ""; // 확장자가 없는 경우 빈 문자열 반환
}

// 요청을 라우팅하고 RouteResult를 반환
RouteResult Router::routeRequest(const Request& request) const {
    const std::string& target = request.getTarget();
    const std::string& fileName = request.getFilename();

    for (size_t i = 0; i < sortedLocations_.size(); i++) {
        const std::string& pattern = sortedLocations_[i].first;
        const LocationConfig& location = sortedLocations_[i].second;

        if (!isPatternMatch(target, pattern)) {
            continue; // 패턴이 매칭되지 않으면 다음으로 넘어감
        }

        std::string relativePath = calculateRelativePath(target, pattern);

        // Ensure relativePath does not include fileName
        if (!fileName.empty() && relativePath.size() >= fileName.size() &&
            relativePath.substr(relativePath.size() - fileName.size()) == fileName) {
            relativePath = relativePath.substr(0, relativePath.size() - fileName.size());
            if (!relativePath.empty() && relativePath.back() == '/') {
                relativePath.pop_back(); // Remove trailing slash if present
            }
        }

        // index 파일을 확인
        std::string resolvedFileName = fileName;
        std::string fileExtension;
        if (fileName.empty()) { // 파일명이 없는 경우 index 파일 확인
            const std::vector<std::string>& indices = location.getIndex();
            for (size_t j = 0; j < indices.size(); j++) {
                std::string potentialFilePath = buildFilePath(location, relativePath, indices[j]);
                if (fileExists(potentialFilePath)) {
                    resolvedFileName = indices[j];
                    fileExtension = extractFileExtension(resolvedFileName); // 확장자 추출
                    break;
                }
            }
        } else {
            fileExtension = extractFileExtension(fileName); // 파일명이 있는 경우 확장자 추출
        }

        // 파일 경로를 생성
        std::string filePath = buildFilePath(location, relativePath, resolvedFileName);

        // resolvedFileName이 비어 있지 않으면 항상 파일 경로에 포함
        if (!resolvedFileName.empty()) {
            return createRouteResult(location, filePath, fileExtension);
        }

        // resolvedFileName이 비어 있고 파일이 없으면 디렉토리 경로만 반환
        if (!fileExists(filePath)) {
            filePath = location.getRoot() + "/" + relativePath;
        }

        return createRouteResult(location, filePath, fileExtension);
    }

    // 매칭되는 Location이 없을 경우 예외 발생
    throw std::runtime_error("Error: No matching location found for target: " + target);
}
