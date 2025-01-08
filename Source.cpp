#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
using namespace std;

struct sWord {
    char data[50];
    sWord* next;
};
struct dWord {
    char data[30];
    sWord* synonym;
    sWord* antonym;
    dWord* next;
    dWord* previous;
};
struct Dictionary {
    dWord* head;
    dWord* tail;
};

sWord* insertattail(sWord* head, char* data)
{
    sWord* newNode = new sWord;
    strcpy_s(newNode->data, sizeof(newNode->data), data);
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
    }
    else {
        sWord* temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

    return head;
}

//part 1
Dictionary* parseFile(Dictionary* d)
{
    ifstream dict("dictionary.txt");
    if (!dict) {
        cout << "Error reading file or empty file";
        return NULL;
    }

    string line;
    while (getline(dict, line)) {
        if (line.empty()) continue;

        dWord* cur = new dWord;
        cur->next = NULL;
        cur->previous = NULL;
        cur->synonym = NULL;
        cur->antonym = NULL;

        int i = 0;
        while (i < line.length() && line[i] != ':' && line[i] != '#') {
            cur->data[i] = line[i];
            i++;
        }
        cur->data[i] = '\0';

        while (i < line.length()) {
            if (line[i] == ':') {
                i++;
                sWord* s = new sWord;
                s->next = NULL;
                int j = 0;
                while (i < line.length() && line[i] != ':' && line[i] != '#' && line[i] != '\n') {
                    s->data[j] = line[i];
                    i++; j++;
                }
                s->data[j] = '\0';
                cur->synonym = insertattail(cur->synonym, s->data);
            }

            if (line[i] == '#') {
                i++;
                sWord* a = new sWord;
                a->next = NULL;
                int j = 0;
                while (i < line.length() && line[i] != '#' && line[i] != '\n') {
                    a->data[j] = line[i];
                    i++; j++;
                }
                a->data[j] = '\0';
                cur->antonym = insertattail(cur->antonym, a->data);
            }
            else {
                i++;
            }
        }

        if (d->head == NULL) {
            d->head = d->tail = cur;
        }
        else {
            d->tail->next = cur;
            cur->previous = d->tail;
            d->tail = cur;
        }
    }
    dict.close();
    return d;
}

//part 2
void writeFile(Dictionary* d)
{
    dWord* curr = d->head;
    ofstream dict("dictionary.txt");
    while (curr != NULL)
    {
        dict << curr->data;
        sWord* syn = new sWord;
        syn = curr->synonym;
        while (syn != NULL)
        {
            dict << ":" << syn->data;
            syn = syn->next;
        }
        sWord* ant = new sWord;
        ant = curr->antonym;
        while (ant != NULL)
        {
            dict << "#" << ant->data;
            ant = ant->next;
        }
        dict << "\n";
        curr = curr->next;
    }

}

//part 3
void sortDictionary(Dictionary*&d) {
    // Use bubble sort algorithm
    bool swapped;
    do {
        swapped = false;
        dWord* temp = d->head;
        while (temp->next != NULL) {
            if (strcmp(temp->data, temp->next->data) > 0) {
                dWord* prev = temp->previous;
                dWord* next = temp->next;
                dWord* nextnext = next->next;
                if (prev != NULL) {
                    prev->next = next;
                }
                else {
                    d->head = next;
                }
                next->previous = prev;
                next->next = temp;
                temp->previous = next;
                temp->next = nextnext;
                if (nextnext != NULL) {
                    nextnext->previous = temp;
                }
                else {
                    d->tail = temp;
                }
                swapped = true;
            }
            else {
                temp = temp->next;
            }
        }
    } while (swapped);
}

