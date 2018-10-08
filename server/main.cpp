/*
* This sample is based on source code from this tutorial:
*   http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
*/

#include <iostream>
#include <thread>
#include <boost/asio.hpp>

#include "argh.h"
#include "server.h"
#include "db_authenticator.h"

#include <odb/database.hxx>
#include <odb/schema-catalog.hxx>
#include <odb/sqlite/database.hxx>
#include "db/user.h"
#include "db/user_odb.h"

int main(int argc, char *argv[]) {
    auto cmd_line = argh::parser{ argv, argh::parser::PREFER_PARAM_FOR_UNREG_OPTION };
    unsigned short portno{};
    cmd_line({ "-p", "--port" }, 9900) >> portno;
    std::string db_path{};
    cmd_line({ "-d", "--database" }, "gap.db") >> db_path;
    bool db_create = cmd_line[{ "-c", "--create-database" }];

    try {
        boost::asio::io_service io_service;

        if (db_create) {
            auto db = std::make_unique<odb::sqlite::database>(db_path, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
            odb::transaction t{ db->begin() };
            odb::schema_catalog::create_schema(*db, "", false);
            auto admin = gap::server::db::user{ "admin", "admin" };
            db->persist(admin);
            t.commit();
        }

        auto auth = gap::server::db_authenticator{ db_path };

        auto srv = gap::server::server{ io_service, portno, auth};

        std::cout << "[Server] Gap is started, listening on port " << portno << "." << std::endl;

        auto clnt_no = 0;
        while (true) {
            clnt_no++;
            std::thread{ gap::server::gap_with_client, std::move(srv.next_client()), clnt_no }.detach();
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
