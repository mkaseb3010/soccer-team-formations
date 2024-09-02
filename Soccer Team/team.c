// Author: Mazen Kaseb

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

// Linked List Definitions
//  here for use throughout the file.
struct Player {
    char *firstName;
    char *familyName;
    char position;
    int value;
    struct Player *next;
};

// Linked List Function Declarations
// Functions that modify the linked list.
struct Player *addItem(struct Player *node, struct Player *head);
struct Player *deleteItem(char *familyName, struct Player *head);
struct Player *searchPlayer(char *familyName, struct Player *head);
void searchPrintByValue(int value, struct Player *head); // Prints all players with values less than equal inputted value.
void printPlayers(struct Player *head);
void printPlayer(struct Player *head);
struct Player *deleteList(struct Player *head);

// Support Function Declarations


void safegets(char s[], int arraySize);        // gets without buffer overflow
void familyNameDuplicate(char familyName[]);   // marker/tester friendly
void familyNameFound(char familyName[]);       //   functions to print
void familyNameNotFound(char familyName[]);    //     messages to user
void familyNameDeleted(char familyName[]);
void printTeamEmpty(void);
void printTeamTitle(void);
void printNoPlayersWithLowerValue(int value);
int getEnumeration(char position); // Enumerates positions G, D, M, S.

// Program-wide Constants

const int MAX_LENGTH = 1023;
const char NULL_CHAR = '\0';
const char NEWLINE = '\n';
const char GOALKEEPER = 'G';
const char DEFENDER = 'D';
const char MIDFIELDER = 'M';
const char STRIKER = 'S';

int main(void) {
    const char bannerString[]
            = "Personal Team Maintenance Program.\n\n";
    const char commandList[]
            = "Commands are I (insert), D (delete), S (search by name),\n"
                    "  V (search by value), P (print), Q (quit).\n";

    // Declare linked list head.
    struct Player *head = NULL;

    // announce start of program
    printf("%s", bannerString);
    printf("%s", commandList);

    char response;
    char input[MAX_LENGTH + 1];
    do {
        printf("\nCommand?: ");
        safegets(input, MAX_LENGTH + 1);
        // Response is first char entered by user.
        // Convert to uppercase to simplify later comparisons.
        response = toupper(input[0]);

        if (response == 'I') {
            // Insert a player entry into the linked list.
            // Maintain the list in correct order (G, D, M, S).
            struct Player *player = malloc(sizeof(struct Player));
            player->familyName = malloc(MAX_LENGTH + 1);
            player->firstName = malloc(MAX_LENGTH + 1);
            char *positionInput = malloc(MAX_LENGTH + 1);
            char *valueInput = malloc(MAX_LENGTH + 1);
            if(!(player && player->familyName && player->firstName && positionInput && valueInput)){
                exit(2); // Can not malloc.
            }
            printf("  family name: ");
            safegets(player->familyName, MAX_LENGTH + 1);

            printf("  first name: ");
            safegets(player->firstName, MAX_LENGTH + 1);

            printf("  position: ");
            int counter = 0;
            while (*positionInput != GOALKEEPER && *positionInput != DEFENDER &&
                   *positionInput != MIDFIELDER && *positionInput != STRIKER) {
                if (counter > 0) {
                    printf("Position entered is not valid. Please try again. : ");
                }
                safegets(positionInput, MAX_LENGTH + 1);
                *positionInput = (char) toupper(*positionInput);
                counter++;
            }
            player->position = *positionInput;

            printf("  value: ");
            safegets(valueInput, MAX_LENGTH + 1);
            player->value = atoi(valueInput); // Converts input into an integer an assigns it to player's value.

            if (searchPlayer(player->familyName, head) == NULL) {
                head = addItem(player, head); // Add item to the list if another item with the same family name does not exist.
            } else {
                familyNameDuplicate(player->familyName);
            }

            free(positionInput);
            free(valueInput);
        } else if (response == 'D') {
            // Delete a player from the list.
            if (head == NULL) {
                printTeamEmpty();
            } else {
                printf("\nEnter family name for entry to delete: ");
                char *inputFamilyName = malloc(MAX_LENGTH + 1);
                if(!(inputFamilyName)){
                    exit(2); // Can not malloc.
                }
                safegets(inputFamilyName, MAX_LENGTH + 1);
                struct Player *playerFound = searchPlayer(inputFamilyName, head);
                if (playerFound == NULL) {
                    familyNameNotFound(inputFamilyName);
                } else {
                    head = deleteItem(playerFound->familyName, head);
                    familyNameDeleted(inputFamilyName);
                }
                free(inputFamilyName);
            }
        } else if (response == 'S') {
            // Search for a player by family name.
            if (head == NULL) {
                printTeamEmpty();
            } else {
                printf("\nEnter family name to search for: ");
                char *inputFamilyName = malloc(MAX_LENGTH + 1);
                if(!(inputFamilyName)){
                    exit(2); // Can not malloc.
                }
                safegets(inputFamilyName, MAX_LENGTH + 1);
                struct Player *playerFound = searchPlayer(inputFamilyName, head);
                if (playerFound != NULL) {
                    familyNameFound(inputFamilyName);
                    printf("\n");
                    printPlayer(playerFound);
                } else {
                    familyNameNotFound(inputFamilyName);
                }
                free(inputFamilyName);
            }
        } else if (response == 'V') {
            // Search for players that are worth less than or equal a value.
            if (head == NULL) {
                printTeamEmpty();
            } else {
                printf("\nEnter value: ");
                char *valueInput = malloc(MAX_LENGTH + 1);
                if(!(valueInput)){
                    exit(2); // Can not malloc.
                }
                safegets(valueInput, MAX_LENGTH + 1);
                int valueInteger = atoi(valueInput);
                searchPrintByValue(valueInteger, head);
                free(valueInput);
            }
        } else if (response == 'P') {
            printPlayers(head);
        } else if (response == 'Q') { ; // do nothing, we'll catch this case below
        } else {
            // do this if no command matched ...
            printf("\nInvalid command.\n%s\n", commandList);
        }
    } while (response != 'Q');

    head = deleteList(head);
    printPlayers(head);
    exit(0); // Return with no problems.
}

