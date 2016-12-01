#include "NumberRangeToPrefix.h"

void NumberRangeToPrefix :: transfer (NNPNumberRange &range, std::vector<PhoneNumber> &prefix) {

    char numberFrom[BUFFER_SIZE];
    char numberTo[BUFFER_SIZE];
    char buffer[BUFFER_SIZE]= {};
    sprintf (numberFrom, "%lld", range.full_number_from);
    sprintf (numberTo, "%lld", range.full_number_to);
    /* Collapsing ranges */

    initial (numberFrom, numberTo, buffer, 0, prefix);
    while (fixZeroing(prefix));
}
void NumberRangeToPrefix :: initial (char* from, char* to, char* buffer, int index, std::vector<PhoneNumber> &prefix) {
    if (*(to+index) == *(from+index)) {
        if ( *(to+index) == 0 ) {
            addAnyUp(buffer, buffer + index - 1, *(index + from - 1) - '0', *(index + to - 1) - '0', prefix);
            return ;
        }
        *(buffer + index) = *(from + index);
        index++;
        initial (from, to, buffer, index, prefix);
        return ;
    }
    if (*(to+index+1) == 0) {
        addAnyUp (buffer, buffer + index, *(index+from) - '0', *(index+to) - '0', prefix);
        return ;
    }
    addAnyUp (buffer, buffer + index, *(index+from)-'0' + 1, *(index+to) - '0' - 1, prefix);
    {
        handleUp(from, index, prefix);

        handleDown(to, index, prefix);

        return ;
    }
}

bool NumberRangeToPrefix :: handleUp   (char* from, int index, std::vector<PhoneNumber> &prefix) {
    bool firstHandle = true;
    int  handleIndex = index;
    while (from[handleIndex])
        handleIndex++;
    handleIndex--;
    while (from[handleIndex] == '0' && handleIndex > index)
        handleIndex--;
    if ( handleIndex == index ) {
        addAnyUp(from, from+index, *(from+index) - '0', *(from+index) - '0', prefix);
        return true;
    }
    while (handleIndex > index) {
        if (firstHandle) {
            firstHandle = false;
            addAnyUp(from, from+handleIndex, *(from+handleIndex) - '0', 9, prefix);
        }
        else
        {
            addAnyUp(from, from+handleIndex, *(from+handleIndex) - '0' + 1, 9, prefix);
        }
        handleIndex--;
    }
}

bool NumberRangeToPrefix :: handleDown   (char* from, int index, std::vector<PhoneNumber> &prefix) {
    bool firstHandle = true;
    int  handleIndex = index;
    while (from[handleIndex])
        handleIndex++;
    handleIndex--;
    while (from[handleIndex] == '9' && handleIndex > index)
        handleIndex--;
    if ( handleIndex == index ) {
        addAnyUp(from, from+index, *(from+index) - '0', *(from+index) - '0', prefix);
        return true;
    }
    while (handleIndex > index) {
        if (firstHandle) {
            firstHandle = false;
            addAnyUp(from, from+handleIndex, 0, *(from+handleIndex) - '0', prefix);
        }
        else
        {
            addAnyUp(from, from+handleIndex, 0, *(from+handleIndex) - '0' - 1, prefix);
        }
        handleIndex--;
    }
}

bool NumberRangeToPrefix :: addAnyUp   (char* buffer, char* change_pos, int num_from, int num_to, std::vector<PhoneNumber> &prefix) {
    char rememberEnd = *(change_pos + 1);
    char rememberVal = *(change_pos);
    *(change_pos + 1) = 0;
    bool zeroed = false;
    for (int i = num_from; i <= num_to; i++) {
        *change_pos = '0' + i;
        PhoneNumber input = atoi(buffer);
        prefix.push_back(input);
    }
    *(change_pos + 1) = rememberEnd;
    *(change_pos) = rememberVal;
    return zeroed;
}

void NumberRangeToPrefix :: add (std::vector<PhoneNumber> &range, PhoneNumber addition) {
    char additionC[BUFFER_SIZE] = {};
    sprintf (additionC, "%lld", addition);

    for (int i = 0; i < range.size(); i++) {
        char range_i[BUFFER_SIZE] = {};
        sprintf (range_i, "%lld", range.at(i));
        for (int j = 0;true;j++) {
            if (range_i[j] != additionC[j])
                break;
            if (range_i[j+1] == 0)
                return ; // already included
        }
    }

    range.push_back (addition);
    while (fixInclusion(range));
    while (fixZeroing(range));
}

bool NumberRangeToPrefix :: fixZeroing   (std::vector<PhoneNumber> &prefix) {
    for (int i = 0; i < prefix.size(); i++) { // Checking each prefix for zeroing
        int founds = 0;
        for (int j = i+1; j < prefix.size(); j++) {
            char prefix_i[32] = {};
            char prefix_j[32] = {};
            sprintf (prefix_i, "%lld", prefix.at(i));
            sprintf (prefix_j, "%lld", prefix.at(j));
            if (prefix_j[1] == 0)
                continue;
            for (int k = 0; true; k++) { // Algorytm of match
                if (prefix_i[k] != prefix_j[k])
                    break;
                if (prefix_i[k+2] == 0 && prefix_j[k+2] == 0) {
                    founds++;
                    break;
                }
            }
        }
        if ( founds >= 9) {
            this->add(prefix, prefix.at(i)/10);
            while (fixInclusion(prefix));
            return true;
        }
    }
    return false;
}

