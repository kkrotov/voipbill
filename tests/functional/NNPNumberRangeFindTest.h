#include "ComparisonTest.h"
#include <vector>
#include <regex>
#include <utility>
#include <string.h>
#include <cxxtest/TestSuite.h>
#include "../../src-bill/classes/BDb.h"

using namespace std;

/**
 * Класс тестирования функции getNNPDestinationsByRange.
 * Тестирование проводится через url запросы к запущенному биллеру.
 *
 */
class NNPNumberRangeFindTest : public CxxTest::TestSuite , public ComparisonTest {

    NNPNumberRangeFindTest (NNPNumberRangeFindTest&);
    NNPNumberRangeFindTest& operator = (NNPNumberRangeFindTest&);

    const static unsigned int maxIndex = 500;

    BDb db;

    string resultRange;
    vector<string> resultPrefix;
    vector<string> resultDestination;

    bool checkRange       (string &number);
    bool checkPrefix      ();
    bool checkDestination ();
    void getDestinations  (vector<string> & dests);

    void setCurlResult (const char* res, vector<string>* prefix, vector<string>* destination);

public:

    NNPNumberRangeFindTest ();

    virtual bool setResult   (unsigned int index) override;
    virtual bool checkResult (unsigned int index) override;

    virtual unsigned int getResultsNumber () const override;
    virtual const char*  getTestName      () const override;

    virtual void curlWriteCallback (void *contents_temp, size_t size) override;

    void testThis () { TS_ASSERT (this->testAllResults()); }

    ~NNPNumberRangeFindTest();
};

/**
 * Callback функция Curl.
 * Обрабатывает данные полученные из url запроса.
 * С помощью регулярных выражений находит range, prefix и destination.
 * Вызывает setCurlResults.
 *
 * @param contents_temp - Данные полученные из url запроса
 * @param size
 * @param nmemb
 * @param test - Объект NNPNumberRangeFindTest
 * @return size * nmemb
 */
void NNPNumberRangeFindTest :: curlWriteCallback(void *contents_temp, size_t size)
{
    size_t realsize = size;
    char* contents = static_cast<char*>(contents_temp);
    string contentsString = contents;

    smatch matches;
    regex regEx;

    string range;
    vector<string> prefix;
    vector<string> destination;

    regEx = ("FOUND\\|NNPNumberRange\\|BY NUM '(.*?)'\n\\|\\|\\(id: (.*?),");
    while ( regex_search (contentsString, matches, regEx) ) {
        range = matches.str(2);
        contentsString = matches.suffix();
    }

    contentsString = contents;
    regEx = "FOUND\\|NNPPrefix\\|BY NNPNumberRange '(.*?)'\n\\|\\|\\(nnp_prefix_id: (.*?),";
    while (regex_search(contentsString, matches, regEx) ) {
        prefix.push_back(matches.str(2));
        contentsString = matches.suffix();
    }

    contentsString = contents;
    regEx = "FOUND\\|NNPDestination\\|BY ID '(.*?)'\n\\|\\|\\(id: (.*?),";
    while (regex_search(contentsString, matches, regEx) ) {
        destination.push_back(matches.str(2));
        contentsString = matches.suffix();
    }

    this->setCurlResult (range.c_str(), &prefix, &destination);

    return ;
}

/**
 * Инициализирует curl и BDb.
 *
 * !!!
 */
NNPNumberRangeFindTest :: NNPNumberRangeFindTest ()
        : db ("host=127.0.0.1 dbname=nispd99_test user=postgres") {
    db.connect();
}

/**
 * Записывет результат полученный из url запроса.
 * @param index индекс результата
 * @return true если запись произведена успешно, false - если неуспешно
 *
 */
bool NNPNumberRangeFindTest :: setResult   (unsigned int index) {
    srand (index);
    CURLcode res;

    string number;
    string prefix = "7997";
    for (int i = 0; i < 7; i++)
        number.push_back ('0' + (rand()%9));
    string fullNumber = prefix + number;
    char request[256]="";

    sprintf (request, "localhost:8099/test/nnpcalc?cmd=getDestinationByNum&num=%s", fullNumber.c_str());

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, request);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);
    }
    else {
        lastError = "Curl is not initialized";
        return false;
    }

    if (res != CURLE_OK) {
        lastError = "Some curl error";
        return false;
    }

    return true;
}

/**
 * Проверяет результат.
 *
 * @param index - Индекс результата
 * @return true - проверка пройдена, false - проверка не пройдена
 */
bool NNPNumberRangeFindTest :: checkResult (unsigned int index) {

    srand(index);
    if (!this->db.connected()) {
        this->lastError = "Error. DB is not connected";
        return false;
    }
    string prefix = "7997";
    string number = "";
    for (int i = 0; i < 7; i++)
        number.push_back ('0' + (rand()%9));
    string fullNumber = prefix + number;

    bool success = checkRange (fullNumber);
    if (!success)
        return false;
    if (!resultDestination.size())
        return true;

    success = checkPrefix();
    if (!success)
        return false;
    if (!resultPrefix.size())
        return true;

    success = checkDestination();
    if (!success)
        return false;

    return true;
}

/**
 * Возвращает максимальный индекс результата.
 * @return Максимальный индекс результата.
 *
 */
unsigned int NNPNumberRangeFindTest :: getResultsNumber () const {
    return this->maxIndex;
}

/**
 * Возвращает название теста.
 * @return название теста.
 *
 */
const char*  NNPNumberRangeFindTest :: getTestName () const {
    static const char* name = "NNP Number range find test";
    return name;
}

/**
 * Функция, которую должен вызвать curl callback,
 *  чтобы сохранить результаты из url - запроса
 *
 */
