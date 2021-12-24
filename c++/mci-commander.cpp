#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <mmsystem.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <filesystem>
#include <vector>
#include <direct.h>
#pragma comment(lib, "Winmm.lib")
#define BUFFER_SIZE 1024
#define ERROR_BUFFER_SIZE BUFFER_SIZE


using namespace std;


static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}


static inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}


bool startswith(string full_str, string to_check) {
    return full_str.rfind(to_check.c_str(), 0) == 0;
}


string get_current_dir() {
    char current_dir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, current_dir);
    return string(current_dir);
}


vector<string> split(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}


void replace_all(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}


void process_input(const char* command_char) {
    locale loc;
    string command = string(command_char);
    string test_command = string(command);
    trim(command);
    trim(test_command);
    if (test_command.size() <= 0) {
        return;
    }
    for (auto& c : test_command)
    {
        c = tolower(c);
    }
    if (startswith(test_command, "quit") || startswith(test_command, "exit")) {
        exit(0);
        return;
    }
    if (startswith(test_command, "dir")) {
        for (const auto& entry : filesystem::directory_iterator(get_current_dir()))
        {
            if (!filesystem::is_directory(entry.path())) {
                continue;
            }
            string string_path = entry.path().string();
            replace_all(string_path, "/", "\\");
            vector<string> elem = split(string_path, "\\");
            cout << elem.at(elem.size() - 1) << endl;
        }
        for (const auto& entry : filesystem::directory_iterator(get_current_dir()))
        {
            if (filesystem::is_directory(entry.path())) {
                continue;
            }
            string string_path = entry.path().string();
            replace_all(string_path, "/", "\\");
            vector<string> elem = split(string_path, "\\");
            cout << elem.at(elem.size() - 1) << endl;
        }
        return;
    }
    if (startswith(test_command, "chdir")) {
        replace_all(command, "\"", "");
        replace_all(command, "\'", "");
        vector<string> to_join = split(command, " ");
        if (to_join.size() <= 0) {
            return;
        }
        string full_path;
        for (int i = 1; i < to_join.size(); i++) {
            if (i > 1) {
                full_path += ' ';
            }
            full_path += to_join.at(i);
        }
        int result = _chdir(full_path.c_str());
        if (result) {
            cout << "Error 0x" << hex << result;
            cout << ": " << endl;
            cout << "Failed To Change Folder" << endl;
        }
        return;
    }
    SetConsoleTitleA(("Pixelsuft MCI-Commander (" + command + ")").c_str());
    char result[BUFFER_SIZE];
    MCIERROR error_code = mciSendStringA(command.c_str(), result, sizeof(result), NULL);
    if (error_code) {
        char error_buffer[ERROR_BUFFER_SIZE];
        mciGetErrorStringA(error_code, error_buffer, sizeof(error_buffer));
        cout << "Error 0x" << hex << error_code;
        cout << ": " << endl;
        cout << error_buffer << endl;
    }
    else {
        string result_copy = string(result);
        replace_all(result_copy, "\n", "");
        replace_all(result_copy, " ", "");
        if (result_copy.size() <= 0) {
            return;
        }
        cout << result << endl;
    }
}


int main(int argc, char** argv)
{
    setlocale(0, "");
    for (int i = 1; i < argc; i++) {
        process_input(argv[i]);
    }
    while (true) {
        SetConsoleTitleA("Pixelsuft MCI-Commander");
        string user_input;
        cout << "MCI " << get_current_dir() << ">";
        getline(cin, user_input);
        process_input(user_input.c_str());
    }
    return 0;
}