bool NumberRangeToPrefix :: fixInclusion (std::vector<PhoneNumber> &prefix) {
    for (int i = 0; i < prefix.size(); i++) {
        for (int j = i+1; j < prefix.size(); j++) {
            char prefix_i[32] = {};
            char prefix_j[32] = {};
            sprintf (prefix_i, "%lld", prefix.at(i));
            sprintf (prefix_j, "%lld", prefix.at(j));
            for (int k = 0; true; k++) { // Algorytm of match
                if (prefix_i[k] != prefix_j[k])
                    break;
                if (prefix_i[k+1] == 0 && prefix_j[k+1] == 0) {
                    prefix.erase(prefix.begin()+j);
                    return true;
                }
                if (prefix_i[k+1] == 0) {
                    prefix.erase(prefix.begin() + j);
                    return true;
                }
                if (prefix_j[k+1] == 0) {
                    prefix.erase(prefix.begin() + i);
                    return true;
                }
            }
        }
    }
    return false;
}

void NumberRangeToPrefix :: sub (std::vector<PhoneNumber> &prefix, PhoneNumber del) {

    for (auto i = 0; i < prefix.size(); i++) {
        char prefix_i[32] = {};
        char delC[32] = {};
        sprintf (prefix_i, "%lld", prefix.at(i));
        sprintf (delC, "%lld", del);
        for (int k = 0; true; k++) { // Algorytm of match
            if (prefix_i[k] != delC[k])
                break;
            if (prefix_i[k+1] == 0 && delC[k+1] == 0) {
                //printf ("0 0 :ERASING:%lld\n", prefix.at(i));
                prefix.erase(prefix.begin() + i);
                i--;
                break;
            }
            if (delC[k+1] == 0) {
                //printf ("delC 0 :ERASING:%lld\n", prefix.at(i));
                prefix.erase(prefix.begin() + i);
                i--;
                break;
            }
            if (prefix_i[k+1] == 0) {
               // printf ("prefix 0 : ERASING:%lld\n", prefix.at(i));
                prefix.erase(prefix.begin() + i);
                i--;
                for (int j = k+1; delC[j] != 0; j++) {
                    prefix_i[j+1] = 0;
                    for (char l = '0'; l <= '9'; l++) {
                        if (l==delC[j])
                            continue;
                        prefix_i[j] = l;
                        PhoneNumber addition = 0;
                        sscanf (prefix_i, "%lld", &addition);
                        prefix.push_back (addition);
                    }
                    prefix_i[j] = delC[j];
                }
                break;
            }
        }
    }
}

void NumberRangeToPrefix :: add (std::vector<PhoneNumber> &range, std::vector<PhoneNumber> &addition) {
    for (auto i : addition) {
        add(range, i);
    }
}
void NumberRangeToPrefix :: sub (std::vector<PhoneNumber> &range, std::vector<PhoneNumber> &del) {
    for (auto i : del) {
        sub (range, i);
    }
}

bool NumberRangeToPrefix :: checkSub (std::vector<PhoneNumber> &a, std::vector<PhoneNumber> &b,
                                      std::vector<PhoneNumber> &c) {/* a - b =c */
    if ( fixInclusion(c) ) {
        printf ("ERROR:c=(a-b) has inclusions\n");
        return false;
    }
    if ( fixZeroing(c) ) {
        printf ("ERROR:c=(a-b) has zeroing\n");
        return false;
    }
    for (auto i : b) {
        if ( isIncluded (c, i) ) {
            printf ("ERROR:c=(a-b) has %lld that is in b\n", i);
            return false;
        }
    }
    for (auto i : c) {
        if ( !isIncluded(a, i)) {
            printf ("ERROR:c=(a-b) has %lld that isn't in a", i);
            return false;
        }
    }
    for (auto i : a) {
        if ( isIncluded(c, i) ) {
            if ( isIncluded(b, i)) {
                printf ("ERROR:c=(a-b) has %lld from a that is included in a and b\n", i);
                return false;
            }
        }
        else
        {
            if ( !isIncluded(b, i)) {
//                printf ("ERROR:c=(a-b) doesn't have %lld from a that isn't in b\n", i);
//                return false;
            }
        }
    }
    return true;
}

