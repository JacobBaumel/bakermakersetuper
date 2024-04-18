#include "ThreadedExtractor.h"
#include "miniz.h"
#include <memory>
#include <utility>
#include "romfs/romfs.hpp"

namespace bakermaker {
    ThreadedExtractor::ThreadedExtractor(const std::string& archive,
                                         std::string outFolder,
                                         const bool romfs) :
            outFolder_(std::move(outFolder)), finished_(false) {
        archive_ = new mz_zip_archive;
        memset((void*) archive_, 0, sizeof(mz_zip_archive));

        if(romfs) {
            romfs::Resource file = romfs::get(archive);
            mz_zip_reader_init_mem(archive_, file.data(), file.size(), 0);
        }
        else mz_zip_reader_init_file(archive_, archive.c_str(), 0);

        totalFiles_ = mz_zip_reader_get_num_files(archive_);
        canAccessFinished_.unlock();
    }

    ThreadedExtractor::~ThreadedExtractor() {
        mz_zip_reader_end(archive_);
        delete archive_;
    }

    void ThreadedExtractor::start() {
        canAccessFinished_.lock();
        finished_ = false;
        finishedFiles_ = 0;
        canAccessFinished_.unlock();
        extractionThread_ = new std::thread(&ThreadedExtractor::extractor, this);
    }

    bool ThreadedExtractor::isFinished() {
        canAccessFinished_.lock();
        bool finished = finished_;
        canAccessFinished_.unlock();
        return finished;
    }

    void ThreadedExtractor::join() {
        extractionThread_->join();
        delete extractionThread_;
    }

    unsigned int ThreadedExtractor::getFinishedFiles() {
        canAccessFinished_.lock();
        unsigned int finishedFiles = finishedFiles_;
        canAccessFinished_.unlock();
        return finishedFiles;
    }

    unsigned int ThreadedExtractor::getTotalFiles() {
        return totalFiles_;
    }

    void ThreadedExtractor::extractor() {
        if(!std::filesystem::directory_entry(outFolder_).exists())
            std::filesystem::create_directories(outFolder_);

        for (mz_uint i = 0; i < totalFiles_; i++) {
            char archname[128];
            mz_zip_reader_get_filename(archive_, i, archname, 128);
            char diskname[1 + outFolder_.size() + strlen(archname)];
            strcpy(diskname, outFolder_.c_str());
            strcat(diskname, archname);

            if(mz_zip_reader_is_file_a_directory(archive_, i))
                std::filesystem::create_directories(std::string(diskname));

            else
                mz_zip_reader_extract_file_to_file(archive_, archname, diskname, 0);

            canAccessFinished_.lock();
            finishedFiles_++;
            canAccessFinished_.unlock();
        }

        canAccessFinished_.lock();
        finished_ = true;
        canAccessFinished_.unlock();
    }
}