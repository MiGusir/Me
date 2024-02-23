#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

enum Status{
    SUCCESS = 0,
    ERROR_OF_MEMORY = -1,
    WRONG_INPUT = -2,
    ERROR = -3
};

enum Compare_Status{
    SIMILAR = 0,
    FIRST_IS_BIGGER = 1,
    FIRST_IS_SMALLER = -1
};

typedef struct{
    char * data;
    int length;
} String, *String_ptr;

String_ptr CreateString(const char * data){
    String_ptr string = (String_ptr)malloc(sizeof(String));
    if (!string){
        return NULL;
    } 
    string->length = strlen(data);
    string->data = (char*)malloc((string->length + 1) * sizeof(char));
    if (!(string->data)) {
        free(string);
        return NULL;
    }
    strcpy(string->data, data);
    return string;
}

void DeleteString(String_ptr string){
    if (!string){
        return;
    } 

    if (string->data){
        free(string->data);
    }

    free(string);
}

int CompareStrings(const String_ptr first, const String_ptr second){
    if (first->length > second->length){
        return FIRST_IS_BIGGER;
    } 
    else if (first->length < second->length){
        return FIRST_IS_SMALLER;
    } 

    int length = first->length;
    for (int i = 0; i < length; i++){
        if ((first->data)[i] > (second->data)[i]){
            return FIRST_IS_BIGGER;
        } 
        else if ((first->data)[i] < (second->data)[i]){
            return FIRST_IS_SMALLER;
        } 
    }
    return SIMILAR;
}

int AreStringsEqual(const String_ptr first, const String_ptr second){
    if (first->length != second->length){
        return ERROR;
    } 

    int length = first->length;
    for (int i = 0; i < length; i++){
        if ((first->data)[i] != (second->data)[i]){
            return ERROR;
        } 
    }
    return SUCCESS;
}

void CopyString(const String_ptr first, String_ptr second){
    second->length = first->length;
    strcpy(second->data, first->data);
    return;
}

String_ptr Dublicate(const String_ptr string){
    String_ptr new = CreateString(string->data);
    if (!new){
        return NULL;
    } 
    return new;
}

int DynamicCopy(String_ptr new, const String_ptr string){
    if (!new){
        return ERROR;
    }
    new->length = string->length;
    new->data = (char*)malloc((new->length) * sizeof(char));
    if (!(new->data)){
        return ERROR_OF_MEMORY;
    } 
    memcpy(new->data, string->data, string->length);
    return SUCCESS;
}

String_ptr ConcatString(const String_ptr first, const String_ptr second){
    char * data = (char*)malloc(((first->length) + (second->length) + 1) * sizeof(char));
    if (!data){
        return NULL;
    } 

    memcpy(data, first->data, first->length);
    memcpy(data + first->length, second->data, second->length);

    String_ptr string = CreateString(data);
    if (!string){
        free(data);
        return NULL;
    }
    free(data);
    return string;
}

typedef struct{
    String_ptr city;
    String_ptr street;
    unsigned int houseNumber; 
    String_ptr building;
    unsigned int flatNumber;
    String_ptr index;
} Adress, *Adress_ptr;

typedef struct{
    Adress_ptr recipient;
    double weight;
    String_ptr id;
    String_ptr creationTime;
    String_ptr deliveryTime;
} Mail, *Mail_ptr, **Mail_array;

typedef struct{
    Adress_ptr postAdress;
    Mail_array mails;
} Post, *Post_ptr;

Post_ptr CreatePost(Adress_ptr recipient){
    Post_ptr post = (Post_ptr)malloc(sizeof(Post));
    if (!post){
        return NULL;
    } 
    post->postAdress = recipient;
    post->mails = NULL;
    return post;
}

