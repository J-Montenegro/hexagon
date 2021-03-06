//  Copyright 2018 Thomas Jandecka.
//  Subject to GNU GENERAL PUBLIC LICENSE Version 3.

#ifndef HEXAGON_CLIENT_CONNECTION_H_
#define HEXAGON_CLIENT_CONNECTION_H_

#include <deque>
#include <string>
#include <vector>

#include <emscripten.h>

#include <hexagon/protocol/message.hpp>

extern "C" {
EMSCRIPTEN_KEEPALIVE void* ws_alloc_buf(std::size_t n);
EMSCRIPTEN_KEEPALIVE void ws_message();
bool ws_send_js(const char*);
}

namespace hexagon::client
{
    class connection
    {
        std::string input_buffer_;
        std::string output_buffer_;
        std::deque<hexagon::protocol::server_message> messages_;

       private:
        connection();

       public:
        static void open(const std::string& uri) {}

        static connection& instance()
        {
            static connection instance_;
            return instance_;
        }

        ~connection();

       public:
        template <typename Visitor>
        void handle_all(Visitor visit)
        {
            for (const auto& msg : messages_) std::visit(visit, msg);
            messages_.clear();
        }

       public:
        template <typename Message>
        bool async_send(const Message& msg)
        {
            write_message(output_buffer_, msg);
            return ws_send_js(output_buffer_.c_str());
        }

       public:  // called internally
        void _message_buffered();
        void* _reset(std::size_t n);
    };  // namespace hexagon::client
}  // namespace hexagon::client

#endif
