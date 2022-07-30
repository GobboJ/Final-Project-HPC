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
    static const std::unordered_map<std::string, std::tuple<std::string, std::size_t, std::size_t>> TESTS;
    
public:
    class CliArguments {

    public:
        bool isTestEnabled() const;
        void setTestEnabled(bool testEnabled);

        bool isOutputEnabled() const;
        void setOutputEnabled(bool outputEnabled);

        bool isParallel() const;
        void setParallel(bool parallel);

        [[gnu::pure]] std::size_t getParallelVersion() const;
        void setParallelVersion(std::size_t parallelVersion);

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
        std::size_t parallelVersion;
        std::filesystem::path filePath;
        std::size_t startColumnIndex;
        std::size_t endColumnIndex;
    };
    
    static CliArguments parseArguments(int argc, char *argv[]);
    
private:
    static std::size_t parseTestOption(std::size_t argc, char *argv[], std::size_t nextOptionIndex, CliArguments &result);
    static std::size_t parseOutputOption(std::size_t argc, char *argv[], std::size_t nextOptionIndex, CliArguments &result);
    static std::size_t parseParallelOption(std::size_t argc, char *argv[], std::size_t nextOptionIndex, CliArguments &result);
    static std::size_t parseFileName(std::size_t argc, char *argv[], std::size_t nextOptionIndex, CliArguments &result);
    static void usage();
    static bool parseSizeT(char *string, std::size_t &result);
};

#endif  // FINAL_PROJECT_HPC_CLIARGUMENTSPARSER_H