void NNPNumberRangeFindTest :: setCurlResult (const char* range, vector<string>* prefix, vector<string>* destination) {

    if (!range || !prefix || !destination) {
        lastError += "\nERROR:\nsetCurlResult badcall";
        this->resultRange = "";
        this->resultPrefix.clear();
        this->resultDestination.clear();
    }
    else {
        this->resultRange  = range;
        this->resultPrefix = *prefix;
        this->resultDestination = *destination;
    }
}

/**
 * Деструктор. Отключается от БД и выключает curl
 *
 */
NNPNumberRangeFindTest :: ~NNPNumberRangeFindTest() {
    db.disconnect();
}

/**
 * Проверяет правильность resultRange по данному номеру.
 *
 * @param fullNumber - номер для проверки
 * @return true  - resultRange и данные из БД совпадают,
 *         false - не совпадают.
 *
 */
bool NNPNumberRangeFindTest :: checkRange (string & fullNumber) {

    char query[256];
    sprintf (query, "SELECT id FROM nnp.number_range WHERE full_number_from < %s AND full_number_to > %s and is_active;", fullNumber.c_str(),
             fullNumber.c_str());

    BDbResult resRange = db.query(query);

    if (resRange.size() > 0 == resultRange.size() > 0) {
        if (!resultRange.size())
            return true;
        if (!resultRange.compare(resRange.get(0, 0))) {
            return true;
        }
    }
    lastError += "Number:";
    lastError += fullNumber.c_str();
    lastError += "\nExpected range:";
    lastError += resRange.size() ? resRange.get(0,0) : "";
    lastError += "\nReceived range:";
    lastError += resultRange.c_str();
    return false;
}

/**
 * Проверяет правильность resultPrefix по resultRange.
 *
 * @return true  - resultPrefix и данные из БД совпадают,
 *         false - не совпадают.
 *
 */
bool NNPNumberRangeFindTest :: checkPrefix      () {

    char query[256];
    sprintf (query, "SELECT prefix_id FROM nnp.number_range_prefix WHERE number_range_id = %s;", resultRange.c_str());
    BDbResult resPrefix = db.query(query);

    if (resPrefix.size() != resultPrefix.size()) {
        char error[256];
        sprintf (error, "RangeId:%s\nExpected_prefix_num:%d\nReceived_prefix_num%d",
                 resultRange.c_str(), resPrefix.size(), resultPrefix.size());
        lastError = error;
        return false;
    }

    if (resPrefix.size() > 1) {
        for (int i = 0; i < resPrefix.size(); i++) {
            bool foundPrefix = false;
            for (int j = 0; j < resultPrefix.size(); j++) {
                if (!resultPrefix.at(j).compare(resPrefix.get(i, 0))) {
                    foundPrefix = true;
                    resultPrefix.erase(resultPrefix.begin() + j);
                    break;
                }
            }
            if (!foundPrefix) {
                char error[256];
                sprintf(error, "RangeId:%s\nDidnt find prefix_id:%s", resPrefix.get(i,0));
                lastError = error;
                return false;
            }
        }
    }
    else if (resPrefix.size() == 0) {
        return true;
    }
    return true;
}

/**
 * Проверяет правильность resultDestination по resultPrefix.
 *
 * @return true  - resultDestination и данные из БД совпадают,
 *         false - не совпадают.
 *
 */
bool NNPNumberRangeFindTest :: checkDestination () {

    vector<string> dests;
    getDestinations(dests);

    if (dests.size() != resultDestination.size()) {
        char error[256];
        sprintf (error, "Range id:%s\nExpected_dest_num:%d\nReceived_dest_num:%d",
                 resultRange.c_str(), dests.size(), resultDestination.size());
        lastError = error;
        return false;
    }

    if (dests.size() > 1) {
        for (int i = 0; i < dests.size(); i++) {
            bool foundDestination = false;
            for (int j = 0; j < resultDestination.size(); j++) {
                if (!resultDestination.at(j).compare(dests.at(i))) {
                    foundDestination = true;
                    resultDestination.erase(resultDestination.begin() + j);
                    break;
                }
            }
            if (!foundDestination) {
                char error[256];
                sprintf (error, "Range id:%s\nDidnt find dest id:%s", resultRange.c_str(), dests.at(i).c_str());
                lastError = error;
                return false;
            }
        }
    }
    else if (dests.size() == 0) {
        return true;
    }

    return true;
}

/**
 * Выбирает какие destination из БД подходят к resultPrefix.
 *
 * @param dests - vector строк для записи результата
 */
void NNPNumberRangeFindTest :: getDestinations  (vector<string> & dests) {

    char query[256];
    vector<pair<string,string>> all;
    for (string prefix : resultPrefix) {
        sprintf(query, "SELECT destination_id, is_addition FROM nnp.prefix_destination WHERE prefix_id = %s",
                prefix.c_str());
        BDbResult dbRes = db.query(query);
        for (int i = 0; i < dbRes.size(); i++)
            all.push_back (pair<string,string>(string(dbRes.get(i, 0)),
                                               string(dbRes.get(i, 1))));
    }

    vector<string> addition, deletion;
    for (int i = 0; i < all.size(); i++) {
        if (!strcmp(all.at(i).second.c_str(), "t"))
            addition.push_back(all.at(i).first);
        else
            deletion.push_back(all.at(i).second);
    }

    for (int i = 0; i < deletion.size(); i++) {
        for (int j = 0; j < addition.size(); j++)
            if (!addition.at(j).compare(deletion.at(i))) {
                addition.erase (addition.begin() + j);
                j--;
            }
    }

    dests = addition;
}