Adress_ptr CreateAdress(char * city, char * street, unsigned int houseNumber, char * building, unsigned int flatNumber, char * index){
    String_ptr new_city = CreateString(city);
    if (!new_city){
        return NULL;
    } 

    String_ptr new_street = CreateString(street);
    if (!new_street){
        DeleteString(new_city);
        return NULL;
    }

    String_ptr new_building = CreateString(building);
    if (!new_building){
        DeleteString(new_city);
        DeleteString(new_street);
        return NULL;
    }

    String_ptr new_index = CreateString(index);
    if (!new_index)
    {
        DeleteString(new_city);
        DeleteString(new_street);
        DeleteString(new_building);
        return NULL;
    }
    
    Adress_ptr recipient = (Adress_ptr)malloc(sizeof(Adress));
    if (!recipient){
        DeleteString(new_city);
        DeleteString(new_street);
        DeleteString(new_building);
        DeleteString(new_index);
        return NULL;
    }

    recipient->city = new_city;
    recipient->street = new_street;
    recipient->houseNumber = houseNumber;
    recipient->building = new_building;
    recipient->flatNumber = flatNumber;
    recipient->index = new_index;

    return recipient;
}

Mail_ptr CreateMail(Adress_ptr recipient, double weight, char * id, char * creationTime, char * deliveryTime){
    String_ptr new_id = CreateString(id);
    if (!new_id){
        return NULL;
    } 

    String_ptr new_creationTime = CreateString(creationTime);
    if (!new_creationTime){
        DeleteString(new_id);
        return NULL;
    }

    String_ptr new_deliveryTime = CreateString(deliveryTime);
    if (!new_deliveryTime){
        DeleteString(new_id);
        DeleteString(new_creationTime);
        return NULL;
    }

    Mail_ptr mail = (Mail_ptr)malloc(sizeof(Mail));
    if (!mail){
        DeleteString(new_id);
        DeleteString(new_creationTime);
        DeleteString(new_deliveryTime);
        return NULL;
    }

    mail->recipient = recipient;
    mail->weight = weight;
    mail->id = new_id;
    mail->creationTime = new_creationTime;
    mail->deliveryTime = new_deliveryTime;

    return mail;
}

void DeleteAdress(Adress_ptr recipient){
    DeleteString(recipient->city);
    DeleteString(recipient->street);
    DeleteString(recipient->building);
    DeleteString(recipient->index);
    free(recipient);
}

void DeleteMail(Mail_ptr mail){
    DeleteAdress(mail->recipient);
    DeleteString(mail->id);
    DeleteString(mail->creationTime);
    DeleteString(mail->deliveryTime);
    free(mail);
}

void DeletePost(Post_ptr post, int length){
    DeleteAdress(post->postAdress);
    for (int i = 0; i < length; i++){
        DeleteMail((post->mails)[i]);
    }
    free(post);
}

int ValidateName(char * string){
    int length = strlen(string);
    for (int i = 1; i < length; i++){
        if (!islower(string[i])){
            return WRONG_INPUT;
        } 
    }
    return SUCCESS;
}

int ValidateCityStreet(char * string){
    int length = strlen(string);
    if (length == 0){
        return WRONG_INPUT;
    } 

    for (int i = 0; i < length; i++){
        if (!isalpha(string[i])){
            return WRONG_INPUT;
        } 
    }
    return SUCCESS;
}

int ValidateHouseFlat(int house){
    if (house <= 0){
        return WRONG_INPUT;
    } 
    return SUCCESS;
}

int ValidateBuilding(char * string){
    if (strcmp(string, "-") == SUCCESS){
        return SUCCESS;
    } 

    int length = strlen(string);
    for (int i = 0; i < length; i++){
        if (!(isalpha(string[i]) || isdigit(string[i]))){
            return WRONG_INPUT;
        } 
    }
    return SUCCESS;
}

int ValidateWeight(double weight){
    double eps = 0.0000001;
    if (weight > eps){
        return SUCCESS;
    } 
    return WRONG_INPUT;
}