bool NumberRangeToPrefix :: checkAdd (std::vector<PhoneNumber> &a, std::vector<PhoneNumber> &b,
                                      std::vector<PhoneNumber> &c) {/* a + b =c */
    if ( fixInclusion(c) ) {
        printf ("ERROR:sum has inclusions\n");
        return false;
    }
    if ( fixZeroing(c) ) {
        printf ("ERROR:sum has zeroing\n");
        return false;
    }
    for (auto i : a) {
        if (!isIncluded(c, i)) {
            printf ("ERROR:c=(a+b) doesn't have %lld from a\n", i);
            return false;
        }
    }
    for (auto i : b) {
        if (!isIncluded(c, i)) {
            printf ("ERROR:c=(a+b) doesn't have %lld from b\n", i);
            return false;
        }
    }
//    for (auto i : c) {
//        if ( !(isIncluded(a, i) || isIncluded(b, i)) ) {
//            printf ("ERROR:%lld from c=(a+b) isn't in a or b\n", i);
//            return false;
//        }
//    }
    return true;
}

bool NumberRangeToPrefix :: isIncluded(std::vector<PhoneNumber> &prefix, PhoneNumber check) {
    for (int i = 0; i < prefix.size(); i++) {
        char prefix_i[BUFFER_SIZE] = {};
        char checkC[BUFFER_SIZE] = {};
        sprintf (prefix_i, "%lld", prefix.at(i));
        sprintf (checkC, "%lld", check);
        for (int k = 0; true; k++) { // Algorytm of match
            if (prefix_i[k] != checkC[k])
                break;
            if (prefix_i[k+1] == 0 && checkC[k+1] == 0) {
                return true;
            }
            if (prefix_i[k+1] == 0) {
                return true;
            }
            if (checkC[k+1] == 0) {
                break;
            }
        }
    }
    return false;
}

bool NumberRangeToPrefix :: checkRangeToPrefix (std::vector<PhoneNumber> &prefix, NNPNumberRange range) {
    bool found = false;
    for (PhoneNumber i = range.full_number_from; i <= range.full_number_to; i++) { //Checking each i from range
        found = false;
        for (int j = 0; j < prefix.size(); j++) { // Looking in each prefix
            if (found)
                break;
            char prefix_j[32] = {};
            char i_j[32] = {};
            sprintf (prefix_j, "%lld", prefix.at(j));
            sprintf (i_j, "%lld", i);
            if (strlen(prefix_j) == 0 || strlen(i_j) == 0) {
                printf ("NULL ERROR\n");
                return false;
            }
            for (int k = 0; k < 32; k++) { // Algorytm of match
                if (k==30) {
                    printf ("OVERFLOW ERROR\n");
                    return false;
                }
                if (prefix_j[k] != i_j[k])
                    break;
                if (i_j[k+1] == 0 && prefix_j[k+1] == 0) {
                    found = true;
                    break;
                }
                if (prefix_j[k+1] == 0) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            printf ("Not found error:%lld\n", i);
            return false;
        }
    }
    if (fixInclusion(prefix)) {
        printf ("INCLUSION CHECK NOT PASSED:\n");
        return false;
    }
    for (int i = 0; i < prefix.size(); i++) { // Checking each prefix for nullables
        int founds = 0;
        for (int j = i+1; j < prefix.size(); j++) {
            char prefix_i[32] = {};
            char prefix_j[32] = {};
            sprintf (prefix_i, "%lld", prefix.at(i));
            sprintf (prefix_j, "%lld", prefix.at(j));
            for (int k = 0; k < 32; k++) { // Algorytm of match
                if (k==30) {
                    printf ("OVERFLOW ERROR\n");
                    return false;
                }
                if (prefix_i[k] != prefix_j[k])
                    break;
                if (prefix_i[k+2] == 0 && prefix_j[k+2] == 0) {
                    founds++;
                    break;
                }
            }
        }
        if ( founds >= 9) {
            printf ("ERROR nullable:%lld\n", prefix.at(i));
            return false;

        }
    }
    for (int i = 0; i < prefix.size(); i++) { // Checking that prefixes doesnt include more than needed
        char prefix_i[32] = {};
        char range_i[32] = {};

        sprintf (prefix_i, "%lld", prefix.at(i)); //Checking left border
        sprintf (range_i, "%lld", range.full_number_from);
        for (int j = strlen(prefix_i); j < strlen(range_i); j++) {
            prefix_i[j] = '0';
            prefix_i[j+1] = 0;
        }
        PhoneNumber checkLeft = 0;
        sscanf (prefix_i, "%lld", &checkLeft);
        if ( checkLeft < range.full_number_from) {
            printf ("ERROR: prefix %lld include more than needed\n", prefix.at(i));
            return false;
        }

        sprintf (prefix_i, "%lld", prefix.at(i)); //Checking right border
        sprintf (range_i, "%lld", range.full_number_to);
        for (int j = strlen(prefix_i); j < strlen(range_i); j++) {
            prefix_i[j] = '9';
            prefix_i[j+1] = 0;
        }
        PhoneNumber checkRight = 0;
        sscanf (prefix_i, "%lld", &checkRight);
        if ( checkRight > range.full_number_to) {
            printf ("ERROR: prefix %lld include more than needed\n", prefix.at(i));
            return false;
        }
    }
    return true;
}