//part 4
Dictionary* addNewWord(Dictionary* d, char val[30])
{
    dWord* newword = new dWord;
    newword->synonym = NULL;
    newword->antonym = NULL;
    strcpy_s(newword->data, val);
    newword->next = NULL;
    newword->previous = NULL;
    int exist = 0;
    dWord* curr = d->head;
    while (curr != NULL) {
        if (strcmp(newword->data, curr->data) == 0)
        {
            exist++;
        }
        curr = curr->next;
    }
    if (exist > 0) {
        cout << "This word already exists" << endl;
        return d;
    }

    if (d->head == NULL) {
        d->head = newword;
        d->tail = newword;
    }
    else if (strcmp(newword->data, d->head->data) < 0) {
        newword->next = d->head;
        d->head->previous = newword;
        d->head = newword;
    }
    else if (strcmp(newword->data, d->tail->data) > 0) {
        newword->previous = d->tail;
        d->tail->next = newword;
        d->tail = newword;
    }
    else {
        dWord* cur2 = d->head;
        while (cur2 != NULL) {
            if (strcmp(cur2->data, newword->data) > 0) {
                newword->next = cur2;
                newword->previous = cur2->previous;
                cur2->previous->next = newword;
                cur2->previous = newword;
                break;
            }
            cur2 = cur2->next;
        }
    }
    return d;
}

//part 5
Dictionary* addsyn(Dictionary* d, char synant[30])
{
    // Add the new word to the dictionary using addNewWord
    d = addNewWord(d, synant);

    char word[30];
    cout << "Enter the word you want to add the synonym to" << endl;
    cin >> word;

    dWord* cur = d->head;
    while (cur != NULL)
    {
        if (strcmp(cur->data, word) == 0)
        {
            // Find the newly added word
            dWord* newword = d->head;
            while (newword != NULL && strcmp(newword->data, synant) != 0)
            {
                newword = newword->next;
            }

            if (newword == NULL)
            {
                cout << "The new word was not found in the dictionary." << endl;
                return d;
            }

            // Add the main word as a synonym to the new word
            sWord* mainsyn = new sWord;
            strcpy_s(mainsyn->data, cur->data);
            mainsyn->next = newword->synonym;
            newword->synonym = mainsyn;

            // Add the new word as a synonym to the main word
            sWord* newsyn = new sWord;
            strcpy_s(newsyn->data, newword->data);
            newsyn->next = cur->synonym;
            cur->synonym = newsyn;

            // Add the synonyms of the main word as synonyms to the new word
            sWord* cursyn = cur->synonym;
            while (cursyn != NULL )
            {
                if(strcmp(newword->data, cursyn->data) != 0)
                {
                sWord* syn = new sWord;
                strcpy_s(syn->data, cursyn->data);
                syn->next = newword->synonym;
                newword->synonym = syn;
                cursyn = cursyn->next;
                }
                else {
                    cursyn = cursyn->next;
                }
            }

            // Add the antonyms of the main word as antonyms to the new word
            sWord* curant = cur->antonym;
            while (curant != NULL)
            {
                sWord* ant = new sWord;
                strcpy_s(ant->data, curant->data);
                ant->next = newword->antonym;
                newword->antonym = ant;
                curant = curant->next;
            }

            return d;
        }
        cur = cur->next;
    }

    cout << "The word was not found in the dictionary." << endl;
    return d;
}

Dictionary* addant(Dictionary* d, char antant[30])
{
    // Add the new word to the dictionary using addNewWord
    d = addNewWord(d, antant);

    char word[30];
    cout << "Enter the word you want to add the antonym to" << endl;
    cin >> word;

    dWord* cur = d->head;
    while (cur != NULL)
    {
        if (strcmp(cur->data, word) == 0)
        {
            // Find the newly added word
            dWord* newword = d->head;
            while (newword != NULL && strcmp(newword->data, antant) != 0)
            {
                newword = newword->next;
            }

            if (newword == NULL)
            {
                cout << "The new word was not found in the dictionary." << endl;
                return d;
            }

            // Add the main word as an antonym to the new word
            sWord* mainant = new sWord;
            strcpy_s(mainant->data, cur->data);
            mainant->next = newword->antonym;
            newword->antonym = mainant;

            // Add the new word as an antonym to the main word
            sWord* newant = new sWord;
            strcpy_s(newant->data, newword->data);
            newant->next = cur->antonym;
            cur->antonym = newant;

            // Add the synonyms of the main word as antonyms to the new word
            sWord* cursyn = cur->synonym;
            while (cursyn != NULL)
            {
                if (strcmp(newword->data, cursyn->data) != 0)
                {
                    sWord* syn = new sWord;
                    strcpy_s(syn->data, cursyn->data);
                    syn->next = newword->antonym;
                    newword->antonym = syn;
                }
                cursyn = cursyn->next;
            }

            // Add the antonyms of the main word as synonyms to the new word
            sWord* curant = cur->antonym;
            while (curant != NULL)
            {
                if (strcmp(newword->data, curant->data) != 0)
                {
                    sWord* ant = new sWord;
                    strcpy_s(ant->data, curant->data);
                    ant->next = newword->synonym;
                    newword->synonym = ant;
                }
                curant = curant->next;
            }

            return d;
        }
        cur = cur->next;
    }

    cout << "The word was not found in the dictionary." << endl;
    return d;
}