int ValidateDigitIndex(char * string){
    int length = strlen(string);
    if (length == 0){
        return WRONG_INPUT;
    } 

    for (int i = 0; i < length; i++){
        if (!isdigit(string[i])){
            return WRONG_INPUT;
        } 
    }
    return SUCCESS;
}

int ValidateId(char * string){
    int result = ValidateDigitIndex(string);
    if (result == WRONG_INPUT){
        return WRONG_INPUT;
    } 
    
    result = strlen(string);
    if (result != 14){
        return WRONG_INPUT;
    } 
    return SUCCESS;
}

int ValidateTime(int day, int month, int year, int hours, int minutes, int seconds){
    if (year < 1900 || year > 9999 || month <= 0 || month > 12 || day <= 0 || day > 31 || hours < 0 || hours > 24 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59){
        return WRONG_INPUT;
    }
    return SUCCESS;
}

int Validation(char * city, char * street, int houseNumber, char * building, int flatNumber, char * index){
    int result;
    result = ValidateCityStreet(city);
    if (result != SUCCESS){
        return WRONG_INPUT;
    } 
    result = ValidateName(city);
    if (result != SUCCESS){
        return WRONG_INPUT;
    } 

    result = ValidateCityStreet(street);
    if (result != SUCCESS){
        return WRONG_INPUT;
    } 
    result = ValidateName(street);
    if (result != SUCCESS){
        return WRONG_INPUT;
    } 

    result = ValidateHouseFlat(houseNumber);
    if (result != SUCCESS){
        return WRONG_INPUT;
    } 

    result = ValidateBuilding(building);
    if (result != SUCCESS){
        return WRONG_INPUT;
    } 

    result = ValidateHouseFlat(flatNumber);
    if (result != SUCCESS){
        return WRONG_INPUT;
    } 

    result = ValidateDigitIndex(index);
    if (result != SUCCESS || strlen(index) != 6){
        return WRONG_INPUT;
    }

    return SUCCESS;
}

void PrintAdress(Adress_ptr recipient){
    printf("%s ", recipient->city->data);
    printf("%s ", recipient->street->data);
    printf("%d ", recipient->houseNumber);
    printf("%s ", recipient->building->data);
    printf("%d ", recipient->flatNumber);
    printf("%s\n", recipient->index->data);
}

int TimeDifferenceChar(char * creation, char * delivered){
    time_t time1;
    int day, month, year, hours, minutes, seconds;
    sscanf(creation, "%d:%d:%d %d:%d:%d", &day, &month, &year, &hours, &minutes, &seconds);
    struct tm ttime;
    ttime.tm_mday = day;
    ttime.tm_mon = month - 1;
    ttime.tm_year = year - 1900;
    ttime.tm_hour = hours;
    ttime.tm_min = minutes;
    ttime.tm_sec = seconds;
    ttime.tm_isdst = -1;
    time_t time2;
    sscanf(delivered, "%d:%d:%d %d:%d:%d", &day, &month, &year, &hours, &minutes, &seconds);
    struct tm ttime1;
    ttime1.tm_mday = day;
    ttime1.tm_mon = month - 1;
    ttime1.tm_year = year - 1900;
    ttime1.tm_hour = hours;
    ttime1.tm_min = minutes;
    ttime1.tm_sec = seconds;
    ttime1.tm_isdst = -1;

    time_t time_1 = mktime(&ttime);
    time_t time_2 = mktime(&ttime1);

    if (time_1 < time_2){
        return -1; 
    } 
    if (time_1 > time_2){
        return 1;
    } 
    return 0;
}

