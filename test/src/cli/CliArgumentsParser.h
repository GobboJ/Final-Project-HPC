#ifndef FINAL_PROJECT_HPC_CLIARGUMENTSPARSER_H
#define FINAL_PROJECT_HPC_CLIARGUMENTSPARSER_H

#include <string>
#include <unordered_map>
#include <tuple>
#include <filesystem>

/**
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-25
 * @since version date
 */
class CliArgumentsParser {

private:
    static const std::unordered_map<std::string, std::tuple<std::string, std::size_t, std::size_t>>
            TESTS;

public:
    class CliArguments {

    public:
        bool isTestEnabled() const;
        void setTestEnabled(bool testEnabled);

        bool isOutputEnabled() const;
        void setOutputEnabled(bool outputEnabled);

        bool isParallel() const;
        void setParallel(bool parallel);
        [[gnu::pure]] std::size_t getVersion() const;
        void setVersion(std::size_t version);
        [[gnu::pure]] std::size_t getDistanceComputationThreadsCount() const;
        void setDistanceComputationThreadsCount(std::size_t distanceComputationThreadsCount);
        [[gnu::pure]] std::size_t getStage4ThreadsCount() const;
        void setStage4ThreadsCount(std::size_t stage4ThreadsCount);
        [[gnu::pure]] std::size_t getStage5ThreadsCount() const;
        void setStage5ThreadsCount(std::size_t stage5ThreadsCount);
        [[gnu::pure]] const std::filesystem::path &getFilePath() const;
        void setFilePath(const std::filesystem::path &filePath);
        [[gnu::pure]] std::size_t getStartColumnIndex() const;
        void setStartColumnIndex(size_t startColumnIndex);
        [[gnu::pure]] std::size_t getEndColumnIndex() const;
        void setEndColumnIndex(std::size_t endColumnIndex);

    private:
        bool testEnabled;
        bool outputEnabled;
        bool parallel;
        std::size_t version;
        std::size_t distanceComputationThreadsCount;
        std::size_t stage4ThreadsCount;
        std::size_t stage5ThreadsCount;
        std::filesystem::path filePath;
        std::size_t startColumnIndex;
        std::size_t endColumnIndex;
    };

    static CliArguments parseArguments(int argc, char *argv[]);

private:
    static std::size_t parseTestOption(
            std::size_t argc, char *argv[], std::size_t nextOptionIndex, CliArguments &result);
    static std::size_t parseOutputOption(
            std::size_t argc, char *argv[], std::size_t nextOptionIndex, CliArguments &result);
    static std::size_t parseParallelOption(
            std::size_t argc, char *argv[], std::size_t nextOptionIndex, CliArguments &result);
    static std::size_t parseParallelThreadsCountOption(
            std::size_t argc, char *argv[], std::size_t nextOptionIndex, CliArguments &result);
    static std::size_t parseFileName(
            std::size_t argc, char *argv[], std::size_t nextOptionIndex, CliArguments &result);
    static void usage();
    static bool parseSizeT(char *string, std::size_t &result);
};

#endif  // FINAL_PROJECT_HPC_CLIARGUMENTSPARSER_H
