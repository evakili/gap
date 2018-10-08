#pragma once

#include <string>

#include <odb/core.hxx>

namespace gap {
namespace server {
    namespace db {
        #pragma db object
        struct user {
        public:
            user(std::string name, std::string password)
                : name_{ name }, password_{ password }
            {}

            std::string name() const {
                return name_;
            }

            std::string password() const {
                return password_;
            }
            
            void name(std::string name) {
                name_ = name;
            }

            void password(std::string password) {
                password_ = password;
            }

            bool authenticate(std::string password) {
                return password_ == password;
            }

        private:
            friend class odb::access;
            
            user() {}

            #pragma db id
            std::string name_;
            std::string password_;
        };
    }
}
}