int TimeDifference(String_ptr creation, String_ptr delivered){
    time_t time1;
    int day, month, year, hours, minutes, seconds;
    sscanf(creation->data, "%d:%d:%d %d:%d:%d", &day, &month, &year, &hours, &minutes, &seconds);
    struct tm ttime;
    ttime.tm_mday = day;
    ttime.tm_mon = month - 1;
    ttime.tm_year = year - 1900;
    ttime.tm_hour = hours;
    ttime.tm_min = minutes;
    ttime.tm_sec = seconds;
    ttime.tm_isdst = -1;
    time_t time2;
    sscanf(delivered->data, "%d:%d:%d %d:%d:%d", &day, &month, &year, &hours, &minutes, &seconds);
    struct tm ttime1;
    ttime1.tm_mday = day;
    ttime1.tm_mon = month - 1;
    ttime1.tm_year = year - 1900;
    ttime1.tm_hour = hours;
    ttime1.tm_min = minutes;
    ttime1.tm_sec = seconds;
    ttime1.tm_isdst = -1;

    time_t time_1 = mktime(&ttime);
    time_t time_2 = mktime(&ttime1);

    if (time_1 > time_2){
        return 1;
    }
    if (time_1 < time_2){
        return -1; 
    }
    return 0;
}

int MinimumTime(Mail_array mails, int time, int length){
    int index, result;
    if (length == 1){
        return 0;
    } 
    String_ptr current = mails[0]->creationTime;
    for (int i = 0; i < length; i++){
        result = TimeDifference(current, mails[i]->creationTime);
        if (result > 0){
            current = mails[i]->creationTime;
            index = i;
        }
    }

    if (time == -1){
        return index;
    } 
    
    for (int i = 0; i < length; i++){
        result = TimeDifference(mails[time]->creationTime, mails[i]->creationTime);
        if ((result == 0 && i > time) || result < 0){
            current = mails[i]->creationTime;
            index = i;
            break;
        }
    }

    for (int i = 0; i < length; i++){
        result = TimeDifference(mails[time]->creationTime, mails[i]->creationTime);
        if (result < 0 && TimeDifference(current, mails[i]->creationTime) > 0){
            current = mails[i]->creationTime;
            index = i;
        }
    }
    return index;
}

int CheckDelivered(String_ptr delivered){
    int day, month, year, hours, minutes, seconds;
    sscanf(delivered->data, "%d:%d:%d %d:%d:%d", &day, &month, &year, &hours, &minutes, &seconds);
    struct tm ttime;
    ttime.tm_mday = day;
    ttime.tm_mon = month - 1;
    ttime.tm_year = year - 1900;
    ttime.tm_hour = hours;
    ttime.tm_min = minutes;
    ttime.tm_sec = seconds;
    ttime.tm_isdst = -1;

    time_t current = time(NULL);

    time_t delivere = mktime(&ttime);

    if (delivere < current){
        return SUCCESS;
    } 
    return ERROR;
}

void PrintStatus(Mail_ptr mail){
    printf("Адрес получателя: ");
    PrintAdress(mail->recipient);
    printf("Вес: %f\n", mail->weight);
    printf("Идентификатор сообщения: %s\n", mail->id->data);
    printf("Время создания: %s\n", mail->creationTime->data);
    int result = CheckDelivered(mail->deliveryTime);
    if (result == SUCCESS){
        printf("Посылка доставлено\n");
        return;
    }
    else{
        printf("Посылка еще не доставлено\n");
    } 
}

void PrintDeliveres(Post_ptr post, int length) {
    if (post->mails == NULL || length <= 0) {
        printf("Нет доставленных посылок\n");
        return;
    }

    int index = -1;
    int count = 0;
    for (int i = 0; i < length; i++) {
        index = MinimumTime(post->mails, index, length);
        if (index >= 0 && index < length && post->mails[index] != NULL && 
            CheckDelivered((post->mails)[index]->deliveryTime) == SUCCESS) {
            printf("%d)\n", count + 1);
            PrintStatus(post->mails[index]);
            count += 1;
        }
    }
    if (count == 0) {
        printf("Нет доставленных посылок\n");
    }
}

