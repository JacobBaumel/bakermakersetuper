#include "ThreadedExtractor.h"
#include "miniz.h"
#include <memory>
#include <utility>
#include <Windows.h>
#include <filesystem>

namespace bakermaker {
    ThreadedExtractor::ThreadedExtractor(std::string outFolder) : outFolder_(std::move(outFolder)),
                                                                  finished_(false) {
        archive_ = new mz_zip_archive;
        memset(archive_, 0, sizeof(mz_zip_archive));

        HRSRC info = FindResourceA(nullptr, "portablegit", "ZIPFILE");
        HGLOBAL res = LoadResource(nullptr, info);
        auto bres = LockResource(res);
        DWORD size = SizeofResource(nullptr, info);

        mz_zip_reader_init_mem(archive_, bres, size, 0);

        UnlockResource(bres);
        FreeResource(res);

        totalFiles_ = mz_zip_reader_get_num_files(archive_);
    }

    ThreadedExtractor::~ThreadedExtractor() {
        mz_zip_reader_end(archive_);
        delete archive_;
    }

    void ThreadedExtractor::start() {
        finished_ = false;
        finishedFiles_ = 0;
        extractionThread_ = new std::thread(&ThreadedExtractor::extractor, this);
    }

    bool ThreadedExtractor::isFinished() {
        return finished_;
    }

    void ThreadedExtractor::join() {
        extractionThread_->join();
        delete extractionThread_;
    }

    unsigned int ThreadedExtractor::getFinishedFiles() {
        return finishedFiles_;
    }

    unsigned int ThreadedExtractor::getTotalFiles() {
        return totalFiles_;
    }

    void ThreadedExtractor::extractor() {
        if(!std::filesystem::directory_entry(outFolder_).exists())
            std::filesystem::create_directories(outFolder_);

        for(mz_uint i = 0; i < totalFiles_; i++) {
            char archname[128];
            mz_zip_reader_get_filename(archive_, i, archname, 128);
            char* diskname = new char[1 + outFolder_.size() + strlen(archname)];
            strcpy(diskname, outFolder_.c_str());
            strcat(diskname, archname);

            if(mz_zip_reader_is_file_a_directory(archive_, i))
                std::filesystem::create_directories(std::string(diskname));

            else
                mz_zip_reader_extract_file_to_file(archive_, archname, diskname, 0);

            finishedFiles_++;
        }

        finished_ = true;
    }
}
