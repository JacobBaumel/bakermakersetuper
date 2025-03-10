#ifndef BAKERMAKERSETUPER_THREADEDEXTRACTOR_H
#define BAKERMAKERSETUPER_THREADEDEXTRACTOR_H

#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include "miniz.h"

using std::string;

namespace bakermaker {
    class ThreadedExtractor {
    public:
        ThreadedExtractor(string outFolder);
        ~ThreadedExtractor();

        void start();
        bool isFinished();
        void join();
        unsigned int getFinishedFiles();
        unsigned int getTotalFiles();

    private:
        void extractor();

        unsigned int totalFiles_;
        std::atomic_uint32_t finishedFiles_;
        mz_zip_archive* archive_;
        std::atomic_bool finished_;
        std::thread* extractionThread_;
        string outFolder_;
    };
} // namespace bakermaker

#endif //BAKERMAKERSETUPER_THREADEDEXTRACTOR_H
