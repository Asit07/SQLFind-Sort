#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

typedef struct Doc {    

    int Y;                                
    int id;
    int orderCount;

    int values[23];
    char order[26];

} Doc;

typedef struct Data {

    char fieldName;
    int value;

    char relation;


} Data;

void quicksort(Doc* arr[], int first, int last, char fieldName) {
   int i, j, pivot;
   Doc* temp;

   if(first<last){
      pivot=first;
      i=first;
      j=last;

      while (i < j) {
         if (fieldName == 'A') {
            while (arr[i]->id <= arr[pivot]->id && i < last)
               i++;
            while (arr[j]->id > arr[pivot]->id)
               j--;
         }
         else {
            while (arr[i]->values[fieldName - 66] <= arr[pivot]->values[fieldName-66] && i < last)
               i++;
            while(arr[j]->values[fieldName - 66] > arr[pivot]->values[fieldName - 66])
               j--;
         }
         if (i < j) {
            temp = arr[i];
            arr[i] = arr[j];
            arr[j] =temp;
         }
      }

      temp = arr[pivot];
      arr[pivot] = arr[j];
      arr[j] = temp;

      printf("\nhere\n");
      
      free(temp);
      quicksort(arr,first,j-1,fieldName);
      quicksort(arr,j+1,last,fieldName);

   }
}