int FindMailById(Mail_array mails, char * id, int length){
    if (mails == NULL){
        return -1;
    } 
    for (int i = 0; i < length; i++){
        if (strcmp(id, mails[i]->id->data) == SUCCESS){
            return i;
        } 
    }
    return -1;
}

void PrintAll(Post_ptr post, int length){
    printf("Посылки:\n");
    if (length == 0){
        printf("Посылки отсутствуют\n");
        return;
    }
    for (int i = 0; i < length; i++){
        PrintStatus((post->mails)[i]);
        printf("\n");
    }
}

void InsertMail(int length, Post_ptr post, Mail_ptr mail){
    int result;
    Mail_ptr temp, temp1;
    for (int i = 0; i < length - 1; i++){
        result = AreStringsEqual((post -> mails)[i]->recipient->index, mail->recipient->index);
        if (result == 0){
            result = AreStringsEqual((post->mails)[i]->id, mail->id);
            if (result == 1){
                temp = (post->mails)[i];
                (post->mails)[i] = mail;
                for (int j = i + 1; j < length; j++){
                    temp1 = (post->mails)[j];
                    (post->mails)[j] = temp;
                    temp = temp1;
                }
                return;
            }
        }
        if (result == 1){
            temp = (post->mails)[i];
            (post->mails)[i] = mail;
            for (int j = i + 1; j < length; j++){
                temp1 = (post->mails)[j];
                (post->mails)[j] = temp;
                temp = temp1;
            }
            return;
        }
    }
    (post->mails)[length - 1] = mail;
    return;
}

int AppendMail(Mail_ptr mail, Post_ptr post, int length){
    Mail_array temp;
    temp = (Mail_array)realloc(post->mails, (length + 1) * sizeof(Mail_ptr));
    if (!temp){
        DeletePost(post, length);
        DeleteMail(mail);
        return ERROR_OF_MEMORY;
    }
    post->mails = temp;
    InsertMail(length + 1, post, mail);
    return SUCCESS;
}

int RemoveMail(Post_ptr * post, int index, int length){
    Mail_ptr mail = ((*post)->mails)[index];
    for (int i = index; i < length - 1; i++){
        ((*post)->mails)[i] = ((*post)->mails)[i + 1];
    }

    ((*post)->mails)[length - 1] = NULL;
    DeleteMail(mail);
    length -= 1;
    if (length == 0){
        free((*post)->mails);
    }
    else {
        Mail_array temp = (Mail_array)realloc((*post)->mails, (length) * sizeof(Mail_ptr));
        if (!temp) {
            DeletePost(*post, length);
            return ERROR_OF_MEMORY;
        }
        (*post)->mails = temp;
    }
    return SUCCESS;
}

void DeleteMailByIndex(Post_ptr post, int *length){
    int result;
    printf("\nВведите идентификатор посылки: ");
    char id[20];
    result = -1;
    while (result != SUCCESS){
        result = scanf("%s", id);
        if (result <= 0){
            printf("\nНеправильный идентификатор, повторите еще раз: ");
        }
        else{
            if (strlen(id) == 14){
                result = ValidateDigitIndex(id);
            } 
            if (result == SUCCESS){
                result = FindMailById(post -> mails, id, *length);
                if (result != -1){
                    break;
                } 
                printf("\nНеправильный идентификатор, повторите еще раз: ");
            }
        }
    }
    RemoveMail(&post, result, *length);
    (*length) -= 1;
}

