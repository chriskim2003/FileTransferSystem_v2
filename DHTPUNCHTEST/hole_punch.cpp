#include "app.h"
#include <rtc/rtc.hpp>
#include <asio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>


using json = nlohmann::json;
using namespace std::chrono_literals;

// HTTP POST 요청 함수 (asio 사용)
std::string http_post(const std::string& host, const std::string& port, const std::string& target, const std::string& body) {
    using asio::ip::tcp;
    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(host, port);
    tcp::socket socket(io_context);
    asio::connect(socket, endpoints);

    std::string request =
        "POST " + target + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "Content-Type: application/json\r\n" +
        "Content-Length: " + std::to_string(body.size()) + "\r\n" +
        "Connection: close\r\n\r\n" +
        body;

    asio::write(socket, asio::buffer(request));

    asio::streambuf response_buf;
    asio::error_code ec;
    std::string response;

    while (asio::read(socket, response_buf, asio::transfer_at_least(1), ec)) {
        response.append(asio::buffers_begin(response_buf.data()), asio::buffers_end(response_buf.data()));
        response_buf.consume(response_buf.size());
    }
    if (ec != asio::error::eof)
        throw asio::system_error(ec);

    // HTTP 헤더와 바디 분리 (헤더 제거)
    auto pos = response.find("\r\n\r\n");
    if (pos != std::string::npos)
        return response.substr(pos + 4);
    return response;
}

// HTTP GET 요청 함수 (asio 사용)
std::string http_get(const std::string& host, const std::string& port, const std::string& target) {
    using asio::ip::tcp;
    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(host, port);
    tcp::socket socket(io_context);
    asio::connect(socket, endpoints);

    std::string request =
        "GET " + target + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "Connection: close\r\n\r\n";

    asio::write(socket, asio::buffer(request));

    asio::streambuf response_buf;
    asio::error_code ec;
    std::string response;

    while (asio::read(socket, response_buf, asio::transfer_at_least(1), ec)) {
        response.append(asio::buffers_begin(response_buf.data()), asio::buffers_end(response_buf.data()));
        response_buf.consume(response_buf.size());
    }
    if (ec != asio::error::eof)
        throw asio::system_error(ec);

    // HTTP 헤더와 바디 분리 (헤더 제거)
    auto pos = response.find("\r\n\r\n");
    if (pos != std::string::npos)
        return response.substr(pos + 4);
    return response;
}

rtc::Configuration init_turn() {
    rtc::Configuration config;
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");
    config.iceServers.emplace_back("stun:stun2.l.google.com:19302");
    return config;
}

std::string key;
std::FILE *file;
std::string file_path;
std::string file_name;
uint64_t file_size;
rtc::byte* file_data;

struct FileHeader {
    char file_name[64];   // 고정 길이 파일 이름 (최대 255 + null)
    uint64_t file_size;    // 파일 크기
};