//part 6
Dictionary* deleteWordsContaining(Dictionary* d)
{
    char dletters[30];
    cout << "Enter the letters of the word you want to delete please" << endl;
    cin >> dletters;
    dWord* cur = d->head;
    while (cur != NULL)
    {
        if (strstr(cur->data, dletters) != NULL)
        {
            if (cur == d->head)
            {
                d->head = cur->next;
                if (d->head != NULL)
                    d->head->previous = NULL;
            }

            else if (cur == d->tail)
            {
                d->tail = cur->previous;
                if (d->tail != NULL)
                    d->tail->next = NULL;
            }

            else
            {
                if (cur->previous != NULL)
                    cur->previous->next = cur->next;
                if (cur->next != NULL)
                    cur->next->previous = cur->previous;
            }
            dWord* nextNode = cur->next;
            delete cur;
            cur = nextNode;
        }
        else
        {
            sWord* dsyn = cur->synonym;
            while (dsyn != NULL && dsyn->next != NULL)
            {
                if (strstr(dsyn->next->data, dletters) != NULL)
                {
                    sWord* nextSyn = dsyn->next->next;
                    delete dsyn->next;
                    dsyn->next = nextSyn;
                }
                else
                {
                    dsyn = dsyn->next;
                }
            }

            sWord* dant = cur->antonym;
            while (dant != NULL && dant->next != NULL)
            {
                if (strstr(dant->next->data, dletters) != NULL)
                {
                    sWord* nextAnt = dant->next->next;
                    delete dant->next;
                    dant->next = nextAnt;
                }
                else
                {
                    dant = dant->next;
                }
            }
            cur = cur->next;
        }
    }
    return d;
}

//part 7
void searchletters(Dictionary* d)
{
    char word[30];
    cout << "enter the letter you want" << endl;
    cin >> word;
    dWord* cur = d->head;
    while (cur != NULL)
    {
        if (strncmp(cur->data, word, strlen(word))==0)
        {
            cout << cur->data << "\t";
        }
        sWord* cur1 = cur->synonym;
        while (cur1 != NULL)
        {
            if (strncmp(cur1->data, word, strlen(word)) == 0)
            {
                cout << cur1->data << "\t";
            }
            cur1 = cur1->next;
        }
        sWord* cur2 = cur->antonym;
        while (cur2 != NULL)
        {
            if (strncmp(cur2->data, word, strlen(word)) == 0)
            {
                cout << cur2->data << "\t";
            }
            cur2 = cur2->next;
        }
        cur = cur->next;
    }
    cout << endl;
}

//part 8
void searchsynsants(Dictionary* d)
{
    char word[30];
    cout << "enter the word you want" << endl;
    cin >> word;
    dWord *cur = d->head;
    while (cur != NULL)
    {
        if (strcmp(cur->data, word) == 0)
        {
            cout << "the word is:" << cur->data << endl;
            sWord* syn = cur->synonym;
            cout << "The synonyms are:" << endl;
            while (syn != NULL)
            {
                cout << syn->data << "  ";
                syn = syn->next;
            }
            cout << endl;
            sWord* ant = cur->antonym;
            cout << "The antonyms are:" << endl;
            while (ant != NULL)
            {
                cout << ant->data << "  ";
                ant = ant->next;
            }
            cout << endl;
            return;
        }
        cur = cur->next;
    }
}