int AddMail(Post_ptr post, int * length){
    int result = -1;
    int size = 40;
    char city[size], street[size], building[size], index[size], id[size];
    int houseNumber, flatNumber;

    printf("Введите адрес получателя.\n");
    printf("Введите адрес формата: city street number of house building number of flat index\n");
    printf("Пример: Москва Сталинская 3 - 23 11231\n");
    printf("Введите адрес: ");
    while (result != SUCCESS){
        result = scanf("%s %s %d %s %d %s", city, street, &houseNumber, building, &flatNumber, index);
        if (result == 6){
            result = Validation(city, street, houseNumber, building, flatNumber, index);
            if (result == SUCCESS){
                break;
            } 
        }
        else if (result == 0){
            result = -1;
        } 
        printf("\n\nНеправильный ввод, повторите еще раз\n");
        fflush(stdin);
        printf("Введите адрес: ");
    }
    printf("\n\n");

    printf("Введите вес: ");
    double weight;
    result = -1;
    while(result != SUCCESS){
        result = scanf("%lf", &weight);
        if (result > 0){
            result = ValidateWeight(weight);
            if (result == SUCCESS){
                break;
            } 
        }
        else if (result == 0){
            result = -1;
        } 
        printf("\n\nНеправильный ввод, повторите еще раз\n");
        fflush(stdin);
        printf("Введите вес: ");
    }

    printf("\n\nВведите идентификатор из 14 цифр: ");
    result = scanf("%s", id);
    if (result){
        result = ValidateDigitIndex(id);
        if (FindMailById(post->mails, id, *length) != -1){
            printf("\nПосылка с таким идентификатором уже существует.\n");
            result = WRONG_INPUT;
        }
        if (result == SUCCESS && strlen(id) != 14){
            result = WRONG_INPUT;
        } 
    }
    while (result != SUCCESS){
        printf("\nНеправильный ввод, повторите еще раз\n");
        fflush(stdin);
        printf("Введите идентификатор посылки:");
        result = scanf("%s", id);
        if (result){
            result = ValidateDigitIndex(id);
            if (FindMailById(post->mails, id, *length) != -1){
                printf("\nПосылка с таким идентификатором уже существует.\n\n");
                result = WRONG_INPUT;
            }
            if (result == SUCCESS && strlen(id) != 14){
                result = WRONG_INPUT;
            } 
        }
        else if (result == 0){
            result = -1;
        } 
    }

    printf("Введите время создания (dd:MM:yyyy hh:mm:ss): ");
    char time[size];
    int day = 0, month = 0, year = 0, hours = 0, minutes = 0, seconds = 0;
    result = scanf("%d:%d:%d %d:%d:%d", &day, &month, &year, &hours, &minutes, &seconds);
    if (result){
        result = ValidateTime(day, month, year, hours, minutes, seconds);
    } 
    while (result != SUCCESS){
        printf("\n\nНеправильный ввод, повторите еще раз\n");
        fflush(stdin);
        printf("Введите время создания (dd:MM:yyyy hh:mm:ss): ");
        result = scanf("%d:%d:%d %d:%d:%d", &day, &month, &year, &hours, &minutes, &seconds);
        if (result){
            result = ValidateTime(day, month, year, hours, minutes, seconds);
            if (result == SUCCESS){
                break;
            } 
        }
        else if (result == 0){
            result = -1;
        } 
    }
    sprintf(time, "%02d:%02d:%04d %02d:%02d:%02d", day, month, year, hours, minutes, seconds);

    printf("Введите время доставки (dd:MM:yyyy hh:mm:ss): ");
    char time1[size];
    result = scanf("%d:%d:%d %d:%d:%d", &day, &month, &year, &hours, &minutes, &seconds);
    if (result){
        result = ValidateTime(day, month, year, hours, minutes, seconds);
        if (result == SUCCESS){
            sprintf(time1, "%02d:%02d:%04d %02d:%02d:%02d", day, month, year, hours, minutes, seconds);
            result = TimeDifferenceChar(time1, time);
            if (result > 0){
                result = SUCCESS;
            } 
            else{
                printf("Время доставки должно превышать время создания\n");
            } 
        }
    }
    while (result != SUCCESS){
        printf("\n\nНеправильный ввод, повторите еще раз\n");
        fflush(stdin);
        printf("Введите время доставки (dd:MM:yyyy hh:mm:ss): ");
        result = scanf("%d:%d:%d %d:%d:%d", &day, &month, &year, &hours, &minutes, &seconds);
        if (result == 0){
            result = -1;
        } 
        else if (result == 6){
            result = ValidateTime(day, month, year, hours, minutes, seconds);
            if (result == SUCCESS){
                sprintf(time1, "%02d:%02d:%04d %02d:%02d:%02d", day, month, year, hours, minutes, seconds);
                result = TimeDifferenceChar(time1, time);
                if (result >= 0){
                    result = SUCCESS;
                } 
                else{
                    printf("Время доставки должно быть больше времени создания.\n");
                } 
            }
        }
    }
    Adress_ptr recipient = CreateAdress(city, street, houseNumber, building, flatNumber, index);
    if (recipient == NULL){
        DeletePost(post, *length);
        return ERROR_OF_MEMORY;
    }

    Mail_ptr mail = CreateMail(recipient, weight, id, time, time1);
    if (mail == NULL){
        DeleteAdress(recipient);
        DeletePost(post, *length);
        return ERROR_OF_MEMORY;
    }

    result = AppendMail(mail, post, *length);
    (*length) += 1;
    if (result == ERROR_OF_MEMORY){
        DeleteMail(mail);
        DeletePost(post, *length);
        return ERROR_OF_MEMORY;
    }

    printf("\n\nПосылка создана\n");
    return SUCCESS;
}

