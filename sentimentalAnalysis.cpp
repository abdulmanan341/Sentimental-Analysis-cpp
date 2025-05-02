#include <iostream>
#include <fstream>
using namespace std;

const int wordsLength = 50;
const int sentenceLength = 500;
const int stopWords = 500;
const int maxWord = 10000;

// Function declarations
void preprocess(const char* rawdataFile, const char* stopwordsFile, char words[maxWord][wordsLength], int counts[maxWord][3], int& wordCount);
bool isStopword(const char* word, char stopwords[stopWords][wordsLength], int stopwordCount);
bool isAlpha(const char* word);
void toLowerCase(char* word);
void predictSentiment(const char* sentence, char stopwords[stopWords][wordsLength], int stopwordCount, char words[maxWord][wordsLength], int counts[maxWord][3], int wordCount, bool normalize);
void processSentence(char* line, char stopwords[stopWords][wordsLength], int stopwordCount, char words[maxWord][wordsLength], int counts[maxWord][3], int label, int& wordCount);
void readWord(char* line, int& index, char* word);
bool compareWords(const char* word1, const char* word2);
void copyWord(char* dest, const char* src);

// Function to preprocess data
void preprocess(const char* rawdataFile, const char* stopwordsFile, char words[maxWord][wordsLength], int counts[maxWord][3], int& wordCount) {
    char sentence[sentenceLength];
    char stopWords[stopWords][wordsLength];
    int stopWordCount = 0;

    // Load stopwords
    ifstream finStopWords(stopwordsFile);
    if (!finStopWords.is_open()) {
        cout << "Error: Stopwords file could not be opened." << endl;
        return;
    }
    char line[wordsLength];
    while (finStopWords.getline(line, wordsLength)) {
        toLowerCase(line);
        copyWord(stopWords[stopWordCount], line);
        stopWordCount++;
    }
    finStopWords.close();

    // Load raw data and process sentences
    ifstream finRawData(rawdataFile);
    if (!finRawData.is_open()) {
        cout << "Error: Raw data file could not be opened." << endl;
        return;
    }
    while (finRawData.getline(sentence, sentenceLength)) {
        int label = sentence[0] - '0';
        processSentence(sentence + 2, stopWords, stopWordCount, words, counts, label, wordCount);
    }
    finRawData.close();
}

// Function to process sentence
void processSentence(char* line, char stopwords[stopWords][wordsLength], int stopwordCount, char words[maxWord][wordsLength], int counts[maxWord][3], int label, int& wordCount) {
    int index = 0;
    char word[wordsLength];
    while (line[index] != '\0') {
        readWord(line, index, word);
        toLowerCase(word);
        if (isAlpha(word) && !isStopword(word, stopwords, stopwordCount)) {
            int i;
            for (i = 0; i < wordCount; i++) {
                if (compareWords(words[i], word)) {
                    break;
                }
            }
            if (i == wordCount) {
                copyWord(words[wordCount], word);
                counts[wordCount][0] = 0; // Total count
                counts[wordCount][1] = 0; // Positive count
                counts[wordCount][2] = 0; // Negative count
                wordCount++;
            }
            counts[i][0]++; // Increment total count
            if (label == 1) {
                counts[i][1]++; // Increment positive count
            }
            else {
                counts[i][2]++; // Increment negative count
            }
        }
    }
}

// Function to read words
void readWord(char* line, int& index, char* word) {
    int wordIndex = 0;
    while (line[index] != '\0' && !((line[index] >= 'A' && line[index] <= 'Z') || (line[index] >= 'a' && line[index] <= 'z'))) {
        index++;
    }
    while (line[index] != '\0' && ((line[index] >= 'A' && line[index] <= 'Z') || (line[index] >= 'a' && line[index] <= 'z'))) {
        word[wordIndex++] = line[index++];
    }
    word[wordIndex] = '\0';
}