//part 9
Dictionary* deleteWordsStartingWith(Dictionary* d)
{
    char dletters[30];
    cout << "Enter the letters of the word you want to delete please" << endl;
    cin >> dletters;
    dWord* cur = d->head;
    while (cur != NULL)
    {
        if (strncmp(cur->data, dletters, strlen(dletters)) == 0)
        {
            if (cur == d->head)
            {
                d->head = cur->next;
                if (d->head != NULL)
                    d->head->previous = NULL;
            }

            else if (cur == d->tail)
            {
                d->tail = cur->previous;
                if (d->tail != NULL)
                    d->tail->next = NULL;
            }

            else
            {
                if (cur->previous != NULL)
                    cur->previous->next = cur->next;
                if (cur->next != NULL)
                    cur->next->previous = cur->previous;
            }
            dWord* nextNode = cur->next;
            delete cur;
            cur = nextNode;
        }
        else
        {
            sWord* dsyn = cur->synonym;
            while (dsyn != NULL && dsyn->next != NULL)
            {
                if (strncmp(dsyn->next->data, dletters, strlen(dletters)) == 0)
                {
                    sWord* nextSyn = dsyn->next->next;
                    delete dsyn->next;
                    dsyn->next = nextSyn;
                }
                else
                {
                    dsyn = dsyn->next;
                }
            }

            sWord* dant = cur->antonym;
            while (dant != NULL && dant->next != NULL)
            {
                if (strncmp(dant->next->data, dletters, strlen(dletters)) == 0)
                {
                    sWord* nextAnt = dant->next->next;
                    delete dant->next;
                    dant->next = nextAnt;
                }
                else
                {
                    dant = dant->next;
                }
            }
            cur = cur->next;
        }
    }
    return d;
}

//part 10
int main() {
    Dictionary* dictionary = new Dictionary;
    dictionary->head = NULL;
    dictionary->tail = NULL;
    parseFile(dictionary);
    sortDictionary(dictionary);

    bool running = true;
    while (running) {
        cout << "Please choose an option:\n";
        cout << "1. Add a new word\n";
        cout << "2. Search letters\n";
        cout << "3. Search synonyms and antonyms\n";
        cout << "4. Delete words starting with a specific letter\n";
        cout << "5. Delete words containing a specific letter\n";
        cout << "6. Add a synonym\n";
        cout << "7. Add an antonym\n";
        cout << "8. Display the Dictionary\n";
        cout << "9. Exit\n";

        int option;
        cin >> option;

        switch (option) {
        case 1: {
            cout << "Enter a new word" << endl;
            cin.sync();
            char val[30];
            cin >> val;
            dictionary = addNewWord(dictionary, val);
            writeFile(dictionary);
            break;
        }
        case 2: {
            searchletters(dictionary);
            break;
        }
        case 3: {
            searchsynsants(dictionary);
            break;
        }
        case 4: {
            dictionary = deleteWordsStartingWith(dictionary);
            break;
        }
        case 5: {
            dictionary = deleteWordsContaining(dictionary);
            break;
        }
        case 6: {
            char synant[30];
            cout << "Enter a new synonym" << endl;
            cin >> synant;
            dictionary = addsyn(dictionary, synant);
            writeFile(dictionary);
            break;
        }
        case 7: {
            char antant[30];
            cout << "Enter a new antonym" << endl;
            cin >> antant;
            dictionary = addant(dictionary, antant);
            writeFile(dictionary);
            break;
        }
        case 8: {
            dWord* dword = dictionary->head;
            while (dword != NULL) {
                cout << "Word: " << dword->data << endl;
                cout << "Synonyms: ";
                sWord* sword = dword->synonym;
                while (sword != NULL) {
                    cout << sword->data << " ";
                    sword = sword->next;
                }
                cout << endl;
                cout << "Antonyms: ";
                sWord* aword = dword->antonym;
                while (aword != NULL) {
                    cout << aword->data << " ";
                    aword = aword->next;
                }
                cout << endl << endl;
                dword = dword->next;
            }
            break;
        }
        case 9: {
            running = false;
            break;
        }
        default: {
            cout << "Invalid option. Please try again.\n";
            break;
        }
        }
    }

     writeFile(dictionary);
    return 0;
}
