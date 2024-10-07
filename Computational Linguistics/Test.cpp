#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

void replace_in_string(std::string &str, std::string &old_str, std::string &new_str); //Not utilised
void remove_emojis(const char** word); //Not utilised
void update_list(std::string &curr_word, int &curr_word_length);

std::vector<std::string> list_words;
std::vector<int> list_lengths;
std::vector<int> list_frequencies;

int main() {
    //std::cout << "Here";
    std::string filename = "Catherine-Martin-Utterances.csv"; //TD-Catherine-Martin-Green2.csv, Catherine-Martin-Utterances.csv
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Error opening file " << filename << std::endl;
        return 1;
    }

    std::string line;
    std::vector< std::vector<std::string> > data;
    
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::vector<std::string> row;
        std::string cell;

        while (getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        data.push_back(row);
    }

    std::vector<std::string> text;
    for (const auto& row : data) {
        text.push_back(row[row.size()-1]);
    }
    
    int total_word_count = 0;
    int total_word_length = 0;
    int curr_word_length = 0;
    int max_word_length = 0;
    std::string longest_word = "";
    std::vector< std::vector< std::vector<std::string> > > processed_sentenes;

    //For each piece of textual content, remove emojis, #s, @s, etc.
    //Loop through each piece of textual content:
    for (int i = 0; i < text.size(); i++) {
        processed_sentenes.push_back(std::vector< std::vector<std::string> >());
        bool final_sentence = false;
        //Delimit into sentences:
        //Process until ". " or "! " or "? "
        std::string sentences = text[i];
        std::vector<std::string> delims;
        delims.push_back(". ");
        delims.push_back("! ");
        delims.push_back("? ");
        int first_delim = INT_MAX;
        int curr_delim = 0;
        int j = 0;
        while(!final_sentence) {
            processed_sentenes[i].push_back(std::vector<std::string>());
            for (int x = 0; x < delims.size(); x++) {
                curr_delim = sentences.find(delims[x]);
                if (curr_delim != std::string::npos) {
                    first_delim = std::min(first_delim, curr_delim);
                }
            }
            if (first_delim == INT_MAX) {
                final_sentence = true;
                first_delim = text[i].length()-1;
            } else {
            }
            std::string curr_sentence = sentences.substr(0, first_delim);
            char *cstr = new char[curr_sentence.length() + 1];
            strcpy(cstr, curr_sentence.c_str());
            char delim[] = " ";
            const char *token = std::strtok(cstr,delim);
            while(token) {
                //Check for # or @:
                if ((token[0] != '#') && (token[0] != '@')) {
                    //Check for http:
                    std::string curr_word = token;
                    //remove_emojis(&token);
                    if (curr_word.length() >= 4) {
                        std::string first_four = curr_word.substr(0, 4);
                        if (!(first_four == "http")) {
                            curr_word_length = strlen(token);
                            total_word_length += curr_word_length;
                            total_word_count++;
                            max_word_length = std::max(max_word_length, curr_word_length);
                            if (curr_word_length == max_word_length) {
                                longest_word = token;
                            }
                            update_list(curr_word, curr_word_length);
                            processed_sentenes[i][j].push_back(token);
                        }
                    } else {
                        curr_word_length = strlen(token);
                        total_word_length += curr_word_length;
                        total_word_count++;
                        max_word_length = std::max(max_word_length, curr_word_length);
                        if (curr_word_length == max_word_length) {
                            longest_word = token;
                        }
                        update_list(curr_word, curr_word_length);
                        processed_sentenes[i][j].push_back(token);
                    }
                }
                token = std::strtok(NULL,delim);
            }
            if (!final_sentence) {
                sentences = sentences.substr((first_delim + 1), (sentences.length() - 1));
                first_delim = INT_MAX;
            }
            j++;
        }
    }

    float avg_word_length = (float)total_word_length / (float)total_word_count;
    std::cout << "Total word length = " << total_word_length << std::endl;
    std::cout << "Total word count = " << total_word_count << std::endl;
    std::cout << "Average word length = " << avg_word_length << std::endl;
    std::cout << "Maximum word length = " << max_word_length << std::endl;
    std::cout << "Longest word = \'" << longest_word << "\'" << std::endl;
    std::string all_sentences = "";

    for (int i = 0; i < processed_sentenes.size(); i++) {
        for (int j = 0; j < processed_sentenes[i].size(); j++) {
            std::string sentence = "";
            for (int k = 0; k < processed_sentenes[i][j].size(); k++) {
                std::string word = processed_sentenes[i][j][k];
                if (k != ((processed_sentenes[i][j].size()) - 1)) {
                    sentence += word + " ";
                } else {
                    sentence += word;
                }
            }
            if (i != 0) {
                all_sentences += sentence + "\n";
            }
        }
    }

    std::ofstream MyFile("all_sentences.txt"); //all_sentences.txt, Dail_all_sentences.txt
    MyFile << all_sentences;
    MyFile.close();
    std::ofstream MyFile2("words_data.csv"); // words_data.csv, Dail_words_data.csv
    std::string first_line = "Word,Length,Frequency\n";
    MyFile2 << first_line;
    for (int i = 0; i < list_words.size(); i++) {
        std::stringstream ss;
        ss << list_lengths[i];
        std::string length;
        ss >> length;
        std::stringstream ss2;
        ss2 << list_frequencies[i];
        std::string frequency;
        ss2 >> frequency;
        std::string next_line = list_words[i] + "," + length + "," + frequency + "\n";
        MyFile2 << next_line;
    }
    MyFile2.close();
    return 0;
}

void replace_in_string(std::string &str, std::string &old_str, std::string &new_str) { //Not utilised
    for(std::size_t pos = str.find(old_str); pos != std::string::npos; pos = str.find(old_str, pos))
        {
            str.replace(pos, old_str.size(), new_str);
            pos += new_str.size();
        }
}

void remove_emojis(const char** word) { //Not utilised
    std::string new_word; //Word with emojis removed
    //*word = new_word;
}

void update_list(std::string &curr_word, int &curr_word_length) {
    bool already_exists = false;
    for (int i = 0; i < list_words.size(); i++) {
        //If curr_word in list
        if (list_words[i] == curr_word) {
            already_exists = true;
            list_frequencies[i]++;
        }
    }
    //If curr_word not in list
    if (!already_exists) {
        list_words.push_back(curr_word);
        list_lengths.push_back(curr_word_length);
        list_frequencies.push_back(1);
    }
}