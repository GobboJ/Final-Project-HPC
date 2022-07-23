/*
 * Description.
 *
 * @author DeB
 * @version 1.0 2022-07-23
 * @since version date
 */
#include <fstream>
#include <stdlib.h>
#include <filesystem>

int main() {
    
    std::filesystem::path outputDirectory{".."};
    outputDirectory = outputDirectory / ".." / "test" / "resources" / "generated.data";
    
    std::ofstream file{outputDirectory};
    srand(time(nullptr));
    for (int i = 0; i < 100'000; i++) {
        for (int j = 0; j < 32; j++) {
            file << rand() % 100;
            if (j != 31) {
                file << ",";
            }
        }
        file << '\n';
    }
}