int Commands(int command, Post_ptr post, int * length){
    int result = 0;
    switch (command){
        case 1:
            result = AddMail(post, length);
            if (result == ERROR_OF_MEMORY){
                return ERROR_OF_MEMORY;
            } 
            break;
        case 2:
            PrintAll(post, *length);
            result = SUCCESS;
            break;
        case 3:
            PrintDeliveres(post, *length);
            result = SUCCESS;
            break;
        case 4:
            DeleteMailByIndex(post, length);
            result = SUCCESS;
        default:
            break;
    }
    return SUCCESS;
}

int main(){
    printf("Введите адрес получателя.\n");
    printf("Введите адрес формата: city street number of house building number of flat index\n");
    printf("Пример: Moscow Lenina 11 - 28 121111\n");
    printf("Введите адрес: ");
    int size = 20;
    char city[size], street[size], building[size], index[size];
    int houseNumber, flatNumber;
    int result = -1;

    while (result != SUCCESS){
        result = scanf("%s %s %d %s %d %s", city, street, &houseNumber, building, &flatNumber, index);
        if (result == 6){
            result = Validation(city, street, houseNumber, building, flatNumber, index);
            if (result == SUCCESS){
                printf("\n\n");
                break;
            } 
        }
        else if (result == 0){
            result = -1;
        } 
        printf("\n\nНеправильный ввод, повторите еще раз\n");
        fflush(stdin);
        printf("Введите адрес: ");
    }

    Adress_ptr postAdress = CreateAdress(city, street, houseNumber, building, flatNumber, index);

    int length = 0;
    Post_ptr post = CreatePost(postAdress);

    printf("\nВаш адрес:");
    PrintAdress(post->postAdress);

    
    int command = -1;
    while (command != SUCCESS){
        int command;
        printf("\n\nSelect an option:\n");
        printf("1. Добавить посылку\n");
        printf("2. Вывести все отправленные посылки\n");
        printf("3. Поиск доставленной посылки\n");
        printf("4. Удалить посылку\n");
        printf("0. Выход\n");
        printf("Введите команду: ");
        while (!scanf("%d", &command)){
            fflush(stdin);
            printf("\n\nОшибка ввода\n");
            printf("Введите команду: ");
        }
        printf("\n");
        if (command == SUCCESS){
            break;
        } 
        result = Commands(command, post, &length);
        if (result != SUCCESS){
            printf("Ошибка выделения памяти\n");
            return result;
        }
    }

    DeletePost(post, length);

}
