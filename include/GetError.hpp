#include <map>
#include <string>

class GetError {
    public:
        struct Error {
            int code;
            std::string name;
            std::string meaning;
        };

    private:
        static std::map<int, Error> errors;

    public:
        static std::string getName(int code);

        static std::string getMeaning(int code);

        static std::string getFullMessage(int code);
};