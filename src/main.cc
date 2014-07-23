#include <iostream>
#include <string>
#include <map>
#include "indexer.h"
#include "search.h"
#include "hash.h"
#include "common.h"
#include "schedule.h"
#include "file_op.h"
#include "tokenizer_init.h"

// for testing
#include "barn.h"
#include "test_performance.h"
#include "file_cache.h"
#include "keyword_tree.h"

using namespace std;
using namespace bible;
typedef void(*pointer_of_function)(int, char const *[]);

void showUsage() {
    char usagestr[] = "Usage:\n"
                      "bible <add>|<search> <index directory>\n\n"
                      "make sure that <index directory> exist. bible will not create this directory.\n"
                      "<add>: index new files. the size of each file should be less than 65536 bytes.\n"
                      "<search>: search an index.\n";

    cout << usagestr << endl;
}

void actionCreateIndex(int argc, char const *argv[]) {
    if (argc < 2) {
        cout << "Usage:\n"
             "bible add <index directory>" << endl;
        return;
    }
    Schedule *indexer = new Schedule(argv[1]);
    string filename;
    indexer->Start();
    while (cin >> filename) {
        indexer->AddFile(filename.c_str());
    }
    indexer->Commit();
    delete indexer;
}

void actionMergeIndexes(int argc, char const *argv[]) {
    if (argc < 10) {
        cout << "usage: merge container1 container2 to_container keyindex1 keyindex2 to_keyindex compressed1 compressed2 to_compressed" << endl;
        return;
    }
    mergeIndex(
        argv[1], argv[2], argv[3],
        argv[4], argv[5], argv[6],
        argv[7], argv[8], argv[9]);

    cout << "merge ok." << endl;
}

void actionSearch(int argc, char const *argv[]) {
    char str[256];

    if (argc < 2) {
        cout << "Usage:\n"
             "bible search <index directory>" << endl;
        return;
    }

    cin.get(str, 256);
    cout << "start searching..." << endl;
    StopWatch watch;

    string directory = argv[1];
    auto searcher = new Schedule(directory);
    searcher->PrepareSearchers();
    auto res = searcher->Search(str);

    res->elapsetime = watch.Stop();

    for (size_t i = 0; i < res->count; ++i) {
        cout << res->filenames[i]
             << "\t->\t" << getfileoffset(res->indexes[i]) << " match(es)." << endl;
    }
    cout << "Total " << res->count << " result(s). Cost "
         << res->elapsetime << " second(s)." << endl;
    if (!res->state) {
        cout << res->msg << endl;
    }
    delete res;
    delete searcher;
    cout << "end." << endl;
}

void actionTest(int argc, char const *argv[]) {
    cout << "start testing..." << endl;

    /*string aaa = "(size|((test)&(perhaps&maybe)))";
    vector<string> *test1 = queryTokenizer(a);
      for(auto &i : *test1){
      cout << i << endl;
      }
      cout << "--------" << endl;*/

    //string aaa = "(aaaaaa)";
    /*KeywordTree *test = parseKeywordTreeStrict(aaa);

    printKeywordTree(test, 0);
    delete test;*/

    /*int port;
    std::string ipAddress;
    std::string username;
    std::string password;
    const char ConfigFile[] = "config.txt";
    Config configSettings(ConfigFile);
    int xport;

    port = configSettings.Read("port", 0);
    xport = configSettings.Read("x.port", 0);
    ipAddress = configSettings.Read("ipAddress", ipAddress);
    username = configSettings.Read("username", username);
    password = configSettings.Read("password", password);
    std::cout << "port:" << port << std::endl;
    std::cout << "ipAddress:" << ipAddress << std::endl;
    std::cout << "xport:" << xport << std::endl;
    std::cout << "username:" << username << std::endl;
    std::cout << "password:" << password << std::endl;

    auto s = globalTokenizers["english"];
    auto res = s("#include <vector>");
    for(auto a: *res){
        cout << a.hash << "\t" << a.offset << endl;
    }*/


    string test_file_name = "test_file_cache";
    fstream writer(test_file_name, ios::out | ios::binary);
    if (writer.is_open()) {
        FileCache *fc = new FileCache(1000000);
        fc->Serve(&writer, 0);
        for (size_t i = 0; i < 100; ++i) {
            fc->Write((char *)&i, sizeof(size_t));
            fc->Write(((char *)&i) + 1, sizeof(size_t) - 1);
            cout << i << ", tellp: " << fc->Tellp() << endl;
        }
        delete fc;
        writer.close();
    }

    fstream reader(test_file_name, ios::in | ios::binary);
    if (reader.is_open()) {
        FileCache *fc = new FileCache(100000);
        fc->Serve(&reader, getFileLength(test_file_name.c_str()));
        for (size_t i = 0; i < 100; ++i) {
            size_t tmp;
            fc->Read((char *)&tmp, sizeof(size_t));
            cout << i << "\t->\t" << tmp << ", tellg: " << fc->Tellg() << endl;
            fc->Read(((char *)&tmp) + 1, sizeof(size_t) - 1);
        }
        delete fc;
        reader.close();
    }


    //testInsertPerformance();


    cout << "end testing." << endl;
}

map<string, pointer_of_function> actions = {
    {"add", actionCreateIndex},
    //{"merge", actionMergeIndexes},
    {"search",  actionSearch},
    {"test",    actionTest}
};

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        showUsage();
    } else {
        string action_str = argv[1];
        if (actions.find(action_str) == actions.end()) {
            cout << "action \"" << action_str << "\" does not exist." << endl;
            showUsage();
        } else {
            // just skip the first argv.
            actions.at(action_str)(argc - 1, argv + 1);
        }
    }
    return 0;
}