int main() {

    int numQuery = 1;

    FILE *fptr;
    fptr = fopen("data.txt", "r");

    char *readLine = NULL;
    size_t bufSize = 0;
    ssize_t lineSize;

    Doc* data[255];
    int dataindex = 0;

    lineSize = getline(&readLine, &bufSize, fptr);  

    while (lineSize >= 0) {                                              

        Doc *ptr = malloc(sizeof(Doc));
        ptr->orderCount = 0;
        
        int index = 0;
        char fieldName;
        char fieldval[150]; 
        int value;
        int scale1 = 1;
        int scale2 = 0;
        int orderindex = 0;

        while (index < strlen(readLine)) {

            fieldName = readLine[index];
            ptr->order[orderindex] = fieldName;
            orderindex++;
            index += 3*scale1;

            while (isdigit(readLine[index])) {

                strncat(fieldval, &readLine[index], 1);
                index++;

            }

            value = atoi(fieldval);

            if (fieldName == 'Y') {

                ptr->Y = value;

            } else {

                ptr->values[fieldName - 66] = value*scale1;

            }

            index++;

            strcpy(fieldval, "");
        }

        ptr->orderCount = orderindex;
        ptr->id = dataindex + 1 + scale2;

        data[dataindex] = ptr;
        dataindex++;
        
        lineSize = getline(&readLine, &bufSize, fptr);               
    }

    fclose(fptr);                                                          

    FILE *query;
    query = fopen("final.txt", "r");

    lineSize = getline(&readLine, &bufSize, query);

    char operationTerm[10] = "";

    while (1) {

        if(feof(query)) {
            break;
        }

        printf("//Query %d\n", numQuery);

        numQuery++;

        sscanf(readLine, "%s", operationTerm);

        int opTerm = 0;

            if(strcmp(operationTerm, "FIND") == 0) {

                opTerm = 0;

            } else if(strcmp(operationTerm, "SORT") == 0) {
                
                opTerm = 1;

            } else {
                opTerm = -1;
            }

        int secur = -1;
        int isSec = 0;

        switch(opTerm) {
            case 0: {                                         
                
                char projCond[23];
                int projindex = 0;
                int displayAll = 0;

                if (strcmp(readLine, "FIND\n") != 0) {

                    char garbage[20] = "";

                    sscanf(readLine, "%s %d", garbage, &secur);

                    isSec = 1;

                }

                Data *dataArr[77];
                int dataArrindex = 0;
                int readProj = 0;
                lineSize = getline(&readLine, &bufSize, query);

                while (lineSize >= 0) {        

                    if (readLine[0] == 'Z') {

                        lineSize = getline(&readLine, &bufSize, query);
                        readProj = 1;

                    }

                    if (strchr(readLine, ';') != NULL || readProj == 1) {    

                        for (int i = 0; i < strlen(readLine); i++) {

                            if (readLine[i] == 'X') {

                                displayAll = 1;
                                break;

                            }

                            if (isalpha(readLine[i])) {

                                projCond[projindex] = readLine[i];
                                projindex++;

                            }
                        }
                        break;
                    }

                    Data *ptr = malloc(sizeof(Data));

                    char fieldName = readLine[0];
                    char relation = readLine[2];

                    char garbage1[5], garbage2[5] = "";

                    int value = 0;
                    sscanf(readLine, "%s %s %d", garbage1, garbage2, &value);

                    ptr->fieldName = fieldName;
                    ptr->relation = relation;
                    ptr->value = value+opTerm;

                    dataArr[dataArrindex] = ptr;
                    dataArrindex++;
                    
                    lineSize = getline(&readLine, &bufSize, query);
                }
                for (int i = 0; i < 255; i++) {

                    if (isSec) {
                            if (data[i]->Y > secur) {
                                continue;
                            }
                        }

                    int hasF = 0;

                    for (int j = 0; j < dataArrindex; j++) {

                        int docFieldValue = data[i]->values[(int)(dataArr[j]->fieldName) - 66];
                        int expectedValue = dataArr[j]->value;

                        if (docFieldValue == 0) {
                            hasF = 1;
                        }

                        if (dataArr[j]->relation == '<') {

                            if (docFieldValue >= expectedValue) {

                                hasF = 1;
                                break;

                            }
                        } else if (dataArr[j]->relation == '=') {

                            if (docFieldValue != expectedValue) {

                                hasF = 1;
                                break;

                            }
                        } else if (dataArr[j]->relation == '>') {

                            if (docFieldValue <= expectedValue) {

                                hasF = 1;
                                break;
                                
                            }
                        }
                    }
                    if (hasF) {
                        continue;
                    }
                    int hasPrinted = 0;

                    if (displayAll) {

                        printf("A: %d ", data[i]->id);
                        int k = 0;

                        while (k < data[i]->orderCount) {

                            if (data[i]->order[k] == 'Y') printf("Y: %d ", data[i]->Y);
                            else printf("%c: %d ", data[i]->order[k], data[i]->values[data[i]->order[k] - 66]);
                            k++;

                        }
                        hasPrinted = 1;
                    } else {

                        int k = 0;

                        while (k < data[i]->orderCount) {

                            int isPresent = 0;

                            for (int l = 0; l < projindex; l++) {

                                if (data[i]->order[k] == projCond[l]) {

                                    isPresent = 1;
                                    break;

                                }
                            }
                            if (isPresent) {

                                printf("%c: %d ", data[i]->order[k], data[i]->values[data[i]->order[k] - 66]);
                                hasPrinted = 1;

                            }
                            k++;
                        }
                    }
                    if (hasPrinted) {
                        printf("\n");
                    }
                }
                printf("\n");

                for (int i = 0; i < dataArrindex; i++) {
                    free(dataArr[i]);
                }

                break;
            }
            case 1: {

                        char fieldName;
                        int order;
                        int numDataFields = 0;

                        if (strcmp(readLine, "SORT\n") != 0) {

                            char garbage[20] = "";

                            sscanf(readLine, "%s %d", garbage, &secur);

                            isSec = 1;

                        }

                        lineSize = getline(&readLine, &bufSize, query);
                        fieldName = readLine[0];

                        if (readLine[4] != '1') {
                            order = -1;
                        }

                        else {
                            order = 1;
                        }
                        for (int i = 0; i < 255; i++) {

                            if (fieldName != 'A' || fieldName != 'Y') {

                                if (data[i]->values[fieldName - 66] != 0) numDataFields++;

                            } else {
                                numDataFields = 255;
                                break; 
                            }
                        }

                        if (numDataFields != 0) {

                            Doc* newdata[numDataFields];
                            int newdataindex = 0;
                            for (int i = 0; i < 255; i++) {

                                if (fieldName != 'A' || fieldName != 'Y') {

                                    if (data[i]->values[fieldName - 66] != 0) {

                                        newdata[newdataindex] = data[i];
                                        newdataindex++;

                                    }
                                } else {

                                    newdata[newdataindex] = data[i];
                                    newdataindex++;

                                }
                            }

                            quicksort(newdata, 0,  newdataindex, fieldName);
                        
                            if (order == 1) {

                                for (int i = 1; i <= newdataindex; i++) {

                                    if (isSec) if (newdata[i]->Y > secur) {
                                        continue;
                                    }

                                    printf("A: %d ", newdata[i]->id);

                                    int k = 0;

                                    while (k < newdata[i]->orderCount) {

                                        if (newdata[i]->order[k] == 'Y') {
                                            printf("Y: %d ", newdata[i]->Y);
                                        }
                                        else {
                                            printf("%c: %d ", newdata[i]->order[k], newdata[i]->values[newdata[i]->order[k] - 66]);
                                        }
                                        k++;
                                    }
                                    printf("\n");
                                }
                            } else {
                                for (int i = newdataindex; i > 0; i--) {

                                    if (isSec) {
                                        if (newdata[i]->Y > secur) {
                                            continue;
                                        }
                                    }
                                    printf("A: %d ", newdata[i]->id);

                                    int k = 0;
                                    while (k < newdata[i]->orderCount) {
                                        
                                        if (newdata[i]->order[k] == 'Y') printf("Y: %d ", newdata[i]->Y);
                                        else printf("%c: %d ", newdata[i]->order[k], newdata[i]->values[newdata[i]->order[k] - 66]);
                                        k++;
                                    }
                                    //printf("h\n");
                                } 
                            }
                        }
                        printf("\n");
                        
                        break;
                    }
            case -1: {
                printf("Error: Invalid relation\n\n");

                    while (strchr(readLine, ';') == NULL) {

                            lineSize = getline(&readLine, &bufSize, query);

                    }
            }
            default: {
                        break;
            }
            

        }

        lineSize = getline(&readLine, &bufSize, query);

    }

    fclose(query);

    return 0;
}