// Function to check if a word is a stop word
bool isStopword(const char* word, char stopwords[stopWords][wordsLength], int stopwordCount) {
    for (int i = 0; i < stopwordCount; i++) {
        if (compareWords(word, stopwords[i])) {
            return true;
        }
    }
    return false;
}

// Function to check if a word contains only alphabetic characters
bool isAlpha(const char* word) {
    for (int i = 0; word[i] != '\0'; i++) {
        if (!((word[i] >= 'A' && word[i] <= 'Z') || (word[i] >= 'a' && word[i] <= 'z'))) {
            return false;
        }
    }
    return true;
}

// Function to convert a word to lower case
void toLowerCase(char* word) {
    for (int i = 0; word[i] != '\0'; i++) {
        if (word[i] >= 'A' && word[i] <= 'Z') {
            word[i] = word[i] + ('a' - 'A');
        }
    }
}

// Function to predict sentiment of a sentence
void predictSentiment(const char* sentence, char stopwords[stopWords][wordsLength], int stopwordCount, char words[maxWord][wordsLength], int counts[maxWord][3], int wordCount, bool normalize) {
    char sentenceCopy[sentenceLength];
    int sentenceIndex = 0;
    while (sentence[sentenceIndex] != '\0') {
        sentenceCopy[sentenceIndex] = sentence[sentenceIndex];
        sentenceIndex++;
    }
    sentenceCopy[sentenceIndex] = '\0';

    int index = 0;
    char word[wordsLength];
    double positiveSum = 0;
    double negativeSum = 0;

    while (sentenceCopy[index] != '\0') {
        readWord(sentenceCopy, index, word);
        toLowerCase(word);
        if (isAlpha(word) && !isStopword(word, stopwords, stopwordCount)) {
            for (int i = 0; i < wordCount; i++) {
                if (compareWords(word, words[i])) {
                    if (normalize) {
                        double total = counts[i][0];
                        if (total != 0) {
                            positiveSum += (double)counts[i][1] / total;
                            negativeSum += (double)counts[i][2] / total;
                        }
                    }
                    else {
                        positiveSum += counts[i][1];
                        negativeSum += counts[i][2];
                    }
                    break;
                }
            }
        }
    }

    if (positiveSum > negativeSum) {
        cout << "Positive" << endl;
    }
    else {
        cout << "Negative" << endl;
    }
}

// Function to compare two words
bool compareWords(const char* word1, const char* word2) {
    int i = 0;
    while (word1[i] != '\0' && word2[i] != '\0') {
        if (word1[i] != word2[i]) {
            return false;
        }
        i++;
    }
    return word1[i] == '\0' && word2[i] == '\0';
}

// Function to copy one word to another
void copyWord(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0'; // Null-terminate the destination string
}

int main() {
    const char* rawdataFile = "rawdata.txt";
    const char* stopwordsFile = "stopword.txt";
    char words[maxWord][wordsLength] = { 0 };
    int counts[maxWord][3] = { 0 };
    int wordCount = 0;

    // Preprocess the data
    preprocess(rawdataFile, stopwordsFile, words, counts, wordCount);

    // For testing, predict the sentiment of a sample sentence
    char sentence[sentenceLength];
    cout << "Enter a sentence to analyze: ";
    cin.getline(sentence, sentenceLength);

    char stopwords[stopWords][wordsLength];
    int stopwordCount = 0;

    // Load stopwords
    ifstream finStopWords(stopwordsFile);
    if (!finStopWords.is_open()) {
        cout << "Error: Stopwords file could not be opened." << endl;
        return 0;
    }

    char line[wordsLength];

    while (finStopWords.getline(line, wordsLength)) {
        toLowerCase(line);
        copyWord(stopwords[stopwordCount], line);
        stopwordCount++;
    }

    finStopWords.close();

    cout << "Prediction without normalization: ";
    predictSentiment(sentence, stopwords, stopwordCount, words, counts, wordCount, false);

    cout << "Prediction with normalization: ";
    predictSentiment(sentence, stopwords, stopwordCount, words, counts, wordCount, true);

    return 0;
}