// Support Function Definitions

// Function to get a line of input without overflowing target char array.
void safegets(char s[], int arraySize) {
    int i = 0, maxIndex = arraySize - 1;
    char c;
    while (i < maxIndex && (c = getchar()) != NEWLINE) {
        s[i] = c;
        i = i + 1;
    }
    s[i] = NULL_CHAR;
}

// Function to call when user is trying to insert a family name
// that is already in the book.
void familyNameDuplicate(char familyName[]) {
    printf("\nAn entry for <%s> is already in the team!\n"
                   "New entry not entered.\n", familyName);
}

// Function to call when a player with this family name was found in the team.
void familyNameFound(char familyName[]) {
    printf("\nThe player with family name <%s> was found in the team.\n",
           familyName);
}

// Function to call when a player with this family name was not found in the team.
void familyNameNotFound(char familyName[]) {
    printf("\nThe player with family name <%s> is not in the team.\n",
           familyName);

}

// Function to call when a family name that is to be deleted
// was found in the team.
void familyNameDeleted(char familyName[]) {
    printf("\nDeleting player with family name <%s> from the team.\n",
           familyName);
}

// Function to call when printing an empty team.
void printTeamEmpty(void) {
    printf("\nThe team is empty.\n");
}

// Function to call to print title when whole team being printed.
void printTeamTitle(void) {
    printf("\nMy Team: \n");
}

// Function to call when no player in the team has lower or equal value to the given value
void printNoPlayersWithLowerValue(int value) {
    printf("\nNo player(s) in the team is worth less than or equal to <%d>.\n", value);
}

// Function to enumerate the positions.
int getEnumeration(char position) {
    enum positions {
        G, D, M, S
    };
    int positionRank;
    switch (position) {
        case 'G' :
            positionRank = G;
            break;
        case 'D' :
            positionRank = D;
            break;
        case 'M' :
            positionRank = M;
            break;
        case 'S' :
            positionRank = S;
            break;
        default :
            break;
    }
    return positionRank;
}

// Adds a player to the list.
struct Player *addItem(struct Player *node, struct Player *head) {
    if (head == NULL) { // If the list is empty...
        head = malloc(sizeof(struct Player));
        if(!(head)){
            exit(2); // Can not malloc.
        }
        *head = *node;
        free(node);
        head->next = NULL;
    } else { // If the list is not empty...
        int nextPositionRank = getEnumeration(node->position) + 1;
        struct Player *current = head;
        struct Player *prev = NULL;

        while (current != NULL) {
            int currentPositionRank = getEnumeration(current->position);
            if (currentPositionRank >= nextPositionRank) {
                break;
            }
            prev = current;
            current = current->next;
        }
        if (prev == NULL) { // If the player needs to be added to the first position...
            node->next = head;
            head = node;
        } else {
            node->next = current;
            prev->next = node;
        }
    }
    return head;
}
// Deletes a player from the list.
struct Player *deleteItem(char *familyName, struct Player *head) {
    if (strcmp(familyName, head->familyName) == 0) { // If heads is the node we are looking for...
        struct Player *toReturn = head->next;
        free(head->firstName);
        free(head->familyName);
        free(head);
        return toReturn;
    } else { // If the node we are looking for is somewhere else...
        struct Player *current = head;
        while (current->next != NULL) {
            if (strcmp(familyName, current->next->familyName) == 0) {
                break;
            }
            current = current->next;
        }
        struct Player *itemToBeDeleted = current->next;
        current->next = itemToBeDeleted->next;
        free(itemToBeDeleted->familyName);
        free(itemToBeDeleted->firstName);
        free(itemToBeDeleted);
        return head;
    }
}
// Searches for a player in the list and returns it if finds any.
struct Player *searchPlayer(char *familyName, struct Player *head) {
    struct Player *current = head;
    while (current != NULL) {
        if (strcmp(familyName, current->familyName) == 0) {
            return current;
        }
        current = current->next;
    }
    return current;
}
// Prints players with values less than or equal to an inputted value.
void searchPrintByValue(int value, struct Player *head) {
    int counter = 0;
    while (head != NULL) {
        if (head->value <= value) {
            printf("\n");
            printPlayer(head);
            counter++;
        }
        head = head->next;
    }
    if (counter <= 0) {
        printNoPlayersWithLowerValue(value);
    }
}
// Prints all the players.
void printPlayers(struct Player *head) {
    if (head == NULL) {
        printTeamEmpty();
    } else {
        printTeamTitle();
        printf("\n");
        while (head != NULL) {
            printPlayer(head);
            head = head->next;
            if(head != NULL){
                printf("\n");
            }
        }
    }
}
// Prints a single player.
void printPlayer(struct Player *head) {
    printf("%s\n", head->familyName);
    printf("%s\n", head->firstName);
    printf("%c\n", head->position);
    printf("%d\n", head->value);
}
// Deletes the whole list and frees the memory allocated by nodes.
struct Player *deleteList(struct Player *head) {
    struct Player *current = head;
    struct Player *toBeDeleted;
    while (current != NULL) {
        toBeDeleted = current;
        current = current->next;
        free(toBeDeleted->firstName);
        free(toBeDeleted->familyName);
        free(toBeDeleted);
    }
    return current;
}
