//
// Created by Agnieszka on 11/08/2025.
//

#ifndef KARST_4_0_OUTSTREAM_H
#define KARST_4_0_OUTSTREAM_H
#include <fstream>
#include <string>
#include <iostream>

namespace karst {

    class OutStream {
        std::ofstream os;
        int nr_of_pages{0};
        const std::string name;

    public:
        explicit OutStream(const std::string &filename)
                : name(filename), nr_of_pages(0) {}

        ~OutStream() {
            if (os.is_open()) {
                os.close();}
        }


        void init() {
            os.open(name, std::ios::out);
            if (!os.is_open()) {
                throw std::runtime_error("Problem while opening the file: " + name);
            }
            nr_of_pages = 0;
        }

        OutStream(const OutStream &) = delete;

        OutStream &operator=(const OutStream &) = delete;

        void add_page() { ++nr_of_pages; }

        int get_page_count() const { return nr_of_pages; }

        const std::string &get_name() const { return name; }

        void close() {
            if (os.is_open()) {
                os.close();
            }
        }

        bool is_open() const { return os.is_open(); }

        bool good() const { return os.good(); }

        bool fail() const { return os.fail(); }

        template<typename T>
        OutStream &operator<<(const T &data) {
            os << data;
            return *this;
        }
    };
}
#endif //KARST_4_0_OUTSTREAM_H
