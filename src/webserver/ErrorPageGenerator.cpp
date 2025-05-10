#include "ErrorPageGenerator.hpp"

std::string generateDefaultErrorPage(int errorCode) {
    std::string reason;

    switch (errorCode) {
        case 400: reason = "Bad Request"; break;
        case 401: reason = "Unauthorized"; break;
        case 403: reason = "Forbidden"; break;
        case 404: reason = "Not Found"; break;
        case 405: reason = "Method Not Allowed"; break;
        case 408: reason = "Request Timeout"; break;
        case 413: reason = "Payload Too Large"; break;
        case 500: reason = "Internal Server Error"; break;
        case 502: reason = "Bad Gateway"; break;
        case 503: reason = "Service Unavailable"; break;
        default:  reason = "Error"; break;
    }

    std::string body =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "  <meta charset=\"UTF-8\">\n"
        "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "  <title>" + std::to_string(errorCode) + " " + reason + "</title>\n"
        "  <style>\n"
        "    body { background: linear-gradient(135deg, #3b82f6 0%, #06b6d4 100%); color: #222; font-family: 'Segoe UI', 'Pretendard', Arial, sans-serif; display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; margin: 0; }\n"
        "    .container { background: #fff; padding: 48px 36px; border-radius: 18px; box-shadow: 0 6px 32px rgba(0,0,0,0.10); text-align: center; min-width: 320px; }\n"
        "    .icon { font-size: 3.5rem; margin-bottom: 18px; }\n"
        "    h1 { font-size: 2.5rem; margin-bottom: 10px; color: #3b82f6; }\n"
        "    p { font-size: 1.1rem; color: #555; margin-bottom: 28px; }\n"
        "    a { display: inline-block; padding: 10px 24px; background: #3b82f6; color: #fff; border-radius: 8px; text-decoration: none; font-weight: 500; font-size: 1rem; transition: background 0.2s; }\n"
        "    a:hover { background: #2563eb; }\n"
        "  </style>\n"
        "</head>\n"
        "<body>\n"
        "  <div class=\"container\">\n"
        "    <div class=\"icon\">🚧</div>\n"
        "    <h1>" + std::to_string(errorCode) + " " + reason + "</h1>\n"
        "    <p>죄송합니다. 서버에서 오류가 발생하여 요청을 처리할 수 없습니다.<br>잠시 후 다시 시도해 주세요.</p>\n"
        "    <a href=\"/\">메인 페이지로 이동</a>\n"
        "  </div>\n"
        "</body>\n"
        "</html>\n";

    return body;
}