// sender 역할
void sender() {
    auto pc = std::make_shared<rtc::PeerConnection>(init_turn());
    auto dc = pc->createDataChannel("data");

    dc->onOpen([&] {
        app.AppendLog("연결 성립됨!\n");

        FileHeader header = {};
        strncpy_s(header.file_name, file_name.c_str(), sizeof(header.file_name) - 1);
        header.file_size = file_size;

        dc->send(reinterpret_cast<const rtc::byte*>(&header), sizeof(FileHeader));

        const size_t CHUNK_SIZE = 256 * 1024;
        std::vector<rtc::byte> buffer(CHUNK_SIZE);

        size_t total_sent = 0;
        while (!feof(file)) {
            size_t bytes_read = fread(buffer.data(), 1, CHUNK_SIZE, file);
            if (bytes_read > 0) {
                dc->send(buffer.data(), bytes_read);
                total_sent += bytes_read;

                // optional throttling
                std::this_thread::sleep_for(5ms);
            }
        }

        fclose(file);
        dc->send("over");
    });


    dc->onMessage([](std::variant<rtc::binary, std::string> msg) {
        if (std::holds_alternative<std::string>(msg)) {
            if (std::get<std::string>(msg) == "over") {
                app.AppendLog("전송 완료!\n");
            }
        }
    });

    pc->onGatheringStateChange([&](rtc::PeerConnection::GatheringState state) {
        if (state == rtc::PeerConnection::GatheringState::Complete) {
            auto desc = pc->localDescription();
            app.AppendLog("연결 대기중...\n");
            if (desc) {
                json j;
                j["key"] = key;
                j["offer"] = std::string(*desc);
                j["candidates"] = json::array();

                // offer POST
                std::string response = http_post("54.206.48.157", "5000", "/session", j.dump());

                // answer 받기 위해 polling 시작
                while (true) {
                    app.AppendLog("응답 대기중...\n");
                    std::string ans_resp = http_get("54.206.48.157", "5000", "/session/" + key + "/answer");
                    if (ans_resp.empty()) {
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        continue;
                    }

                    json j_ans = json::parse(ans_resp);

                    if (j_ans.contains("answer") && !j_ans["answer"].get<std::string>().empty()) {
                        std::string answer_sdp = j_ans["answer"];
                        rtc::Description answer(answer_sdp, rtc::Description::Type::Answer);

                        app.AppendLog("응답 완료!\n");
                        std::cout << answer_sdp << std::endl;
                        pc->setRemoteDescription(answer);
                        break;
                    }

                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
        }
    });
    
    fopen_s(&file, file_path.c_str(), "rb");
    if (file == NULL) {
        app.AppendLog("잘못된 파일!\n");
        return;
    }
    std::fseek(file, 0, SEEK_END);
    file_size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    if (file_size < 1) {
        app.AppendLog("잘못된 파일!\n");
        fclose(file);
        return;
    }
    
    file_name = std::filesystem::path(file_path).filename().string();

    std::string tmp = "보낼 파일 : " + file_name + "\n";
    tmp += "파일 크기 : " + std::to_string(file_size) + "\n";
    app.AppendLog(tmp.c_str());
    
    pc->setLocalDescription();

    // 무한루프 (프로그램 종료 방지)
    while (true) {
        std::this_thread::sleep_for(1s);
    }
}

// receiver 역할
void receiver() {

    auto pc = std::make_shared<rtc::PeerConnection>(init_turn());

    static FileHeader header{};
    static bool header_received = false;
    static FILE* f = nullptr;
    static uint64_t bytes_received = 0;

    pc->onDataChannel([](std::shared_ptr<rtc::DataChannel> dc) {
        dc->onOpen([dc] {
            app.AppendLog("연결 성립됨!\n");
        });

        dc->onMessage([dc](const rtc::message_variant& msg) {
            if (std::holds_alternative<std::string>(msg)) {
                std::string s = std::get<std::string>(msg);
                if (s == "over") {
                    app.AppendLog("파일 수신 완료!\n");

                    if (f) {
                        fclose(f);
                        f = nullptr;
                        app.AppendLog("파일 저장 완료!\n");
                    }

                    dc->send("over");
                }
                return;
            }

            const auto* data = std::get_if<std::vector<rtc::byte>>(&msg);
            if (!data) return;

            if (!header_received) {
                if (data->size() >= sizeof(FileHeader)) {
                    // 헤더 파싱
                    std::memcpy(&header, data->data(), sizeof(FileHeader));
                    header_received = true;

                    // 파일 경로 설정
                    std::filesystem::path path = getDownloadPath();
                    path /= header.file_name;

                    fopen_s(&f, path.string().c_str(), "wb");
                    if (!f) {
                        app.AppendLog("파일 열기 실패\n");
                        return;
                    }

                    app.AppendLog(("받을 파일: " + std::string(header.file_name)).c_str());
                    app.AppendLog(("파일 크기: " + std::to_string(header.file_size)).c_str());

                    // 헤더 뒤에 데이터가 있으면 바로 저장
                    if (data->size() > sizeof(FileHeader)) {
                        size_t payload_size = data->size() - sizeof(FileHeader);
                        fwrite(data->data() + sizeof(FileHeader), 1, payload_size, f);
                        bytes_received += payload_size;
                    }
                }
                else {
                    app.AppendLog("헤더 크기 부족\n");
                }
            }
            else {
                if (f) {
                    fwrite(data->data(), 1, data->size(), f);
                    bytes_received += data->size();

                    // 진행 상황 출력
                    app.AppendLog(("진행: " + std::to_string(bytes_received) + "/" + std::to_string(header.file_size) + "\n").c_str());
                }
            }
        });
    });

    pc->onGatheringStateChange([&](rtc::PeerConnection::GatheringState state) {
        if (state == rtc::PeerConnection::GatheringState::Complete) {
            app.AppendLog("연결 대기중...\n");

            auto desc = pc->localDescription();
            if (!desc) return;

            std::string sdp = std::string(*desc);
            std::istringstream iss(sdp);
            std::ostringstream oss;
            std::string line;
            while (std::getline(iss, line)) {
                if (line.rfind("a=setup:actpass", 0) == 0) {
                    oss << "a=setup:active\n";
                }
                else {
                    oss << line << "\n";
                }
            }
            sdp = oss.str();

            json j;
            j["key"] = key;
            j["answer"] = sdp;
            j["candidates"] = json::array();

            std::string resp = http_post("54.206.48.157", "5000", "/session/" + key + "/answer", j.dump());
            app.AppendLog("응답 완료!\n");
        }
    });

    // offer SDP 받기 (HTTP GET)
    std::string offer_resp = http_get("54.206.48.157", "5000", "/session/" + key);
    if (offer_resp.empty()) {
        app.AppendLog("잘못된 키!!\n");
        return;
    }

    json j_offer = json::parse(offer_resp);
    if (!j_offer.contains("offer")) {
        app.AppendLog("잘못된 응답!!\n");
        return;
    }

    app.AppendLog("응답 대기중...\n");
    std::string offer_sdp = j_offer["offer"];

    std::cout << offer_sdp << std::endl;

    rtc::Description offer(offer_sdp, rtc::Description::Type::Offer);
    pc->setRemoteDescription(offer);

    pc->setLocalDescription();

    // 무한루프 (프로그램 종료 방지)
    while (true) {
        std::this_thread::sleep_for(1s);
    }
}

